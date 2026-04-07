cmake_minimum_required(VERSION 3.12)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(PDM_BOOT MATCHES ON)
    message("Build bootloadet for pdm")  
    # Set the project name
    set(CMAKE_PROJECT_NAME pdm_boot)    
    add_definitions(      
        -DPDM_BOOT
        -DNO_INTERRUPT
    )
endif()
if(PDM_BOOT MATCHES OFF)
    message("Build pdm")  
    set(CMAKE_PROJECT_NAME pdm)
    add_definitions(      
        -DFREERTOS
    )    
endif()

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
add_subdirectory(stm_lib)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB_RECURSE NMEA_SRC 
        NMEA2000/src/*.cpp
    )

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/app.c
    Core/Src/bsp.c
    nmea2k/nmea2k.c
    Core/Src/led.c
    Core/Src/vn7004.c    
    Core/Src/comm.c
    Core/Src/vn_double.c
    #${NMEA_SRC}

)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${STM_LIB_INC_DIR}
    nmea2k
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    -DFREERTOS
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    STM_LIB
    # Add user defined libraries
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMENT "Generate bin..."
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)
    