#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <tuple>

#include "BufferManager.h"
#include "CatalogManager.h"
#include "GlobalData.h"
#include "Tokenizer.h"
#include "minisql.h"

CM::CatalogManager* MiniSQL::catalogManager = nullptr;
RM::RecordManager* MiniSQL::recordManager = nullptr;
IM::IndexManager* MiniSQL::indexManager = nullptr;

void MiniSQL::init() {
    catalogManager = new CM::CatalogManager;
    recordManager = new RM::RecordManager;
    //    indexManager = IM::IndexManager;
}
void MiniSQL::clean_up() {
    delete catalogManager;
    delete recordManager;
    //    delete indexManager;
}

CM::CatalogManager& MiniSQL::get_catalog_manager() { return *catalogManager; }
RM::RecordManager& MiniSQL::get_record_manager() { return *recordManager; }
IM::IndexManager& MiniSQL::get_index_manager() { return *indexManager; }

void write() {
    CM::CatalogManager cm;
    std::string s("book");
    CM::table& t = cm.get_table(s);

    Records data;
    data.push_back({std::to_string(1), "book1", "author1", std::to_string(12.4f)});
    data.push_back({std::to_string(2), "book2", "author2", std::to_string(1989.1f)});
    data.push_back({std::to_string(3), "sdgsg", "auther", std::to_string(1989.120f)});
    data.push_back({std::to_string(4), "sfhshsh", "sgshd", std::to_string(1989.0f)});
    data.push_back({std::to_string(5), "assdjfj", "CCCCCCCCCCCCCCCCCCC", std::to_string(1989.0f)});
    data.push_back({std::to_string(6), "fgjsfrkjsrk", "BBBBB", std::to_string(1989.0f)});
    data.push_back({std::to_string(7), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(8), "book1", "author1", std::to_string(12.4f)});
    data.push_back({std::to_string(9), "book2", "author2", std::to_string(1989.1f)});
    data.push_back({std::to_string(10), "sdgsg", "auther", std::to_string(1989.120f)});
    data.push_back({std::to_string(11), "sfhshsh", "sgshd", std::to_string(1989.0f)});
    data.push_back({std::to_string(12), "assdjfj", "CCCCCCCCCCCCCCCCCC", std::to_string(1989.0f)});
    data.push_back({std::to_string(13), "fgjsfrkjsrk", "BBBBB", std::to_string(1989.0f)});
    data.push_back({std::to_string(14), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(15), "book1", "author1", std::to_string(12.4f)});
    data.push_back({std::to_string(16), "book2", "author2", std::to_string(1989.1f)});
    data.push_back({std::to_string(17), "sdgsg", "auther", std::to_string(1989.120f)});
    data.push_back({std::to_string(18), "sfhshsh", "sgshd", std::to_string(1989.0f)});
    data.push_back({std::to_string(19), "assdjfj", "CCCCCCCCCCCCCC", std::to_string(1989.0f)});
    data.push_back({std::to_string(20), "fgjsfrkjsrk", "BBBBB", std::to_string(1989.0f)});
    data.push_back({std::to_string(21), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(22), "book1", "author1", std::to_string(12.4f)});
    data.push_back({std::to_string(23), "book2", "author2", std::to_string(1989.1f)});
    data.push_back({std::to_string(24), "sdgsg", "auther", std::to_string(1989.120f)});
    data.push_back({std::to_string(25), "sfhshsh", "sgshd", std::to_string(1989.0f)});
    data.push_back({std::to_string(26), "assdjfj", "CCCCCCCCCCCCCCCCCC", std::to_string(1989.0f)});
    data.push_back({std::to_string(27), "fgjsfrkjsrk", "BBBBB", std::to_string(1989.0f)});
    data.push_back({std::to_string(28), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(29), "fgjsfrkjsrk", "BBBBB", std::to_string(1989.0f)});
    data.push_back({std::to_string(30), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(31), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(32), "sgfjsrfjkrstykrsk", "AAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(33), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});

    char zero[256] = {0};

    std::ofstream os;
    os.open(t.name, std::ios::out | std::ios::binary);

    for (const auto& row : data) {
        for (unsigned int i = 0; i < t.NOF; ++i) {
            switch (t.fields[i].type) {
                case DataType::INT: {
                    int tmp = std::stoi(row[i]);
                    os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
                    break;
                }
                case DataType::CHAR_N: {
                    os.write(row[i].c_str(), row[i].size());
                    os.write(zero, t.fields[i].N - row[i].size());
                    break;
                }
                case DataType::FLOAT: {
                    float tmp = std::stof(row[i]);
                    os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
                    break;
                }
            }
        }
        int _ = 1;
        os.write(reinterpret_cast<const char*>(&_), BM::RECORD_TAIL_SIZE);
    }

    os.close();
}

int main() {
    MiniSQL::init();
    write();

    BM::BufferManager bm;
    CM::CatalogManager cm;

    for (int i = 0; i < 30; ++i) {
        std::string s("book");
        void* data = bm.read(s, i);
        int bookid = *static_cast<int*>(data);
        std::string title(static_cast<char*>(data) + 4, std::min((int)std::strlen(static_cast<char*>(data) + 4), 20));
        std::string author(static_cast<char*>(data) + 24,
                           std::min((int)std::strlen(static_cast<char*>(data) + 24), 20));
        float price = *(reinterpret_cast<float*>(static_cast<char*>(data) + 44));
        std::cout << bookid << " " << title << " " << author << " " << price << std::endl;
    }

    int idx;
    Records data;
    data.push_back({std::to_string(34), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(35), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(36), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(37), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(38), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(39), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(40), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(41), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(42), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(43), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(44), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(45), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(46), "sgfjsrfjkrstykrsk", "AAAAAA", std::to_string(1989.0f)});
    data.push_back({std::to_string(47), "cccsrsdgstykrsk", "AAAAAA", std::to_string(1239.0f)});
    data.push_back({std::to_string(48), "cccsrsdgstykrsk", "AAAAAA", std::to_string(1.0f)});
    data.push_back({std::to_string(49), "cccsrsdgstykrsk", "AAAAAA", std::to_string(2.0f)});
    data.push_back({std::to_string(50), "cccsrsdgstykrsk", "AAAAAA", std::to_string(3.0f)});
    data.push_back({std::to_string(51), "cccsrsdgstykrsk", "AAAAAA", std::to_string(1239.0f)});
    data.push_back({std::to_string(52), "cccsrsdgstykrsk", "AAAAAA", std::to_string(4.0f)});
    data.push_back({std::to_string(53), "cccsrsdgstykrsk", "AAAAAA", std::to_string(5.0f)});

    uint32_t offset;
    for (const auto& r : data) {
        std::tie(offset, idx) = bm.append_record("book", r);
        std::clog << offset << std::endl;
    }
    bm.save(idx);

    data.clear();
    data.push_back({std::to_string(54), "cccsrsdgstykrsk", "AAAAAA", std::to_string(1239.0f)});
    data.push_back({std::to_string(55), "cccsrsdgstykrsk", "AAAAAA", std::to_string(6.0f)});
    std::tie(offset, idx) = bm.append_record("book", data[0], 0);
    std::clog << offset << std::endl;
    bm.save(idx);

    std::string s("book");
    for (int i = 0; i < 2; ++i) {
        void* data = bm.read(s, i);
        int bookid = *static_cast<int*>(data);
        std::string title(static_cast<char*>(data) + 4, std::min((int)std::strlen(static_cast<char*>(data) + 4), 20));
        std::string author(static_cast<char*>(data) + 24,
                           std::min((int)std::strlen(static_cast<char*>(data) + 24), 20));
        float price = *(reinterpret_cast<float*>(static_cast<char*>(data) + 44));
        std::cout << bookid << " " << title << " " << author << " " << price << std::endl;
    }

    CM::table t;
    t.name = "test";
    t.primaryKeyID = 0;
    t.fields.push_back({"id", DataType::INT, 4, true});
    t.NOF = 1;
    t.sizePerTuple = 4;
    if (cm.create_table(t)) {
        std::clog << "create table " << t.name << " successful!" << std::endl;
    }
    std::cout << t.name << std::endl;

    CM::index id;
    id.name = "testIndex";
    id.onTableName = "test";
    id.onFieldID = 0;
    if (cm.create_index(id)) {
        std::clog << "create index " << id.name << " successful!" << std::endl;
    }

    cm.save();

    RM::RecordManager rm;
    auto& table = MiniSQL::get_catalog_manager().get_table(s);
    rm.create_table(s);

    std::string colName = "bookid";
    std::string operand = "45";
    std::string cond = ">=";
    Records records;
    rm.select(s, colName, operand, cond, records);

    int rowSize = records[0].size();
    std::cout << std::setw(20) << "┌──────────────────────┬";
    for (int i = 1; i < rowSize - 1; ++i)
        std::cout << std::setw(20) << "─────────────────────┬";
    std::cout << std::setw(20) << "─────────────────────┐" << std::endl;

    std::cout << "│ " << std::setw(20) << table.fields[0].name << " │";
    for (int i = 1; i < table.fields.size(); ++i) {
        std::cout << std::setw(20) << table.fields[i].name << " │";
    }
    std::cout << std::endl;

    std::cout << std::setw(20) << "├──────────────────────┼";
    for (int i = 1; i < rowSize - 1; ++i)
        std::cout << std::setw(20) << "─────────────────────┼";
    std::cout << std::setw(20) << "─────────────────────┤" << std::endl;

    for (const auto& row : records) {
        std::cout << "│ " << std::setw(20) << row[0] << " │";
        for (int i = 1; i < row.size(); ++i) {
            std::cout << std::setw(20) << row[i] << " │";
        }
        std::cout << std::endl;
    }
    std::cout << std::setw(20) << "└──────────────────────┴";
    for (int i = 1; i < rowSize - 1; ++i)
        std::cout << std::setw(20) << "─────────────────────┴";
    std::cout << std::setw(20) << "─────────────────────┘";
    return 0;
}