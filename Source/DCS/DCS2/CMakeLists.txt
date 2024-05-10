project(DCS2)
# Required to find ws32
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(DCS2_INCLUDE_FILES
    include/DCS.h
    include/DCSBaseData.h
    include/DCSBaseHeader.h
    include/DCSBeamData.h
    include/DCSBeamHeader.h
    include/DCSCategoryData.h
    include/DCSCom.h
    include/DCSConsoleLogger.h
    include/DCSCoordReferenceFrame.h
    include/DCSDiscover.h
    include/DCSEvent.h
    include/DCSFileLogger.h
    include/DCSGateData.h
    include/DCSGateHeader.h
    include/DCSGenericData.h
    include/DCSHeaderContainer.h
    include/DCSLogger.h
    include/DCSPlatformData.h
    include/DCSPlatformHeader.h
    include/DCSPoint.h
    include/DCSScenarioData.h
    include/DCSScenarioHeader.h
    include/DCSScopeData.h
    include/DCSStreamLogger.h
    include/DCSTimeData.h
    include/DCSTimeHeader.h
    include/DCSdefs.h
    include/DCSswapbytes.h
    include/DCSutils.h
)
set(DCS2_CONSTANTS_INCLUDE_FILES
    include/Constants/ConstantsTime.h
    include/Constants/ConstantsUnits.h
    include/Constants/ConstantsVariableType.h
)
set(DCS2_SRC_FILES
    src/DCSBaseData.cpp
    src/DCSBeamData.cpp
    src/DCSBeamHeader.cpp
    src/DCSCategoryData.cpp
    src/DCSCom.cpp
    src/DCSConsoleLogger.cpp
    src/DCSCoordReferenceFrame.cpp
    src/DCSDiscover.cpp
    src/DCSEvent.cpp
    src/DCSFileLogger.cpp
    src/DCSGateData.cpp
    src/DCSGateHeader.cpp
    src/DCSGenericData.cpp
    src/DCSHeaderContainer.cpp
    src/DCSLogger.cpp
    src/DCSPlatformData.cpp
    src/DCSPlatformHeader.cpp
    src/DCSScenarioData.cpp
    src/DCSScenarioHeader.cpp
    src/DCSScopeData.cpp
    src/DCSStreamLogger.cpp
    src/DCSTimeData.cpp
    src/DCSTimeHeader.cpp
    src/DCSutils.cpp
)

set(DCS2U_INCLUDE_FILES
  extensions/DCSServer.h
)
set(DCS2U_SRC_FILES
  extensions/DCSServer.cpp
)

# DCS2 core library
add_library(DCS2 STATIC ${DCS2_INCLUDE_FILES} ${DCS2_CONSTANTS_INCLUDE_FILES} ${DCS2_SRC_FILES})
set(DCS2_INCLUDE_PATH "${DCS2_SOURCE_DIR}/include" CACHE INTERNAL "DCS2 Library Include Directory")
add_definitions(${STD_DCS_DEFINITIONS})
include_directories(${DCS2_INCLUDE_PATH})
include_directories(${STD_INCLUDE_PATH})
include_directories(${NET_INCLUDE_PATH})
target_link_libraries(DCS2 NET)

# DCS2 Utility library (DCSServer)
add_library(DCS2U STATIC ${DCS2U_INCLUDE_FILES} ${DCS2U_SRC_FILES})
include_directories(extensions)
target_link_libraries(DCS2U DCS2)

# Simple Client
#add_executable(sclient examples/sclient/sclient.cpp)
#target_link_libraries(sclient DCS2)

# Simple Server
#add_executable(sserver examples/sserver/sserver.cpp)
#target_link_libraries(sserver DCS2)

# Simple Client Server
#add_executable(scserver examples/scserver/scserver.cpp)
#target_link_libraries(scserver DCS2U)

# Tutorial: DCSaux
#add_library(DCSaux tutorial/DCSaux/DCSaux.cpp tutorial/DCSaux/DCSaux.h)
#include_directories(tutorial/DCSaux)
#target_link_libraries(DCSaux DCS2)

# Tutorial: Example 1
#add_executable(example1 tutorial/example1/example1.cpp)
#target_link_libraries(example1 DCS2 DCSaux)
# Tutorial: Example 2
#add_executable(example2 tutorial/example2/example2.cpp)
#target_link_libraries(example2 DCS2 DCSaux)
# Tutorial: Example 3
#add_executable(example3 tutorial/example3/example3.cpp)
#target_link_libraries(example3 DCS2 DCSaux)
# Tutorial: Example 4
#add_executable(example4 tutorial/example4/example4.cpp)
#target_link_libraries(example4 DCS2 DCSaux)
# Tutorial: Example 5
#add_executable(example5 tutorial/example5/example5.cpp)
#target_link_libraries(example5 DCS2 DCS2U DCSaux)

# Set installation paths
install(TARGETS DCS2 DCS2U #DCSaux sclient sserver scserver
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES ${DCS2_INCLUDE_FILES} DESTINATION DCS2/include)
install(FILES ${DCS2_CONSTANTS_INCLUDE_FILES} DESTINATION DCS2/include/Constants)
install(FILES ${DCS2U_INCLUDE_FILES} DESTINATION DCS2/extensions)
install(FILES tutorial/DCSaux/DCSaux.h DESTINATION DCS2/tutorial/DCSaux)
