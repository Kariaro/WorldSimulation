
#ifndef RENDERING_SHADER
#define RENDERING_SHADER

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

namespace render::shader
{

static const char* getShaderTypeName(unsigned int a_shaderType)
{
	switch(a_shaderType)
	{
		case GL_GEOMETRY_SHADER: return "geometry";
		case GL_FRAGMENT_SHADER: return "fragment";
		case GL_VERTEX_SHADER: return "vertex";
	}
	return "unknown";
}

static int compileShaderCode(
	const char* a_shaderCode,
	const int a_shaderType)
{
	// Compile vertex shader
	int shaderId = glCreateShader(a_shaderType);
	if(shaderId == 0)
	{
		if(a_shaderType)
		std::printf("Failed to create %s shader. glCreateShader() returned 0\n", getShaderTypeName(a_shaderType));
		return 0;
	}

	glShaderSource(shaderId, 1, &a_shaderCode, nullptr);
	glCompileShader(shaderId);

	int compileStatus = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus == 0)
	{
		int logMessageLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logMessageLength);

		char* logMessage = new char[logMessageLength + 1];
		glGetShaderInfoLog(shaderId, logMessageLength, nullptr, logMessage);

		std::printf("Error compiling %s shader:\n", getShaderTypeName(a_shaderType));
		std::printf("%s\n", logMessage);
		delete[] logMessage;
		return 0;
	}

	return shaderId;
}

static int compileShader(
	const char* a_vertexShader,
	const char* a_fragmentShader)
{
	int vertexShaderId = compileShaderCode(a_vertexShader, GL_VERTEX_SHADER);
	if(vertexShaderId == 0)
	{
		return 0;
	}

	int fragmentShaderId = compileShaderCode(a_fragmentShader, GL_FRAGMENT_SHADER);
	if(fragmentShaderId == 0)
	{
		glDeleteShader(vertexShaderId);
		return 0;
	}

	int programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	// Link the program
	int linkStatus = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == 0)
	{
		int logMessageLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logMessageLength);

		char* logMessage = new char[logMessageLength + 1];
		glGetProgramInfoLog(programId, logMessageLength, nullptr, logMessage);

		std::printf("Error linking shader program:\n");
		std::printf("%s\n", logMessage);
		delete[] logMessage;
		return 0;
	}

	// Validate the program
	glValidateProgram(programId);

	int validateStatus = 0;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateStatus);
	if(validateStatus == 0)
	{
		int logMessageLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logMessageLength);

		char* logMessage = new char[logMessageLength + 1];
		glGetProgramInfoLog(programId, logMessageLength, nullptr, logMessage);

		std::printf("Warning validating shader code:\n");
		std::printf("%s\n", logMessage);
		delete[] logMessage;
		return 0;
	}

	return programId;
}

} // render::shader

#endif  // RENDERING_SHADER
