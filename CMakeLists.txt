# CMake entry point.


cmake_minimum_required (VERSION 2.6)
project (MUSIC_VISUALIZER_REBOOTED)

find_library(DFT

	NAMES 
		fftw3
	PATHS 
		/usr/lib
		/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	      	/usr/local/lib
)
find_library(MATH_L

	NAMES 
		m 
	PATHS 
		/usr/lib
	      	/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	      	/usr/local/lib
)
find_library(THREAD_L

	NAMES 
		pthread 
	PATHS 
		/usr/lib
	      	/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	     	/usr/local/lib
)
find_library(SDL
	NAMES 
		SDL2 
	PATHS 
		/usr/lib
	      	/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE} 
	      	/usr/local/lib
)
find_library(GLU_LIB
	NAMES 
		GLU
	PATHS 
		/usr/lib
	      	/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	      	/usr/local/lib
)

message(${CMAKE_LIBRARY_ARCHITECTURE})

if(MATH_L)
	message(${MATH_L})
endif(MATH_L)
if(SDL)
	message(${SDL})
endif(SDL)
if(DFT)
	message(${DFT})
endif(DFT)

if(GLU_LIB)
	message(${GLU_LIB})
endif(GLU_LIB)

#set(MY_HEADER_FILES

#	includes/audioInformation.h
#	includes/dataprocessing.h
#)

set(MY_SOURCE_FILES
	
	src/visualize.c
	src/audioInformation.c
	src/dataprocessing.c
)

include_directories(
  # includes
 )

file(GLOB SOURCES *.c)
file(GLOB INCLUDES *.h)
##list command line warnings

set(WARNINGS 
-Wall -Wextra -Wpedantic -Wformat=2
-Wno-unused-parameter -Wshadow -Wwrite-strings
-Wredundant-decls -Wmissing-include-dirs
)

##		compile for DEBUGGING: 
##		NOTE: This works also: set(CMAKE_CXX_FLAGS "-ggdb")


## List libraries

#execute_process(COMMAND pkg-config --libs --cflags fftw3
#				OUTPUT_VARIABLE DFT_LIB
#				OUTPUT_STRIP_TRAILING_WHITESPACE)
#execute_process(COMMAND pkg-config --libs sdl2 
#				OUTPUT_VARIABLE SDL_LIB 
#				OUTPUT_STRIP_TRAILING_WHITESPACE)
#set(LINKER_FLAGS "-lm")


set(ALL_LIBS
	${MATH_L}
	${DFT}
	${SDL}
	${THREAD_L}
	${WARNINGS}
)




##Output Assembly

#set_property(SOURCE ${MY_SOURCE_FILES} PROPERTY COMPILE_FLAGS -save-temps)

##Link files and libraries to executable

set(binary_name "visual.exe")

add_executable(${binary_name}
	${SOURCES} ${INCLUDES}
	#${MY_HEADER_FILES}
	#${MY_SOURCE_FILES}
)
target_link_libraries(${binary_name}
	${ALL_LIBS} 
)

## Optimize and dubug enabled if GNU compiler
	if(CMAKE_COMPILER_IS_GNUCC)
		message("Optimize and debug enabled!")
    	set(CMAKE_CXX_FLAGS "-O2 -g -std=c++11")       
    	set(CMAKE_C_FLAGS "-O2 -g -std=c99")
	endif(CMAKE_COMPILER_IS_GNUCC)


set(CMAKE_BUILD_TYPE Debug)



# HOW-TO. You can safely remove anything below.  

# test if linux, mac or windows : 

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("You're on Mac OS !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	message("You're on Linux !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message("You're on Windows !")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # Yes this is weird but you have to repeat the "if" here.


# Add a checkbox in CMakeGUI : 

#set(SOME_STUFF OFF CACHE BOOL "Check this to print an extra message at the beginning of main()")
set(TEST_PROGRAM "default" CACHE STRING "empty message for CMakeGUI")
set(VERS "default" CACHE STRING "empty message for CMakeGUI")


# Add a preprocessor define : 


#if(${VERS} MATCHES "v1")
#	add_definitions(
#		-DVERSION_1
#	)
#	message("version1 directive set!!!")
#endif(${VERS} MATCHES "v1")

