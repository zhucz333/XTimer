﻿CMAKE_MINIMUM_REQUIRED (VERSION 3.5)

PROJECT (XTimerTest CXX)

SET(TARGETTEST XTimerTest)

FILE(GLOB_RECURSE XTIMERTEST_HEADERS *.h)
AUX_SOURCE_DIRECTORY(. XTIMERTEST_SRCS)

ADD_EXECUTABLE(${TARGETTEST} ${XTIMERTEST_HEADERS} ${XTIMERTEST_SRCS})

INCLUDE_DIRECTORIES(${INC_PATH})
LINK_DIRECTORIES(${LIB_PATH})
TARGET_LINK_LIBRARIES(${TARGETTEST} ${TARGETBIN})

SET_TARGET_PROPERTIES(${TARGETTEST} PROPERTIES OUTPUT_NAME "${TARGETTEST}${ARCH}$<$<CONFIG:Debug>:D>")

