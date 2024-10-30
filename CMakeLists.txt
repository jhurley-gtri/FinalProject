# CMake entry point
cmake_minimum_required (VERSION 3.23)
project (FinalProject)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

link_directories(${CMAKE_SOURCE_DIR}/Final_Project/)

# FinalProject
add_executable(FinalProject
	Final_Project/FinalProject.cpp
	Final_Project/assimp_model.cpp
	Final_Project/assimp_model.h
	Final_Project/vertexBufferObject.cpp	
	Final_Project/vertexBufferObject.h
	Final_Project/texture.cpp	
	Final_Project/texture.h
	common/shader.cpp	
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp	
	Final_Project/StandardShading.vertexshader
	Final_Project/StandardShading.fragmentshader
)
target_link_libraries(FinalProject
	${ALL_LIBS}
	assimp
	FreeImage
)
set_target_properties(FinalProject PROPERTIES COMPILE_DEFINITIONS "USE_ASSIMP")
# Xcode and Visual working directories
set_target_properties(FinalProject PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Final_Project/")
# Set the working directory for Visual Studio
set_property(TARGET FinalProject PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Final_Project")

add_custom_command(
   TARGET FinalProject POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/FinalProject${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/Final_Project/"
)


