cmake_minimum_required(VERSION 2.8.11)

if(!UNIX)
    message(FATAL_ERROR "This script only supports Unix-like platforms")
endif(!UNIX)

project(PlanetGenerator)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

set(Qt5Libs Qt5::Widgets)

if(EXISTS "/usr/local/lib/OGRE/cmake")

    set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

elseif(EXISTS "/usr/lib/OGRE/cmake")

    set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

elseif(EXISTS "/usr/share/OGRE/cmake/modules") # Ubuntu

    set(CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/modules/;${CMAKE_MODULE_PATH}")

# Just warn here, Fedora does not require this to succeed.
else ()
    message(WARNING "Failed to find path for Ogres cmake-module.")

endif(EXISTS "/usr/local/lib/OGRE/cmake")


# By default build a release. Pass "-DCMAKE_BUILD_TYPE=Debug" to cmake to build debug binary
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin-dbg)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_DEBUG_POSTFIX "_d")

find_package(OGRE REQUIRED)
 
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
    message(SEND_ERROR "Failed to find OIS.")
endif()
 
# Find Boost
# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})

set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
# Components that need linking (NB does not include header-only components like bind)
set(OGRE_BOOST_COMPONENTS thread date_time)
find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
if (NOT Boost_FOUND)
    # Try again with the other type of libs
    set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
endif()
find_package(Boost QUIET)
 
# Set up referencing of Boost
include_directories(${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_NO_LIB)
set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
 
set(HDRS
    ../simplexnoise1234.h
    ../PSphere.h
    ../GeneratorFrameListener.h
    ../initOgre.h
    ../HeightMap.h
    ../CollisionTools.h
    ../Common.h
    ../ResourceParameter.h
    ../ObjectInfo.h
    ../testui2/mainwindow.h
    ../testui2/freqampdialog.h
    ../testui2/meshdialog.h
)
 
set(SRCS
    ../simplexnoise1234.cpp
    ../PSphere.cpp
    ../GeneratorFrameListener.cpp
    ../initOgre.cpp
    ../main.cpp
    ../HeightMap.cpp
    ../CollisionTools.cpp
    ../Common.cpp
    ../ResourceParameter.cpp
    ../ObjectInfo.cpp
    ../testui2/mainwindow.cpp
    ../testui2/freqampdialog.cpp
    ../testui2/meshdialog.cpp
)

set(UI_FILES
    ../testui2/mainwindow.ui
    ../testui2/freqampdialog.ui
    ../testui2/meshdialog.ui
)
source_group("UI Files" FILES ${UI_FILES})

qt5_wrap_ui(UI_HEADERS ${UI_FILES})
source_group("Generated UI Headers" FILES ${UI_HEADERS})

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_Overlay_INCLUDE_DIRS}
)
 
add_executable(PlanetGenerator ${HDRS} ${SRCS} ${UI_HEADERS} ${UI_FILES})

# Copy files necessary to run executable
add_custom_command(TARGET PlanetGenerator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../media $<TARGET_FILE_DIR:PlanetGenerator>/media)
add_custom_command(TARGET PlanetGenerator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/../resources.cfg $<TARGET_FILE_DIR:PlanetGenerator>)

# Add "_d" to debug-binary
set_target_properties(PlanetGenerator PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(PlanetGenerator ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${Qt5Libs})

set(CMAKE_CXX_FLAGS "-Wall")

install(TARGETS PlanetGenerator
        RUNTIME DESTINATION bin
        CONFIGURATIONS all)
 
