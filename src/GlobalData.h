//
// Created by rasp on 19-6-5.
//

#ifndef DB_MINISQL_GLOBALDATA_H
#define DB_MINISQL_GLOBALDATA_H

#include <string>
#include <vector>

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

const int BLOCK_SIZE = 8192;

enum DataType { INT, CHAR_N, FLOAT };

namespace CM {
    typedef struct field_ {
        std::string name;
        DataType type;
        int N = -1;
        bool isUnique = false;
    } field;

    typedef struct table_ {
        std::string name;      // 表名
        uint32_t NOF;          // field的数量
        uint32_t primaryKeyID; // 主键所在field的序号（从0开始）
        uint32_t sizePerTuple; // 每一个元组所占的空间（bytes）
        std::vector<field> fields;
    } table;

    typedef struct index_ {
        std::string name;       // 索引名
        std::string onableName; // 索引所在的表
        std::string onFieldID;  // 单值索引所在的field序号（从0开始）
    } index;

}

namespace BM {
    const int BUF_NUM = 2; // buffer数量

    typedef struct buffer_ {
        uint32_t beginOffset; // 该buffer包含多个tuple的最低offset
        uint32_t endOffset;
        char buf[BLOCK_SIZE];
        uint8_t accessTimes; // 访问次数
        bool isModified;     // 数据是否被修改过
        bool isPinned;       // buffer是否被锁定
        bool inUse;          // buffer是否正在被使用
    } buffer;
}

#endif // DB_MINISQL_GLOBALDATA_H
