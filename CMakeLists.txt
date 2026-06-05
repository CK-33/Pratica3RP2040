cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)

set(PICO_SDK_PATH "C:/Users/thyag/.pico-sdk/sdk/2.2.0")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(PWM_CONV_AD C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(PWM_CONV_AD
    PWM_CONV_AD.c
)

pico_enable_stdio_usb(PWM_CONV_AD 1)
pico_enable_stdio_uart(PWM_CONV_AD 0)

target_link_libraries(PWM_CONV_AD
    pico_stdlib
    hardware_adc
    hardware_pwm
)

set(picotool_DIR "C:/Users/thyag/.pico-sdk/picotool/2.2.0-a4/picotool")

pico_add_extra_outputs(PWM_CONV_AD)
