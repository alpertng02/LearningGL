add_library(Shader "Shader.cpp" "Shader.h")

find_package(glad CONFIG REQUIRED)

target_link_libraries(Shader 
	PRIVATE	 
		glad::glad
)

target_include_directories(Shader
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}"
)

