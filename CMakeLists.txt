# cmake_minimum_required(VERSION 3.16)
cmake_minimum_required(VERSION 3.16)
set(PICO_SDK_FETCH_FROM_GIT ON)

include(pico_sdk_import.cmake)
include(liblightmodbus/lightmodbusConfig.cmake)
project(firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# to use the sdk
pico_sdk_init()

# if (TARGET tinyusb_device)
add_executable(firmware
    firmware.c
)

# pull in common dependencies
target_link_libraries(firmware pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(firmware 1)
pico_enable_stdio_uart(firmware 1)

# create binaries
pico_add_extra_outputs(firmware)

# add url via pico_set_program_url
# example_auto_set_url(firmware)

# elseif(PICO_ON_DEVICE)
#     message(WARNING "not building firmware because TinyUSB submodule is not initialized in the SDK")
# endif()
