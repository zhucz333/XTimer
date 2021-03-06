﻿CMAKE_MINIMUM_REQUIRED (VERSION 3.5)

PROJECT (XTimer CXX)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

IF(USE_32BITS)				
	MESSAGE(STATUS "Using 32bits")
	SET(ARCH "32")
ELSE()	
	MESSAGE(STATUS "Using 64bits")
	SET(ARCH "64")
ENDIF()

SET(SRC_ROOT ${PROJECT_SOURCE_DIR}/..)
SET(XTIMER_COMMONS ${SRC_ROOT})

SET(INC_PATH ${XTIMER_COMMONS}/include)
SET(LIB_PATH ${XTIMER_COMMONS}/lib)
SET(BIN_PATH ${XTIMER_COMMONS}/bin)			
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_PATH})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIB_PATH})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH})

SET(TARGETBIN XTimer)
FILE(GLOB_RECURSE XTIMER_HEADERS *.h)
AUX_SOURCE_DIRECTORY(. XTIMER_SRCS)

ADD_LIBRARY(${TARGETBIN} STATIC ${XTIMER_HEADERS} ${XTIMER_SRCS})

SET_TARGET_PROPERTIES(${TARGETBIN} PROPERTIES OUTPUT_NAME "${TARGETBIN}${ARCH}$<$<CONFIG:Debug>:D>")

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
	ADD_DEFINITIONS(-DDEBUG)
ENDIF()

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "Current platform: Linux ")
	
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -O0")
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")	
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -pthread")
	
	IF(USE_32BITS)				
		MESSAGE(STATUS "Using 32bits")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")		
	ELSE()	
		MESSAGE(STATUS "Using 64bits")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	ENDIF()

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "Current platform: Windows")
	
	SET(CMAKE_CONFIGURATION_TYPES Debug Release)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

INSTALL(FILES XTimer.h DESTINATION ${INC_PATH})

ADD_SUBDIRECTORY(XTimerTest)
