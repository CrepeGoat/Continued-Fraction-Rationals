add_library(BitSequence INTERFACE)

target_compile_features(BitSequence INTERFACE
    cxx_defaulted_functions
    cxx_default_function_template_args
    cxx_nullptr
    cxx_long_long_type
  )

target_include_directories(BitSequence INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitSequence INTERFACE BitTwiddles EndianDetector)
