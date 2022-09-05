cmake_minimum_required(VERSION 3.16)
project(POGLoader)

find_package(Qt5Core)
find_package(Qt5Xml)

set(POG_HEADERS
    pog.h
)

set(POG_SRC
    pog.cpp
)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
	${BAST_SOURCE_DIR}
)

add_library(POGLoader
    ${POG_SRC}
    ${POG_HEADERS}
)

target_link_libraries(POGLoader BAST Qt5::Core Qt5::Xml)

