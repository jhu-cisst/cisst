cmake_minimum_required (VERSION 3.16)
project (cisstDownstreamDataGenerator LANGUAGES CXX)

find_package (cisst 1.5 REQUIRED COMPONENTS cisstCommon)
if (NOT TARGET cisst::cisstDataGenerator)
  message (FATAL_ERROR "The cisst package did not export cisstDataGenerator")
endif ()

# Exercise spaces in output paths as well as configuration-aware lookup.
cisst_data_generator (sample "${CMAKE_CURRENT_BINARY_DIR}/generated include" "sample/" sample.cdg)
add_custom_target (generate_sample ALL DEPENDS ${sample_CISST_DG_SRCS})
