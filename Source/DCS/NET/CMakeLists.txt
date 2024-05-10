project(NET)

set(NET_INCLUDE_FILES
    include/SockAddrFactory.h
    include/SockAddrInet.h
    include/SockBroadcast.h
    include/SockCOM.h
    include/SockClient.h
    include/SockException.h
    include/SockIO.h
    include/SockInet.h
    include/SockMonitor.h
    include/SockMulticast.h
    include/SockServer.h
    include/SockTCP.h
    include/SockUDP.h
    include/SockUnicast.h
    include/net.h
    include/sockdefs.h
)
set(NET_SRC_FILES
    src/SockAddrFactory.cpp
    src/SockAddrInet.cpp
    src/SockBroadcast.cpp
    src/SockCOM.cpp
    src/SockClient.cpp
    src/SockException.cpp
    src/SockIO.cpp
    src/SockInet.cpp
    src/SockMonitor.cpp
    src/SockMulticast.cpp
    src/SockServer.cpp
    src/SockTCP.cpp
    src/SockUDP.cpp
    src/SockUnicast.cpp
)

# Create NET library
add_library(NET STATIC ${NET_INCLUDE_FILES} ${NET_SRC_FILES})
add_definitions(${STD_DEFINITIONS})
set(NET_INCLUDE_PATH "${NET_SOURCE_DIR}/include" CACHE INTERNAL "NET Library Include Directory")
include_directories(include)
include_directories(${STD_INCLUDE_PATH})
# Windows requires WinSock2
if(WIN32)
    target_link_libraries(NET ws2_32.lib)
endif(WIN32)

# Examples
add_executable(echocli examples/echocli.cpp)
add_executable(echoserv examples/echoserv.cpp)
add_executable(udpcli examples/udpcli.cpp)
add_executable(udpserv examples/udpserv.cpp)
add_executable(mctcs examples/mctcs.cpp)
target_link_libraries(echocli NET)
target_link_libraries(echoserv NET)
target_link_libraries(udpcli NET)
target_link_libraries(udpserv NET)
target_link_libraries(mctcs NET)

# Set installation paths
install(TARGETS NET echocli echoserv udpcli udpserv mctcs
    ARCHIVE DESTINATION ${NET_INSTALL_PREFIX}lib
    RUNTIME DESTINATION ${NET_INSTALL_PREFIX}bin
)
# if statement here for dual use in DCS2 as in PluginAPI
if(NOT NET_INSTALL_PREFIX)
    set(NET_INSTALL_PREFIX "NET/")
endif(NOT NET_INSTALL_PREFIX)
install(FILES ${NET_INCLUDE_FILES} DESTINATION ${NET_INSTALL_PREFIX}include)
