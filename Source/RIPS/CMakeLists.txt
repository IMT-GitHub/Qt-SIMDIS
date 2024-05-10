cmake_minimum_required(VERSION 3.14)

project(RIPS LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)



set(RIPS_SRC_FILES
    main.cpp
    src/RipsManager.cpp
)

set(RIPS_INCLUDE_FILES
  include/RipsManager.h  
)

set(RIPS_COMMON_FILES
  Common/DCSServer.h  
  Common/DCSServer.cpp
)

set(RIPS_CONSTANTS_INCLUDE_FILES
)

# .h files
set(RIPS_HEADERS_TO_MOC
)

add_executable(LTSS_RIPS ${RIPS_SRC_FILES} ${RIPS_INCLUDE_FILES} ${RIPS_COMMON_FILES})

set(RIPS_INCLUDE_PATH "${RIPS_SOURCE_DIR}/include" CACHE INTERNAL " RIPS Include Directory")

if(WIN32)
  message("WIN32 RIPS  CMake")
  target_link_libraries(LTSS_RIPS simNotify simCore simData simUtil 
    Qt${QT_VERSION_MAJOR}::Core 
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
    Comm
    DCS2)

  
else()

message("LINUX RIPS CMake")
target_link_libraries(LTSS_RIPS simNotify simCore simData simUtil 
    Qt${QT_VERSION_MAJOR}::Network
    #Qt${QT_VERSION_MAJOR}::Widgets
    Comm 
    DCS2)
endif()

  # import libraries  include
  include_directories(${RIPS_INCLUDE_PATH})
  include_directories(${COMM_INCLUDE_PATH})
  include_directories(${DCS2_INCLUDE_PATH})
  include_directories(${STD_INCLUDE_PATH})
  include_directories(${NET_INCLUDE_PATH})


# Set installation paths
install(TARGETS LTSS_RIPS 
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS LTSS_RIPS LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES $(RIPS_INCLUDE_FILES) DESTINATION RIPS/include)
install(FILES DESTINATION Comm/include)