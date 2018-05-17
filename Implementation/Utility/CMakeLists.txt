add_library(BitTwiddles BitTwiddles.h)
set_target_properties(BitTwiddles PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(BitTwiddles PUBLIC cxx_static_assert)
target_include_directories(BitTwiddles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS BitTwiddles DESTINATION bin)
install(FILES BitTwiddles.h DESTINATION include)


add_executable(BitTwiddles_test BitTwiddles_test.cpp)
target_link_libraries(BitTwiddles_test LINK_PUBLIC BitTwiddles)
