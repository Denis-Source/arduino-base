cmake_minimum_required(VERSION 3.16)

# Project configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Project setup
project(arduino_base LANGUAGES C CXX ASM)
set(PROJECT_PATH "${CMAKE_SOURCE_DIR}")
if (NOT COM_PORT)
    message(FATAL_ERROR "Please set the COM_PORT variable")
endif ()


# Target CPU
set(TARGET_CPU atmega328p)
set(TARGET_CPU_CLOCKS 16000000)

# Toolchain
set(AVR_TOOLCHAIN_PATH "${PROJECT_PATH}/tools/avr_toolchain")
set(CMAKE_C_COMPILER "${AVR_TOOLCHAIN_PATH}/bin/avr-gcc.exe")
set(CMAKE_CXX_COMPILER "${AVR_TOOLCHAIN_PATH}/bin/avr-g++.exe")
set(CMAKE_ASM_COMPILER "${AVR_TOOLCHAIN_PATH}/bin/avr-as.exe")
set(ELF_TO_HEX_EXECUTABLE "${AVR_TOOLCHAIN_PATH}/bin/avr-objcopy.exe")

# Flashing
set(AVR_DUDE_EXECUTABLE "${PROJECT_PATH}/tools/avrdude/bin/avrdude.exe")
set(AVR_DUDE_CONF_PATH "${PROJECT_PATH}/tools/avrdude/etc/avrdude.conf")

# Arduino core library
set(ARDUINO_CORE_PATH "${PROJECT_PATH}/external/arduino")
# Pin definitions based on variant
set(ARDUINO_VARIANT_PATH "${PROJECT_PATH}/external/variants/standard")
file(GLOB CORE_FILES
        ${ARDUINO_CORE_PATH}/*.c
        ${ARDUINO_CORE_PATH}/*.cpp
)

# Executable
add_executable(${PROJECT_NAME} src/main.cpp ${CORE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${ARDUINO_CORE_PATH}
        ${ARDUINO_VARIANT_PATH}
)

# Preprocessor, compiler and linker flags
target_compile_features(${PROJECT_NAME} PRIVATE c_std_11 cxx_std_17)

# Preprocessor
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "__AVR_${TARGET_CPU}__"
        "F_CPU=${TARGET_CPU_CLOCKS}L"
)
# Compiler
target_compile_options(${PROJECT_NAME} PRIVATE
        -mmcu=${TARGET_CPU}
        -Os
        -Wall
        -Wextra
        -ffunction-sections
        -fdata-sections
)
# Linker
target_link_options(${PROJECT_NAME} PRIVATE
        -mmcu=${TARGET_CPU}
        -Wl,--gc-sections
        -Wl,-Map=${PROJECT_NAME}.map
)

# Flashing shortcut
add_custom_target(flash
        COMMAND ${AVR_DUDE_EXECUTABLE}
        -C ${AVR_DUDE_CONF_PATH}
        -p ${TARGET_CPU}
        -c arduino
        -b57600
        -P ${COM_PORT}
        -F
        -D
        -U
        flash:w:${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex:a
        DEPENDS ${PROJECT_NAME}
        COMMENT "Flashing to ${TARGET_CPU}"
)

# Before flashing ELF should be converted to HEX
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${ELF_TO_HEX_EXECUTABLE}
        -O ihex
        -R .eeprom
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
        COMMENT "Generating HEX file"
)
