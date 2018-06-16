add_library(TypeUtility INTERFACE)
target_include_directories(TypeUtility INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
target_sources(TypeUtility INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/TypeUtility.hpp
  )
target_compile_features(TypeUtility INTERFACE
    cxx_std_11
    cxx_alias_templates
  )




add_library(BitTwiddles BitTwiddles.cpp)
target_include_directories(BitTwiddles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(BitTwiddles
  PUBLIC
    cxx_std_11
    cxx_static_assert
  )
target_link_libraries(BitTwiddles INTERFACE TypeUtility)




add_library(EndianDetector INTERFACE)
target_include_directories(EndianDetector INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
target_sources(EndianDetector INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/endian.hpp
  )
target_compile_features(EndianDetector INTERFACE
    cxx_strong_enums
  )

