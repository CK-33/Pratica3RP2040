cmake_minimum_required(VERSION 3.13)

# Inicializa o SDK do Pico (deve ser feito antes de project())
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(servo_joystick C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(servo_joystick
    servo_joystick.c
)

# Habilita saída USB e UART para printf (debug)
pico_enable_stdio_usb(servo_joystick 1)
pico_enable_stdio_uart(servo_joystick 0)

# Linka as bibliotecas necessárias
target_link_libraries(servo_joystick
    pico_stdlib
    hardware_adc
    hardware_pwm
)

# Gera arquivos .uf2, .hex, .bin etc.
pico_add_extra_outputs(servo_joystick)
