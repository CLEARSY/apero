project(PPTRANSSMT)

find_package(Qt5Core)
find_package(Qt5Xml)

set(PPTRANSSMT_HEADERS
    utils.h
    decomposition.h
    ppTrans.h
    ppTransIncr.h
    ppTransNonIncr.h
    )

set(PPTRANSSMT_SRC
    utils.cpp
    decomposition.cpp
    ppTrans.cpp
    ppTransIncr.cpp
    ppTransNonIncr.cpp
    ppTransSmt.cpp
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${BAST_SOURCE_DIR}
    ${POGLoader_SOURCE_DIR}
    )

add_executable(ppTransSmt
    ${PPTRANSSMT_SRC}
    )

target_link_libraries(ppTransSmt POGLoader BAST Qt5::Core Qt5::Xml)
