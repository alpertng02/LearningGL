# CMakeList.txt : CMake project for LearningGL, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (MovingTriangle "movingTriangle.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MovingTriangle PROPERTY CXX_STANDARD 20)
endif()

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(MovingTriangle 
	PRIVATE
		glm::glm
		glfw
		OpenGL::GL
		glad::glad
		Shader
)

# TODO: Add tests and install targets if needed.
