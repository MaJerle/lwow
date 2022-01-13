cmake_minimum_required(VERSION 3.22)

message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

#
# Core project settings
#
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
project(lwow_stm32f401re_nucleo_ds18b20_rtos)
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
set(linker_script_SRC               ${PROJ_PATH}/STM32CubeIDE/STM32F401RETX_FLASH.ld)
set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})

#
# Source files
#
set(linked_SRCS
    ${PROJ_PATH}/../../lib/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.c
	${PROJ_PATH}/../../lib/FreeRTOS/croutine.c
	${PROJ_PATH}/../../lib/FreeRTOS/event_groups.c
	${PROJ_PATH}/../../lib/FreeRTOS/portable/MemMang/heap_4.c
	${PROJ_PATH}/../../lib/FreeRTOS/list.c
	${PROJ_PATH}/../../lib/FreeRTOS/portable/GCC/ARM_CM4F/port.c
	${PROJ_PATH}/../../lib/FreeRTOS/queue.c
	${PROJ_PATH}/../../lib/FreeRTOS/tasks.c
	${PROJ_PATH}/../../lib/FreeRTOS/timers.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_pwr.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c
	${PROJ_PATH}/src/main.c
	${PROJ_PATH}/src/stm32f4xx_it.c
	${PROJ_PATH}/src/syscalls.c
	${PROJ_PATH}/src/system_stm32f4xx.c
	${PROJ_PATH}/../../../lwow/src/lwow/lwow.c
	${PROJ_PATH}/../../../lwow/src/devices/lwow_device_ds18x20.c
	${PROJ_PATH}/../../../lwow/src/system/lwow_ll_stm32f401re_nucleo.c
	${PROJ_PATH}/../../../lwow/src/system/lwow_sys_cmsis_os.c
	${PROJ_PATH}/../../../snippets/scan_devices.c
)
set(source_folder_FreeRTOS_SRCS 
	)

set(source_folder_STM32F4xx_HAL_Driver_SRCS 
	)

set(source_folder_User_SRCS 
	)

set(source_folder_lwow_SRCS 
	)

set(source_folder_startup_SRCS 
	${PROJ_PATH}/STM32CubeIDE/startup/startup_stm32f401xe.s)

# Source files in the same path as ".cproject/.project" but nowhere included in file descriptions
set(all_project_dir_SRCS
    
)

#
# Include directories
#
set(include_c_DIRS
    ${PROJ_PATH}/inc
	${PROJ_PATH}/../../../snippets/include
	${PROJ_PATH}/../../../lwow/src/include
	${PROJ_PATH}/../../lib/st/CMSIS/Include
	${PROJ_PATH}/../../lib/st/CMSIS/Device/ST/STM32F4xx/Include
	${PROJ_PATH}/../../lib/st/STM32F4xx_HAL_Driver/Inc
	${PROJ_PATH}/../../lib/FreeRTOS/include
	${PROJ_PATH}/../../lib/FreeRTOS/CMSIS_RTOS_V2
	${PROJ_PATH}/../../lib/FreeRTOS/portable/GCC/ARM_CM4F
)
set(include_cxx_DIRS
    
)
set(include_asm_DIRS
    
)

#
# Symbols definition
#
set(symbols_c_SYMB
    "__weak=__attribute__((weak))"
	"HSE_VALUE=8000000"
	"__packed=__attribute__((__packed__))"
	"STM32F401xE"
	"USE_FULL_LL_DRIVER"
)
set(symbols_cxx_SYMB
    "STM32F10X_MD"
	"USE_STDPERIPH_DRIVER"
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
	${source_folder_FreeRTOS_SRCS}
	${source_folder_STM32F4xx_HAL_Driver_SRCS}
	${source_folder_User_SRCS}
	${source_folder_lwow_SRCS}
	${source_folder_startup_SRCS})

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