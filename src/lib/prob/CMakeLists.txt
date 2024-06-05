# == src/lib/prob/CMakeLists.txt ==

# Multovl probability calculations

# overlap probability convenience library
set(movl_probsrc
    empirdistr.cc freeregions.cc shuffleovl.cc stat.cc
     probpipeline.cc probopts.cc
     parprobpipeline.cc parprobopts.cc
)
if (MULTOVL_USE_STATIC_LIBS)
	add_library(movl_prob STATIC ${movl_probsrc})
else()
	add_library(movl_prob SHARED ${movl_probsrc})
endif()
target_link_libraries(movl_prob PUBLIC movl)
flag_fix(movl_prob)
