cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pwm_geber_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(pwm_geber pwm_geber.c )

# Bibliotheksverzeichnisse
add_subdirectory(../bibliotheken/sh_1106 [subproject/sh_1106])
add_subdirectory(../bibliotheken/drehgeber [subproject/drehgeber])
add_subdirectory(../bibliotheken/momefilo_flash [subproject/momefilo_flash])

target_link_libraries(pwm_geber
	sh_1106
	drehgeber
	momefilo_flash
	pico_time
	hardware_irq
	hardware_pwm
	pico_stdlib)
pico_add_extra_outputs(pwm_geber)
