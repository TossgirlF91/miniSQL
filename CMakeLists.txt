cmake_minimum_required(VERSION 3.10)
project(DB_miniSQL)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fsanitize=address")

if (WIN32)
    message(STATUS ">>> Win")
    add_definitions(-DWIN32)
endif ()

if (UNIX)
    message(STATUS ">>> UNIX")
    add_definitions(-DUNIX)
endif ()

find_package(RapidJSON CONFIG REQUIRED)


add_executable(MiniSQL src/minisql.cpp src/minisql.h src/GlobalData.h src/BufferManager.cpp
        src/BufferManager.h src/CatalogManager.cpp src/CatalogManager.h src/Api.cpp src/Api.h
        src/Tokenizer.cpp src/Tokenizer.h src/Interpreter.cpp src/Interpreter.h
        src/RecordManager.cpp src/RecordManager.h src/BTree.h src/BTree.cpp src/BNode.cpp src/BNode.h)

target_include_directories(MiniSQL PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
