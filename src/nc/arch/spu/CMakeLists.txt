set(SOURCES
	SPU.cpp
	SPU.h
	SPUArchitecture.cpp
	SPUArchitecture.h
	SPUDisassembler.cpp
	SPUDisassembler.h
	SPUInstruction.h
	SPUInstructionAnalyzer.cpp
	SPUInstructionAnalyzer.h
	SPURegisters.cpp
	SPURegisters.h
	SPUCallingConventions.cpp
	SPUCallingConventions.h
)

add_library(nc-arch-spu ${SOURCES})
target_link_libraries(nc-arch-spu nc-core nc-common capstone-static ${Boost_LIBRARIES} ${QT_LIBRARIES})
	