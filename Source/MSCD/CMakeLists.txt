cmake_minimum_required(VERSION 3.5)

project(MSCD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
#find_package(OpenGL REQUIRED)

if(WIN32 AND MSVC)
    add_compile_options(/utf-8)
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        Common/global.cpp
        Common/global.h
        Common/define.h
        Common/struct.h
        Common/qdialogskin.cpp
        Common/qdialogskin.h
        Operation/operationmain.cpp
        Operation/operationmain.h
        Operation/operationmain.ui
        Operation/MissionControl/scenariocontrol.cpp
        Operation/MissionControl/scenariocontrol.h
        Operation/MissionControl/scenariocontrol.ui
        Operation/MissionControl/tdpslist.cpp
        Operation/MissionControl/tdpslist.h
        Operation/MissionControl/tdpslist.ui
        Operation/MissionControl/tdpslist_tab.cpp
        Operation/MissionControl/tdpslist_tab.h
        Operation/MissionControl/tdpslist_tab.ui        
        Operation/MissionControl/dcslist.cpp
        Operation/MissionControl/dcslist.h
        Operation/MissionControl/dcslist.ui
        Operation/MissionControl/dcslist_tab.cpp
        Operation/MissionControl/dcslist_tab.h
        Operation/MissionControl/dcslist_tab.ui
        Operation/ScenarioEdit/scenarioedit.cpp
        Operation/ScenarioEdit/scenarioedit.h
        Operation/ScenarioEdit/scenarioedit.ui
        Operation/ScenarioEdit/tdpsrecvconfig.cpp
        Operation/ScenarioEdit/tdpsrecvconfig.h
        Operation/ScenarioEdit/tdpsrecvconfig.ui
        Operation/ScenarioEdit/tdpseudatalist.cpp
        Operation/ScenarioEdit/tdpseudatalist.h
        Operation/ScenarioEdit/tdpseudatalist.ui
        Operation/ScenarioEdit/dcsrecvconfig.cpp
        Operation/ScenarioEdit/dcsrecvconfig.h
        Operation/ScenarioEdit/dcsrecvconfig.ui
        DataMngr/datamngr.cpp
        DataMngr/datamngr.h
        DataMngr/qlmparsser.cpp
        DataMngr/qlmparsser.h
        DataMngr/dcsparsser.cpp
        DataMngr/dcsparsser.h
        Display/huddisplay.cpp
        Display/huddisplay.h
        Display/huddisplay_legend.h
        ScenarioMngr/scenariomngr.cpp
        ScenarioMngr/scenariomngr.h
        ../Interface/recvqlm.cpp
        ../Interface/recvqlm.h
        ../Interface/recvdcs.cpp
        ../Interface/recvdcs.h
        #MyMainWindow.h
        #ExampleQt.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MSCD
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()    
    add_executable(MSCD
        ${PROJECT_SOURCES}
        MSCD.qrc
    )
endif()


include_directories(${DCS2_INCLUDE_PATH})
include_directories(${STD_INCLUDE_PATH})
include_directories(${NET_INCLUDE_PATH})

target_link_libraries(MSCD PRIVATE Qt${QT_VERSION_MAJOR}::Widgets simVis simUtil simQt DCS2 Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(MSCD PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MSCD
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MSCD)
endif()
