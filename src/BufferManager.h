//
// Created by rasp on 19-6-5.
//

#ifndef DB_MINISQL_BUFFERMANAGER_H
#define DB_MINISQL_BUFFERMANAGER_H

#include "CatalogManager.h"
#include "GlobalData.h"
#include <string>

namespace BM {
    class BufferManager {
    public:
        BufferManager();
        ~BufferManager();

        // 从磁盘读取对应offset的数据
        void* read(std::string dbName, uint32_t offset);

        // 保存buffer[idx]内容到磁盘
        void save(size_t idx);

        // 标记buffer[idx]已被修改
        void set_modified(size_t idx);

        // 添加/覆盖一条record到buffer中，指定offset时直接覆盖数据，返回该record的offset和buffer的idx，buffer满时自动写回磁盘
        std::pair<uint32_t, int> append_record(std::string dbName, const Record& row, uint32_t offset = -1);

        // 删除一条record（填0），返回该record的offset和buffer的idx，buffer删空时自动写回磁盘
        void* delete_record(std::string dbName, uint32_t offset);

    private:
        int get_free_buffer();
        void copy_to_buffer(const Record& row, const CM::table& t, char* p) const;

        buffer* buf;
        CM::table* tables[BUF_NUM];
        CM::CatalogManager cm;
    };
}

#endif // DB_MINISQL_BUFFERMANAGER_H
