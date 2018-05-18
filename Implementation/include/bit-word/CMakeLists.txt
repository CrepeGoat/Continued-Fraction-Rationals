add_library(BitSeqWord INTERFACE)

target_compile_features(BitSeqWord
  INTERFACE
    cxx_alias_templates
    cxx_auto_type
    cxx_constexpr
    cxx_lambdas
    cxx_generic_lambdas
    cxx_static_assert
    cxx_rvalue_references
  )

target_include_directories(BitSeqWord INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(BitSeqWord INTERFACE BitTwiddles)
