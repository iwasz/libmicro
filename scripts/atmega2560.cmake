SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR avr)

SET(CMAKE_C_FLAGS "-mmcu=atmega2560 -std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-mmcu=atmega2560 -std=c++17 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "cxx compiler flags")
SET (CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections" CACHE INTERNAL "exe link flags")

ADD_DEFINITIONS ("-D__AVR_ATmega2560__")
