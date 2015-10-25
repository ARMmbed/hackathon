# Copyright (C) 2014-2015 ARM Limited. All rights reserved.

if(TARGET_NORDIC_NRF51822_16K_GCC_TOOLCHAIN_INCLUDED)
    return()
endif()
set(TARGET_NORDIC_NRF51822_16K_GCC_TOOLCHAIN_INCLUDED 1)

# legacy definitions for building mbed 2.0 modules with a retrofitted build
# system:
set(MBED_LEGACY_TARGET_DEFINITIONS "NORDIC" "NRF51822_MKIT" "MCU_NRF51822" "MCU_NORDIC_16K")
# provide compatibility definitions for compiling with this target: these are
# definitions that legacy code assumes will be defined. 
add_definitions("-DNRF51 -DTARGET_NORDIC -DTARGET_M0 -D__MBED__=1 -DMCU_NORDIC_16K -DTARGET_NRF51822 -DTARGET_MCU_NORDIC_16K")

# append non-generic flags, and set NRF51822-specific link script
set(_CPU_COMPILATION_OPTIONS "-mcpu=cortex-m0 -mthumb -D__thumb2__")

set(CMAKE_C_FLAGS_INIT             "${CMAKE_C_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_ASM_FLAGS_INIT           "${CMAKE_ASM_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_CXX_FLAGS_INIT           "${CMAKE_CXX_FLAGS_INIT} ${_CPU_COMPILATION_OPTIONS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_MODULE_LINKER_FLAGS_INIT} -mcpu=cortex-m0 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "${CMAKE_EXE_LINKER_FLAGS_INIT} -mcpu=cortex-m0 -mthumb -T${CMAKE_CURRENT_LIST_DIR}/../ld/NRF51822.ld")

# used by the apply_target_rules function below:
set(NRF51822_SOFTDEVICE_HEX_FILE "${CMAKE_CURRENT_LIST_DIR}/../softdevice/s130_nrf51_1.0.0_softdevice.hex")
set(NRF51822_MERGE_HEX_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../scripts/merge_hex.py")

# define a function for yotta to apply target-specific rules to build products,
# in our case we need to convert the built elf file to .hex, and add the
# pre-built softdevice:
function(yotta_apply_target_rules target_type target_name)
    if(${target_type} STREQUAL "EXECUTABLE")
        add_custom_command(TARGET ${target_name}
            POST_BUILD
            # objcopy to hex
            COMMAND arm-none-eabi-objcopy -O ihex ${target_name} ${target_name}.hex
            # and append the softdevice hex file
            COMMAND python ${NRF51822_MERGE_HEX_SCRIPT} ${NRF51822_SOFTDEVICE_HEX_FILE} ${target_name}.hex ${target_name}-combined.hex            
            COMMENT "hexifying and adding softdevice to ${target_name}"
            VERBATIM
        )
    endif()
endfunction()
