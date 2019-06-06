//
// Created by rasp on 19-6-5.
//

#include "CatalogManager.h"
#include <fstream>
#include <iostream>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

using namespace rapidjson;

CM::CatalogManager::CatalogManager() {
    std::ifstream in("db.json");
    IStreamWrapper isw(in);
    d.ParseStream(isw);

    for (const auto& m : d["database"].GetObject()) {
        if (strncmp(m.name.GetString(), "tables", 5) == 0) {
            for (const auto& v : m.value.GetArray()) {
                std::cout << v.GetString() << std::endl;
                table tmp;
                tmp.name = v.GetString();
                tables.push_back(tmp);
            }
        } else if (strncmp(m.name.GetString(), "indices", 7) == 0) {
            for (const auto& v : m.value.GetArray()) {
                std::cout << v.GetString() << std::endl;
                index tmp;
                tmp.name = v.GetString();
                indices.push_back(tmp);
            }
        }
    }

    auto itrJ = d["table"].MemberBegin();
    auto itrT = tables.begin();
    while (itrJ != d["table"].MemberEnd()) {
        for (const auto& item : itrJ->value.GetObject()) {
            if (strncmp(item.name.GetString(), "NOF", 3) == 0) {
                itrT->NOF = item.value.GetInt();
            } else if (strncmp(item.name.GetString(), "primaryKeyID", 12) == 0) {
                itrT->primaryKeyID = item.value.GetInt();
                itrT->fields[itrT->primaryKeyID].isUnique = true;
            } else if (strncmp(item.name.GetString(), "uniqueIDs", 9) == 0) {
                for (const auto& v : item.value.GetArray()) {
                    itrT->fields[v.GetInt()].isUnique = true;
                }
            } else if (strncmp(item.name.GetString(), "field", 5) == 0) {
                for (const auto& v : item.value.GetArray()) {
                    field f;
                    f.name = v.GetArray()[0].GetString();
                    std::string type = v.GetArray()[1].GetString();
                    if (type == "INT") {
                        f.type = DataType::INT;
                        f.N = 4;
                    } else if (type == "CHAR_N") {
                        f.type = DataType::CHAR_N;
                        f.N = v.GetArray()[2].GetInt();
                    } else if (type == "FLOAT") {
                        f.type = DataType::FLOAT;
                        f.N = 4;
                    }
                    itrT->fields.push_back(f);
                }
            }
        }
        uint32_t sizePerTuple = 0;
        for (const auto& f : itrT->fields) {
            sizePerTuple += f.N;
        }
        itrT->sizePerTuple = sizePerTuple;
        itrJ++, itrT++;
    }

    itrJ = d["index"].MemberBegin();
    auto itrI = indices.begin();
    while (itrJ != d["index"].MemberEnd()) {
        for (const auto& item : itrJ->value.GetObject()) {
            if (strncmp(item.name.GetString(), "onTable", 7) == 0) {
                itrI->onableName = item.value.GetString();
            } else if (strncmp(item.name.GetString(), "onFieldID", 9) == 0) {
                itrI->onFieldID = item.value.GetInt();
            }
        }
        itrJ++, itrI++;
    }
}

CM::CatalogManager::~CatalogManager() { save(); }

CM::table& CM::CatalogManager::getTable(std::string name) {
    for (auto& t : tables) {
        if (t.name == name) {
            return t;
        }
    }
    throw(std::runtime_error("CM getTable: table name not found!"));
}
void CM::CatalogManager::save() {
    std::ofstream ofs("db.json");
    OStreamWrapper osw(ofs);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}
