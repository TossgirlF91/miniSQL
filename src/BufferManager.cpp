//
// Created by rasp on 19-6-5.
//

#include <algorithm>
#include <cstring>
#include <iostream>

// POSIX api
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

#include "BufferManager.h"

void* BM::BufferManager::read(std::string dbName, uint32_t offset) {
    // 若buffer中已经缓存过数据，则直接返回buffer中的数据
    for (int i = 0; i < BUF_NUM; ++i) {
        if (buf[i].inUse && tables[i] != nullptr && tables[i]->name == dbName && buf[i].beginOffset <= offset &&
            buf[i].endOffset > offset) {
            buf[i].accessTimes += 1;
#ifndef NDEBUG
            std::clog << offset << " from buf" << std::endl;
#endif
            return buf[i].buf + tables[i]->sizePerTuple * (offset - buf[i].beginOffset);
        }
    }

    CM::table& t = cm.get_table(dbName);
    int fd = open(t.name.c_str(), O_RDONLY, S_IREAD);
    // 获取文件大小，以免endOffset越界
    uint32_t size = lseek(fd, 0, SEEK_END);

    if (offset >= size / t.sizePerTuple) {
        throw std::out_of_range("CM read: offset out of range!");
    }

    int i = get_free_buffer();

    tables[i] = &t;
    buf[i].inUse = true;
    buf[i].beginOffset = offset;
    buf[i].endOffset = std::min(offset + BLOCK_SIZE / t.sizePerTuple, size / t.sizePerTuple);
#ifndef NDEBUG
    std::clog << "beginOffset: " << buf[i].beginOffset << "; endOffset: " << buf[i].endOffset << std::endl;
#endif

    uint64_t totalOffet = t.sizePerTuple * offset;
    lseek(fd, totalOffet, SEEK_SET);
    ::read(fd, buf[i].buf, BLOCK_SIZE);
    close(fd);
    return buf[i].buf;
}

// 查找空余的buffer
int BM::BufferManager::get_free_buffer() {
    int i = 0;
    while (this->buf[i].inUse && i < BM::BUF_NUM)
        i++;

    // 若所有buffer都已使用，则替换最近访问次数最少的buffer
    if (i == BM::BUF_NUM) {
        int bufToBeReplace = 0;
        for (int j = 1; j < BM::BUF_NUM; ++j) {
            if (this->buf[j].accessTimes < this->buf[bufToBeReplace].accessTimes && !this->buf[j].isPinned) {
                bufToBeReplace = j;
            }
        }
#ifndef NDEBUG
        std::clog << "replace " << bufToBeReplace << std::endl;
#endif
        this->save(bufToBeReplace);
        i = bufToBeReplace;
    }
    return i;
}

BM::BufferManager::BufferManager() {
    buf = new buffer[BUF_NUM];
    for (int i = 0; i < BUF_NUM; ++i) {
        tables[i] = nullptr;
        buf[i].accessTimes = 0;
        buf[i].isPinned = buf[i].isModified = buf[i].inUse = false;
    }
}

BM::BufferManager::~BufferManager() {
    for (int i = 0; i < BUF_NUM; ++i) {
        save(i);
    }
    delete[] buf;
}

void BM::BufferManager::save(size_t idx) {
    // 将buffer标记为未使用，且如果buffer没有被修改，则直接返回
    if (!buf[idx].isModified) {
        buf[idx].accessTimes = 0;
        buf[idx].isPinned = buf[idx].isModified = buf[idx].inUse = false;
        tables[idx] = nullptr;
        return;
    }

    int fd = open(tables[idx]->name.c_str(), O_WRONLY, S_IWRITE | S_IREAD);
    lseek(fd, buf[idx].beginOffset * tables[idx]->sizePerTuple, SEEK_SET);
    // 写回时不能直接写BLOCK_SIZE，因为buffer的末尾部分并不完全
    write(fd, buf[idx].buf, (buf[idx].endOffset - buf[idx].beginOffset) * tables[idx]->sizePerTuple);
    close(fd);

    buf[idx].accessTimes = 0;
    buf[idx].isPinned = buf[idx].isModified = buf[idx].inUse = false;
    tables[idx] = nullptr;
}

void BM::BufferManager::set_modified(size_t idx) { buf[idx].isModified = true; }

