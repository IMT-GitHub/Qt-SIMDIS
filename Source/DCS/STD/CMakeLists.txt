project(STD)

set(STD_FILES
    inttypesc.h
    inttypes_ms.h
    iomanipc
    iostreamc
    mathc.h
    sstreamc
    statc.h
    stdint_ms.h
    stdioc.h
    stdlibc.h
    stringc.h
    swapbytes.h
    timec.h
)

add_custom_target(STD SOURCES ${STD_FILES})

set(STD_INCLUDE_PATH "${STD_SOURCE_DIR}" CACHE INTERNAL "STD Library Include Directory")

source_group("Header Files" FILES ${STD_FILES})

install(FILES ${STD_FILES} DESTINATION STD)

