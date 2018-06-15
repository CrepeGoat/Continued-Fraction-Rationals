add_executable(BitTwiddles_test BitTwiddles_test.cpp)
target_link_libraries(BitTwiddles_test
  LINK_PRIVATE
    BitTwiddles
  )



add_executable(WordPointer_test WordPointer_test.cpp)
target_link_libraries(WordPointer_test
  LINK_PRIVATE
    WordPointer
    BitTwiddles
    EndianDetector
  )



add_executable(BitSeqWord_test BitSeqWord_test.cpp)
target_compile_features(BitSeqWord_test PUBLIC
	cxx_auto_type
	cxx_variadic_templates
	)
target_link_libraries(BitSeqWord_test
  LINK_PRIVATE
    BitSeqWord
  )



add_executable(BitSequence_test BitSequence_test.cpp)
target_link_libraries(BitSequence_test
  LINK_PRIVATE
    BitSequence
    BitTwiddles
  )



add_executable(WholeNumSequence_test WholeNumSequence_test.cpp)
target_link_libraries(WholeNumSequence_test
  LINK_PRIVATE
    WholeNumSequence
    BitSequence
  )
