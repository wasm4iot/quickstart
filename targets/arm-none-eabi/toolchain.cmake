set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN arm-none-eabi)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

if(NOT DEFINED TOOLCHAIN_PREFIX)
        set(TOOLCHAIN_PREFIX "/usr")
        message(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${TOOLCHAIN} ${EXTRA_FIND_PATH})

set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++)