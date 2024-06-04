cmake_minimum_required (VERSION 3.14)
cmake_policy(SET CMP0077 NEW)
project(CMSISCV)

option(NEON "Neon acceleration" OFF)
option(NEONEXPERIMENTAL "Neon experimental acceleration" OFF)
option(DISABLEFLOAT16 "Disable building float16 kernels" OFF)
option(HOST "Build for host" OFF)
option(LAXVECTORCONVERSIONS "Lax vector conversions" ON)

SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(CMSISCV STATIC)

target_include_directories(CMSISCV PUBLIC "${ROOT}/Include")

target_sources(CMSISCV PRIVATE arm_cv_common.c
    FeatureDetection/cannysobel.c 
    LinearFilters/gaussian.c
    ColorTransforms/arm_yuv420_to_gray8.c
    ColorTransforms/arm_bgr_8U3C_to_gray8.c
    ColorTransforms/arm_gray8_to_rgb24.c
    ColorTransforms/arm_bgr_8U3C_to_rgb24.c
    ColorTransforms/arm_yuv420_to_rgb24.c
    ColorTransforms/arm_rgb24_to_gray8.c
    ImageTransforms/arm_crop_gray8.c
    ImageTransforms/arm_crop_rgb24.c
    ImageTransforms/arm_image_resize_bgr_8U3C.c
    ImageTransforms/arm_image_resize_gray8.c
    )

if (HOST)
      target_compile_definitions(CMSISCV PUBLIC __GNUC_PYTHON__)
endif()

if (NEON)
    target_compile_definitions(${project} PRIVATE ARM_MATH_NEON)
endif()

if (NEONEXPERIMENTAL)
    target_compile_definitions(${project} PRIVATE ARM_MATH_NEON_EXPERIMENTAL)
endif()



if (MVEI OR MVEF OR HELIUM)
    # By default, GCC does not enable implicit conversion between vectors of different numbers or types of elements
    # which is required by some code in CMSIS-DSP
    if (LAXVECTORCONVERSIONS)
       target_compile_options(CMSISCV PRIVATE $<$<STREQUAL:${CMAKE_C_COMPILER_ID},GNU>:-flax-vector-conversions>)
       target_compile_options(CMSISCV PRIVATE $<$<STREQUAL:${CMAKE_C_COMPILER_ID},ARMClang>:-flax-vector-conversions=integer>)
    else()
       target_compile_options(CMSISCV PRIVATE $<$<STREQUAL:${CMAKE_C_COMPILER_ID},GNU>:-fno-lax-vector-conversions>)
       target_compile_options(CMSISCV PRIVATE $<$<STREQUAL:${CMAKE_C_COMPILER_ID},ARMClang>:-flax-vector-conversions=none>)
    endif()
endif()

if (DISABLEFLOAT16)
    target_compile_definitions(CMSISCV PRIVATE DISABLEFLOAT16) 
endif()

target_link_libraries(CMSISCV PUBLIC CMSISDSP)
