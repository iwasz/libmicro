CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE OFF)

# hardcoded!
SET (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_C_FLAGS "-std=gnu99 -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-std=c++14 -Wall" CACHE INTERNAL "cxx compiler flags")
ADD_DEFINITIONS ("-DUNIT_TEST")

PROJECT (unittest)
ENABLE_TESTING ()
INCLUDE_DIRECTORIES("../src/")

SET(SOURCES)
LIST (APPEND SOURCES "../src/storage/IRandomAccessStorage.h")
LIST (APPEND SOURCES "../src/storage/ICircullarQueueStorage.h")
LIST (APPEND SOURCES "../src/storage/FlashEepromStorage.h")
LIST (APPEND SOURCES "../src/storage/FileRandomAccessStorage.h")
LIST (APPEND SOURCES "../src/ErrorHandler.h")
LIST (APPEND SOURCES "../src/ErrorHandler.c")
LIST (APPEND SOURCES "../src/collection/CharacterCircularQueue.h")
LIST (APPEND SOURCES "00StorageTest.cc")
LIST (APPEND SOURCES "01CcqTest.cc")

ADD_EXECUTABLE (unittest ${SOURCES})
ADD_TEST (unittest unittest)
