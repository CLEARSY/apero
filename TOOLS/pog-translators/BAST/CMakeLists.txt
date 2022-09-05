cmake_minimum_required(VERSION 3.16)
project(BAST)

find_package(Qt5Core)
find_package(Qt5Xml)

set(AST_HEADERS
    btype.h
    vars.h
    expr.h
    pred.h
    gpred.h
    exprDesc.h
    predDesc.h
    subst.h
    exprReader.h
    predReader.h
    gpredReader.h
    substReader.h
    exprWriter.h
    predWriter.h
    hashutil.h
)

set(AST_SRC
    btype.cpp
    vars.cpp
    expr.cpp
    pred.cpp
    gpred.cpp
    subst.cpp
    exprReader.cpp
    predReader.cpp
    gpredReader.cpp
    substReader.cpp
    exprWriter.cpp
    predWriter.cpp
)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(BAST
    ${AST_SRC}
    ${AST_HEADERS}
)

target_link_libraries(BAST Qt5::Core Qt5::Xml)
