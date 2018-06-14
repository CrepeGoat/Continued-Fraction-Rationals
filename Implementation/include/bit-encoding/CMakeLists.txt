add_library(BitSequence INTERFACE)

target_compile_features(BitSequence INTERFACE
    cxx_default_function_template_args
    cxx_nullptr
  )

target_include_directories(BitSequence INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitSequence INTERFACE BitTwiddles)
