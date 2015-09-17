set(SOURCES
	PPCArchitecture.cpp
	PPCArchitecture.h
	PPCDisassembler.cpp
	PPCDisassembler.h
	PPCInstruction.h
	PPCInstructionAnalyzer.cpp
	PPCInstructionAnalyzer.h
	PPCRegisters.cpp
	PPCRegisters.h
	PPCCallingConventions.cpp
	PPCCallingConventions.h
)

add_library(nc-arch-ppc ${SOURCES})
target_link_libraries(nc-arch-ppc nc-core nc-common capstone-static ${Boost_LIBRARIES} ${QT_LIBRARIES})
	