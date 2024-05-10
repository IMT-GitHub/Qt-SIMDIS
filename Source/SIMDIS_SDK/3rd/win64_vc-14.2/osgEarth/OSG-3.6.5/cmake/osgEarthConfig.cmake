set(OSGEARTH_VERSION 3.1.0)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was osgEarthConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../simdisfs/osgEarth/win64_vc-14.2/OSG-3.6.5" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(XPREFIX OSGEARTH)


if (osgEarth_USE_STATIC_LIBS)
    set(XPREFIX ${XPREFIX}_STATIC)
endif()

set(osgEarth_DEFINITIONS ${${XPREFIX}_CFLAGS})

find_path(osgEarth_INCLUDE_DIR
    NAMES osgEarth/rtree.h
    HINTS ${${XPREFIX}_INCLUDE_DIRS}
)

set(OSGEARTH_NAMES osgEarth)

find_library(osgEarth_LIBRARY
    NAMES ${OSGEARTH_NAMES}
    HINTS ${${XPREFIX}_LIBRARY_DIRS}
)

set(osgEarth_LIBRARIES    ${osgEarth_LIBRARY})
set(osgEarth_LIBRARY_DIRS ${${XPREFIX}_LIBRARY_DIRS})
set(osgEarth_LIBRARY_DIR  ${osgEarth_LIBRARY_DIRS})
set(osgEarth_INCLUDE_DIRS ${osgEarth_INCLUDE_DIR})
set(osgEarth_LDFLAGS      ${${XPREFIX}_LDFLAGS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(osgEarth DEFAULT_MSG
    osgEarth_LIBRARY
    osgEarth_INCLUDE_DIR
)

if (osgEarth_FOUND)
    string (REPLACE ";" " " osgEarth_LDFLAGS "${osgEarth_LDFLAGS}")

    add_library(osgEarth UNKNOWN IMPORTED)

    set_target_properties(osgEarth
      PROPERTIES
      IMPORTED_LOCATION             "${osgEarth_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${osgEarth_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES      "${osgEarth_LDFLAGS}"
      INTERFACE_COMPILE_OPTIONS     "${osgEarth_DEFINITIONS}"
    )
endif()

message(STATUS "osgEarth_FOUND: ${osgEarth_FOUND}")
message(STATUS "osgEarth_INCLUDE_DIRS: ${osgEarth_INCLUDE_DIRS}")
message(STATUS "osgEarth_LIBRARIES: ${osgEarth_LIBRARIES}")
message(STATUS "osgEarth_LDFLAGS: ${osgEarth_LDFLAGS}")
message(STATUS "osgEarth_DEFINITIONS: ${osgEarth_DEFINITIONS}")

check_required_components(osgEarth)
