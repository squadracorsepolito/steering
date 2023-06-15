cmake_minimum_required(VERSION 3.16)

set(STM32_TOOLCHAIN_PATH /opt/homebrew/bin)
set(STM32_TARGET_TRIPLET arm-none-eabi)
set(STM32_CUBE_H7_PATH $ENV{HOME}/STM32Cube/Repository/STM32Cube_FW_H7_V1.10.0)

add_definitions(-DSTM32H745xx)

message(STATUS "STM32_TOOLCHAIN_PATH: ${STM32_TOOLCHAIN_PATH}")
message(STATUS "STM32_TARGET_TRIPLET: ${STM32_TARGET_TRIPLET}")
message(STATUS "STM32_CUBE_H7_PATH: ${STM32_CUBE_H7_PATH}")

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/stm32-cmake/cmake/stm32_gcc.cmake)

project(stm32-template C ASM)

# Find CMSIS for both cores
find_package(CMSIS COMPONENTS STM32H745XI_M4 STM32H745XI_M7 REQUIRED)
find_package(HAL COMPONENTS STM32H745XI_M4 STM32H745XI_M7 REQUIRED)

file(GLOB_RECURSE M7_SOURCE CM7/Core/Src/*.c CM7/Drivers/*.c)
file(GLOB_RECURSE M4_SOURCE CM4/Core/Src/*.c)

add_executable(m7core ${M7_SOURCE} Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c CM7/Core/Startup/startup_stm32h745xihx.s)
add_executable(m4core ${M4_SOURCE} Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c CM4/Core/Startup/startup_stm32h745xihx.s)

target_include_directories(m7core PRIVATE CM7/Core/Inc CM7/Drivers/Steering CM7/Drivers/Components)
target_include_directories(m4core PRIVATE CM4/Core/Inc)

target_link_libraries(m7core PRIVATE
  HAL::STM32::H7::M7::RCC
  HAL::STM32::H7::M7::GPIO
  HAL::STM32::H7::M7::CORTEX
  HAL::STM32::H7::M7::HSEM
  HAL::STM32::H7::M7::QSPI
  HAL::STM32::H7::M7::SDRAM
  HAL::STM32::H7::M7::ADC
  HAL::STM32::H7::M7::FDCAN
  HAL::STM32::H7::M7::MDMA
  HAL::STM32::H7::M7::LTDC
  HAL::STM32::H7::M7::DMA2D
  HAL::STM32::H7::M7::ADCEx
  HAL::STM32::H7::M7::RCCEx
  HAL::STM32::H7::M7::TIMEx
  HAL::STM32::H7::M7::PWREx
  HAL::STM32::H7::M7::I2CEx
  HAL::STM32::H7::M7::UARTEx
  HAL::STM32::H7::M7::LL_FMC
  CMSIS::STM32::H7::M7
  STM32::NoSys
)
stm32_print_size_of_target(m7core)
stm32_add_linker_script(m7core PRIVATE CM7/STM32H745XIHX_FLASH.ld)

target_link_libraries(m4core PRIVATE
  HAL::STM32::H7::M4::RCC
  HAL::STM32::H7::M4::GPIO
  HAL::STM32::H7::M4::CORTEX
  HAL::STM32::H7::M4::HSEM
  HAL::STM32::H7::M4::PWREx
  CMSIS::STM32::H7::M4
  STM32::NoSys
)
stm32_print_size_of_target(m4core)
stm32_add_linker_script(m4core PRIVATE CM4/STM32H745XIHX_FLASH.ld)

function(make_output EXECUTABLE)
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin)

    if(${DUMP_ASM})
        add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
            COMMAND ${CMAKE_OBJDUMP} -D -C $<TARGET_FILE:${EXECUTABLE}> > ${EXECUTABLE}.s)
    endif()
endfunction()

make_output(m4core)
make_output(m7core)
