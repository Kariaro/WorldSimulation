
#ifndef RENDERING_CAMERA
#define RENDERING_CAMERA

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <cmath>

namespace render::camera
{

struct Camera
{
	// Position is stored as double to prevent position glitches in the shader
	glm::dvec3 position{};
	glm::dquat rotation{};

	// Fov and 
	double cameraFov{90};
	double cameraFar{1'000};
	double cameraNear{0.00001};

	[[nodiscard]] glm::mat4 getPerspectiveMatrix(double a_width, double a_height)
	{
		return 
			glm::perspective(glm::radians(cameraFov), a_width / a_height, cameraNear, cameraFar) *

			// Converts from ECEF coordinates to OpenGL
			glm::dmat4(glm::mat3(
				0, 0,-1,
			   -1, 0, 0,
				0, 1, 0
			));
	}

	[[nodiscard]] glm::mat4 getCameraMatrix()
	{
		return glm::mat4_cast(rotation);
	}

	[[nodiscard]] glm::mat4 getViewMatrix(double a_width, double a_height)
	{
		glm::mat4 perspectiveMatrix = getPerspectiveMatrix(a_width, a_height);
		glm::mat4 cameraMatrix = getCameraMatrix();

		return perspectiveMatrix * cameraMatrix;
	}
};


} // render::camera

#endif  // RENDERING_CAMERA
