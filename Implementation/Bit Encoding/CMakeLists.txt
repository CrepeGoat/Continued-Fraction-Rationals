add_library(BitSequence BitSequence.h BitSequence.tpp)

set_target_properties(BitSequence PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(BitSequence INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitSequence LINK_PUBLIC BitTwiddles)


add_executable(BitSequence_test BitSequence_test.cpp)
target_link_libraries(BitSequence_test LINK_PRIVATE BitSequence)
