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


#include_directories(
#  include
# )

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

)
target_link_libraries(${binary_name}
	${ALL_LIBS} 
)

## Optimize and dubug enabled if GNU compiler
	if(CMAKE_COMPILER_IS_GNUCC)
		message("Optimize and debug enabled!")
    	set(CMAKE_CXX_FLAGS "-O0 -fbuiltin -g -std=c++11")       
    	set(CMAKE_C_FLAGS "-fbuiltin -g -std=c99")
	endif(CMAKE_COMPILER_IS_GNUCC)



# HOW-TO. You can safely remove anything below.  

# test if linux, mac or windows : 

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("You're on Mac OS !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	message("You're on Linux !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message("You're on Windows !")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # Yes this is weird but you have to repeat the "if" here.

