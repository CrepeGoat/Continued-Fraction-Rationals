add_library(BitTwiddles INTERFACE)

#set_target_properties(BitTwiddles PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(BitTwiddles INTERFACE cxx_static_assert)

target_include_directories(BitTwiddles INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
