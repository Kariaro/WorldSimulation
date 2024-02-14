
#ifndef RENDERING_GEOMETRY
#define RENDERING_GEOMETRY

#include <glad/glad.h>

#include <iostream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.h"

#include "wgs84.hpp"

using namespace geodecy;

namespace render::geometry
{

// A mesh with geometry
struct geometry_mesh
{
	unsigned int vaoId;
	unsigned int vboId_position;
	unsigned int vboId_color;
	unsigned int vboId_uv;
	unsigned int vertexCount;

	void draw() const
	{
		glBindVertexArray(vaoId);
		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // color
		glEnableVertexAttribArray(2); // uv

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}
};

struct test_geometry
{
	// Shader program id
	unsigned int programId;
	unsigned int uniform_viewMatrix;

	// Model
	geometry_mesh xyz_arrow;
};

struct draw_data
{
	std::vector<float> positions;
	std::vector<float> colors;
	std::vector<float> uvs;

	void pos(float x, float y, float z, float r = 0, float g = 0, float b = 0)
	{
		positions.push_back(x);
		positions.push_back(y);
		positions.push_back(z);
		colors.push_back(r);
		colors.push_back(g);
		colors.push_back(b);
	}
};

void drawBox(
	draw_data& a_data,

	// Position
	float x, float y, float z,

	// Sizes
	float xsize, float ysize, float zsize,

	// Color
	float r, float g, float b
)
{
	const float xs = x;
	const float ys = y;
	const float zs = z;
	const float xe = x + xsize;
	const float ye = y + ysize;
	const float ze = z + zsize;

	const float c = 0.5;
	
	// Front
	a_data.pos(xs, ys, zs, r - c, g - c, b - c);
	a_data.pos(xs, ye, zs, r - c, g	, b - c);
	a_data.pos(xs, ye, ze, r - c, g	, b	);
	a_data.pos(xs, ys, zs, r - c, g - c, b - c);
	a_data.pos(xs, ye, ze, r - c, g	, b	);
	a_data.pos(xs, ys, ze, r - c, g - c, b	);

	// Back
	a_data.pos(xe, ys, zs, r	, g - c, b - c);
	a_data.pos(xe, ye, zs, r	, g	, b - c);
	a_data.pos(xe, ye, ze, r	, g	, b	);
	a_data.pos(xe, ys, zs, r	, g - c, b - c);
	a_data.pos(xe, ye, ze, r	, g	, b	);
	a_data.pos(xe, ys, ze, r	, g - c, b	);

	// Right
	a_data.pos(xs, ys, zs, r - c, g - c, b - c);
	a_data.pos(xs, ys, ze, r - c, g - c, b	);
	a_data.pos(xe, ys, ze, r	, g - c, b	);
	a_data.pos(xs, ys, zs, r - c, g - c, b - c);
	a_data.pos(xe, ys, ze, r	, g - c, b	);
	a_data.pos(xe, ys, zs, r	, g - c, b - c);

	// Left
	a_data.pos(xs, ye, zs, r - c, g	, b - c);
	a_data.pos(xs, ye, ze, r - c, g	, b	);
	a_data.pos(xe, ye, ze, r	, g	, b	);
	a_data.pos(xs, ye, zs, r - c, g	, b - c);
	a_data.pos(xe, ye, ze, r	, g	, b	);
	a_data.pos(xe, ye, zs, r	, g	, b - c);

	// Top
	a_data.pos(xs, ye, ze, r - c, g	, b	);
	a_data.pos(xe, ye, ze, r	, g	, b	);
	a_data.pos(xe, ys, ze, r	, g - c, b	);
	a_data.pos(xs, ye, ze, r - c, g	, b	);
	a_data.pos(xe, ys, ze, r	, g - c, b	);
	a_data.pos(xs, ys, ze, r - c, g - c, b	);

	// Bottom
	a_data.pos(xs, ye, zs, r - c, g	, b - c);
	a_data.pos(xe, ye, zs, r	, g	, b - c);
	a_data.pos(xe, ys, zs, r	, g - c, b - c);
	a_data.pos(xs, ye, zs, r - c, g	, b - c);
	a_data.pos(xe, ys, zs, r	, g - c, b - c);
	a_data.pos(xs, ys, zs, r - c, g - c, b - c);
}

geometry_mesh create_mesh(const draw_data& a_data)
{
	geometry_mesh mesh{};
	std::vector<float> positions = a_data.positions;
	std::vector<float> colors = a_data.colors;
	std::vector<float> uvs = a_data.uvs;

	mesh.vertexCount = positions.size() / 3;
	colors.resize(mesh.vertexCount * 3);
	uvs.resize(mesh.vertexCount * 2);

	glGenVertexArrays(1, &mesh.vaoId);
	glBindVertexArray(mesh.vaoId);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Positions
	glGenBuffers(1, &mesh.vboId_position);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	
	// Colors
	glGenBuffers(1, &mesh.vboId_color);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	// Texture
	glGenBuffers(1, &mesh.vboId_uv);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return mesh;
}

// Create an xyz arrow mesh
void create_xyz_arrow(geometry_mesh& a_mesh)
{
	draw_data data;
	drawBox(data, 0, 0, 0,  1.0f, 0.1f, 0.1f,  1, 0, 0);
	drawBox(data, 0, 0, 0,  0.1f, 1.0f, 0.1f,  0, 1, 0);
	drawBox(data, 0, 0, 0,  0.1f, 0.1f, 1.0f,  0, 0, 1);
}


test_geometry test()
{
	int programId = shader::compileShader(
// Vertex
R"glsl(
#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Color;
layout (location = 2) in vec2 in_Uv;

uniform mat4 viewMatrix;

out vec3 pass_Color;

void main() {
	gl_Position = viewMatrix * vec4(in_Position, 1.0);
	pass_Color = in_Color;
}
)glsl",

// Fragment
R"glsl(
#version 330 core

in vec3 pass_Color;

out vec4 out_Color;

void main() {
	out_Color = vec4(pass_Color, 1.0);
}
)glsl"
	);

	test_geometry geometry{};
	geometry.programId = programId;
	geometry.uniform_viewMatrix = glGetUniformLocation(geometry.programId, "viewMatrix");
	
	{
		float t  = 0.01f;
		float ht = -t * 0.5f;
		draw_data data;
		drawBox(data,  0, ht, ht,  1.0f,	t,	t,  1, 0, 0);
		drawBox(data, ht,  0, ht,	 t, 1.0f,	t,  0, 1, 0);
		drawBox(data, ht, ht,  0,	 t,	t, 1.0f,  0, 0, 1);
		geometry.xyz_arrow = create_mesh(data);
	}

	// data.pos(-0.5, -0.5, -0.5,   1, 0, 0);
	// data.pos( 0.5, -0.5, -0.5,   0, 1, 0);
	// data.pos( 0.5,  0.5, -0.5,   0, 0, 1);

	// data.pos(1, -1, -1,   1, 1, 0);
	// data.pos(1,  1, -1,   1, 0, 1);
	// data.pos(1,  1,  1,   0, 1, 1);

	return geometry;
}

