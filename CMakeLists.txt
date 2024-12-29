cmake_minimum_required(VERSION 3.13...3.27)

# Use modern C
set(CMAKE_C_STANDARD 23)

# Export `compile_commands.json` for `clangd`
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We're using the official Pico 2 board
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Initialize pico-sdk from submodule
# NOTE: This must happen before `project()`
include(pico-sdk/pico_sdk_init.cmake)

# FIXME: Can this go after `pico_sdk_init()`?
project(led-soundsystem)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(led-soundsystem
  src/main.c
)

# Pull in common dependencies
target_link_libraries(led-soundsystem pico_stdlib)

# Enable usb output, disable uart output
pico_enable_stdio_usb(led-soundsystem 1)
pico_enable_stdio_uart(led-soundsystem 0)

# Enter BOOTSEL mode after the program finishes
target_compile_definitions(led-soundsystem PRIVATE PICO_ENTER_USB_BOOT_ON_EXIT=1)

# With helpful error messages
target_compile_options(led-soundsystem PRIVATE -Wall -Wextra -Wpedantic)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(led-soundsystem)
