cmake_minimum_required(VERSION 3.22)

message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

#
# Core project settings
#
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
project(lwow_stm32f429zi_nucleo_ds18b20_multi_rtos)
enable_language(C CXX ASM)
message("Build type: "              ${CMAKE_BUILD_TYPE})

# Setup compiler settings
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)

#
# Core MCU flags, CPU, instruction set and FPU setup
#
set(CPU_PARAMETERS
    -mthumb
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Set linker script
set(linker_script_SRC               ${PROJ_PATH}/STM32F429ZITX_FLASH.ld)
set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})

#
# Source files
#
set(linked_SRCS
    ${PROJ_PATH}/../../../lwow/src/lwow/lwow.c
	${PROJ_PATH}/../../../lwow/src/devices/lwow_device_ds18x20.c
	${PROJ_PATH}/../../../lwow/src/system/lwow_ll_stm32_hal.c
	${PROJ_PATH}/../../../lwow/src/system/lwow_sys_cmsis_os.c
	${PROJ_PATH}/../../../snippets/scan_devices.c
)
set(source_folder_Core_SRCS 
	${PROJ_PATH}/Core/Src/freertos.c
	${PROJ_PATH}/Core/Src/main.c
	${PROJ_PATH}/Core/Src/stm32f4xx_hal_msp.c
	${PROJ_PATH}/Core/Src/stm32f4xx_hal_timebase_tim.c
	${PROJ_PATH}/Core/Src/stm32f4xx_it.c
	${PROJ_PATH}/Core/Src/syscalls.c
	${PROJ_PATH}/Core/Src/sysmem.c
	${PROJ_PATH}/Core/Src/system_stm32f4xx.c
	${PROJ_PATH}/Core/Startup/startup_stm32f429zitx.s)

set(source_folder_Drivers_SRCS 
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c)

set(source_folder_Middlewares_SRCS 
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/list.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c)

set(source_folder_lwow_SRCS 
	)

# Source files in the same path as ".cproject/.project" but nowhere included in file descriptions
set(all_project_dir_SRCS
    
)

#
# Include directories
#
set(include_c_DIRS
    ${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/include
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
	${PROJ_PATH}/Drivers/CMSIS/Include
	${PROJ_PATH}/../../../lwow/src/include
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc
	${PROJ_PATH}/Core/Inc
	${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
	${PROJ_PATH}/../../../snippets/include
	${PROJ_PATH}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
	${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
)
set(include_cxx_DIRS
    
)
set(include_asm_DIRS
    
)

#
# Symbols definition
#
set(symbols_c_SYMB
    "USE_HAL_DRIVER"
	"DEBUG"
	"STM32F429xx"
)
set(symbols_cxx_SYMB
    
)
set(symbols_asm_SYMB
    
)

#
# Link directories setup
# Must be before executable is added
#
set(link_DIRS
    
)
link_directories(${EXECUTABLE} ${link_DIRS})

#
# Executable files
#
add_executable(${EXECUTABLE}
    ${linked_SRCS}
    ${all_project_dir_SRCS} 
	${source_folder_Core_SRCS}
	${source_folder_Drivers_SRCS}
	${source_folder_Middlewares_SRCS}
	${source_folder_lwow_SRCS})

#
# Add linked libraries for linker
#
set(link_LIBS
    
)
target_link_libraries(${EXECUTABLE} ${link_LIBS})

#
# Project symbols
#
target_compile_definitions(${EXECUTABLE} PRIVATE
    # Language specific only
    $<$<COMPILE_LANGUAGE:C>: ${symbols_c_SYMB}>
    $<$<COMPILE_LANGUAGE:CXX>: ${symbols_cxx_SYMB}>
    $<$<COMPILE_LANGUAGE:ASM>: ${symbols_asm_SYMB}>

    # Configuration specific
    $<$<CONFIG:Debug>:
        DEBUG
    >
    $<$<CONFIG:Release>: >
)

#
# Add include paths for each of the compiler
#
target_include_directories(${EXECUTABLE} PRIVATE
    # Language specific only
    $<$<COMPILE_LANGUAGE:C>: ${include_c_DIRS}>
    $<$<COMPILE_LANGUAGE:CXX>: ${include_cxx_DIRS}>
    $<$<COMPILE_LANGUAGE:ASM>: ${include_asm_DIRS}>

    # Configuration specific
    $<$<CONFIG:Debug>: >
    $<$<CONFIG:Release>: >
)

# Compiler and linker options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:C>:

    >
    $<$<COMPILE_LANGUAGE:CXX>:
        #-Wno-volatile
        #-Wold-style-cast
        #-Wuseless-cast
        #-Wsuggest-override
    >
    $<$<COMPILE_LANGUAGE:ASM>:

    >
    $<$<CONFIG:Debug>:
        -Og -g3 -ggdb
    >
    $<$<CONFIG:Release>:
        -Og -g0
    >
)

# Setup linker parameters
target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}${linker_script_SRC}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage
)

# Execute post-build to print size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
)

# Convert output to hex and binary
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
)

# Conver to bin file -> add conditional check?
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
)

message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")