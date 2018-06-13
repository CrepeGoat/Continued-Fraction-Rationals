add_library(WholeNumSequence
	WholeNumSeq_Base.cpp
	WholeNumSeq_SBSBase.cpp
	WholeNumSeq_SBS1.cpp
	WholeNumSeq_SBS2.cpp
	WholeNumSequence.cpp
	)

target_include_directories(WholeNumSequence PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WholeNumSequence PUBLIC BitSequence)