void test_draw(
	const test_geometry& a_geometry,
	const glm::mat4& a_perspectiveMatrix,
	const glm::mat4& a_cameraMatrix,
	int width,
	int height)
{
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// glm::mat4 matrix = a_perspectiveMatrix;
	// matrix =
	// glm::perspective(glm::radians(90.0f), width / static_cast<float>(height), 0.0001f, 1000.0f) *
	// glm::mat4(glm::mat3(
	//	 0, 0, -1,
	//	-1, 0, 0,
	//	 0, 1, 0
	// )) * a_cameraMatrix;
	glm::mat4 matrix = a_perspectiveMatrix * a_cameraMatrix;

	glUseProgram(a_geometry.programId);

	// Draw xyz world arrow
	glUniformMatrix4fv(a_geometry.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
	a_geometry.xyz_arrow.draw();

	// Draw xyz test arrow
	glm::mat4 test = matrix;
	test = glm::translate(test, glm::vec3(0, -2, 0));
	test = test * glm::mat4(geodecy::rpy(0, -45, glfwGetTime() * 60));

	glUniformMatrix4fv(a_geometry.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(test));
	a_geometry.xyz_arrow.draw();
	
	// Draw xyz test arrow
	test = matrix;
	test = glm::translate(test, glm::vec3(0, -4, 0));
	test = test * glm::mat4(glm::mat3_cast(
		glm::angleAxis(glm::radians(-45.0f), glm::vec3(0, 1, 0)) *
		glm::angleAxis(float(glm::radians(glfwGetTime() * 60.0f)), glm::vec3(0, 0, 1))
	));

	glUniformMatrix4fv(a_geometry.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(test));
	a_geometry.xyz_arrow.draw();

	// Unbind shader
	glUseProgram(0);
}

} // render::geometry

#endif  // RENDERING_GEOMETRY