std::pair<uint32_t, int> BM::BufferManager::append_record(std::string dbName, const Record& row, uint32_t offset) {
    if (offset != -1) {
        // 在buffer中寻找是否已有缓存
        for (int i = 0; i < BUF_NUM; ++i) {
            if (buf[i].inUse && tables[i] != nullptr && tables[i]->name == dbName && buf[i].beginOffset <= offset &&
                buf[i].endOffset > offset) {
                buf[i].accessTimes += 1;
#ifndef NDEBUG
                std::clog << "replace: " << offset << " from buf" << std::endl;
#endif
                buf[i].inUse = buf[i].isModified = true;
                // p为指向offset的指针
                char* p = buf[i].buf + tables[i]->sizePerTuple * (offset - buf[i].beginOffset);
                copy_to_buffer(row, *tables[i], p);
                return std::make_pair(offset, i);
            }
        }

        CM::table& t = cm.get_table(dbName);
        int fd = open(t.name.c_str(), O_RDONLY, S_IREAD);
        // 获取文件大小，以免endOffset越界
        uint32_t size = lseek(fd, 0, SEEK_END);

        if (offset >= size / t.sizePerTuple) {
            throw std::out_of_range("CM read: offset out of range!");
        }

        int i = get_free_buffer();

        tables[i] = &t;
        buf[i].inUse = buf[i].isModified = true;
        buf[i].beginOffset = offset;
        buf[i].endOffset = std::min(offset + BLOCK_SIZE / t.sizePerTuple, size / t.sizePerTuple);
#ifndef NDEBUG
        std::clog << "replace: beginOffset: " << buf[i].beginOffset << "; endOffset: " << buf[i].endOffset << std::endl;
#endif

        uint64_t totalOffet = t.sizePerTuple * offset;
        lseek(fd, totalOffet, SEEK_SET);
        ::read(fd, buf[i].buf, BLOCK_SIZE);
        close(fd);
        copy_to_buffer(row, *tables[i], buf[i].buf);
        return std::make_pair(offset, i);

    } else {
        CM::table& t = cm.get_table(dbName);
        int fd = open(t.name.c_str(), O_RDONLY, S_IREAD);
        uint32_t size = lseek(fd, 0, SEEK_END);
        uint32_t _endOffset = size / t.sizePerTuple;

        // 若buffer中已经有缓存数据，则直接在buffer中继续添加数据
        for (int i = 0; i < BUF_NUM; ++i) {
            if (buf[i].inUse && tables[i] != nullptr && tables[i]->name == dbName && buf[i].beginOffset == _endOffset) {
#ifndef NDEBUG
                std::clog << "append data to buf" << std::endl;
#endif
                char* p = buf[i].buf + ((buf[i].endOffset++ - buf[i].beginOffset) * t.sizePerTuple);
                copy_to_buffer(row, t, p);

                // buffer写满，写回磁盘
                if (buf[i].endOffset - buf[i].beginOffset >= BLOCK_SIZE / t.sizePerTuple) {
#ifndef NDEBUG
                    std::clog << "sync data to disk" << std::endl;
#endif
                    save(i);
                    return std::make_pair(buf[i].endOffset - 1, BUF_NUM);
                }

                return std::make_pair(buf[i].endOffset - 1, i);
            }
        }

#ifndef NDEBUG
        std::clog << "write to new offset: " << _endOffset << std::endl;
#endif
        int idx = get_free_buffer();
        buf[idx].accessTimes = 0;
        buf[idx].inUse = buf[idx].isModified = true;
        tables[idx] = &t;
        buf[idx].beginOffset = _endOffset;
        buf[idx].endOffset = buf[idx].beginOffset + 1;
        char* p = buf[idx].buf;
        copy_to_buffer(row, t, p);
        return std::make_pair(_endOffset, idx);
    }
}

void BM::BufferManager::copy_to_buffer(const Record& row, const CM::table& t, char* p) const {
    char zero[256] = {0};
    for (unsigned int i = 0; i < t.NOF; ++i) {
        switch (t.fields[i].type) {
            case INT: {
                int tmp = std::stoi(row[i]);
                memcpy(p, reinterpret_cast<const char*>(&tmp), sizeof(tmp));
                p += 4;
                break;
            }
            case CHAR_N: {
                memcpy(p, row[i].c_str(), row[i].size());
                p += row[i].size();
                memcpy(p, zero, t.fields[i].N - row[i].size());
                p += t.fields[i].N - row[i].size();
                break;
            }
            case FLOAT: {
                float tmp = std::stof(row[i]);
                memcpy(p, &tmp, sizeof(tmp));
                p += 4;
                break;
            }
        }
    }
}

void* BM::BufferManager::delete_record(std::string dbName, uint32_t offset) { return read(dbName, offset); }
