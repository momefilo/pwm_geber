cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pwm_geber_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(pwm_geber pwm_geber.c )

# Bibliotheksverzeichnisse

target_link_libraries(pwm_geber
	pico_stdlib)
pico_add_extra_outputs(pwm_geber)
