SET (DEVICE "STM32H743xx")
SET (CUBE_ROOT "$ENV{HOME}/workspace/STM32Cube_FW_H7_V1.3.1")
SET (STARTUP_CODE "${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h743xx.s")
SET (LINKER_SCRIPT "${CUBE_ROOT}/Projects/STM32H743ZI-Nucleo/Templates/SW4STM32/STM32H743ZI-Nucleo/STM32H743ZITx_FLASH_DTCMRAM.ld")

SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_C_FLAGS "-std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-std=c++17 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")

SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

INCLUDE_DIRECTORIES(${SUPPORT_FILES})
LINK_DIRECTORIES(${SUPPORT_FILES})
ADD_DEFINITIONS(-D${DEVICE})

INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32H7xx/Include/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Include/")
