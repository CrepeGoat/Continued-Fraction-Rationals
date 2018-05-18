add_library(BitTwiddles BitTwiddles.h)

set_target_properties(BitTwiddles PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(BitTwiddles PUBLIC cxx_static_assert)

target_include_directories(BitTwiddles INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(BitTwiddles_test BitTwiddles_test.cpp)
target_link_libraries(BitTwiddles_test LINK_PRIVATE BitTwiddles)
