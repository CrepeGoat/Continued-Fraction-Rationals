add_library(WordPointer INTERFACE)

target_compile_features(WordPointer
  INTERFACE
  	cxx_std_17
    cxx_constexpr
  )

target_include_directories(WordPointer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
