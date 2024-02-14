
#ifndef RENDERING_QUADSPHERE_RENDER
#define RENDERING_QUADSPHERE_RENDER

#include <glad/glad.h>

#include <iostream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace render::quadsphere
{

struct lod_mesh_list
{
	/// Single face of a quad sphere
	struct mesh
	{
		unsigned int vaoId;
		unsigned int vboId;
		unsigned int vertexCount;
	} meshes[16];
};

struct data
{
	lod_mesh_list mesh;

	int shaderProgramId;
	int uniform_pointTranslation;
	int uniform_viewMatrix;
	int uniform_localRotation;
};

/// Create a single face quadsphere lod meshes 
lod_mesh_list createLodMeshes(int a_subdivisions);

/// Create data to draw a quadsphere
data generateQuadsphereData(int a_subdivisions);

constexpr int c_maxLod = 12;
struct QuadTree
{
private:
	const glm::mat3& m_rotation;

public:
	const int face;
	const int x;
	const int y;
	const int size;
	const double true_size;

	const glm::dvec3 center;
	std::vector<QuadTree> children;

	// Root
	explicit QuadTree(int a_face, const glm::mat3& a_rotation)
		: QuadTree(a_face, 0, 0, 1 << c_maxLod, a_rotation)
	{}


	// Child
	explicit QuadTree(
		int a_face,
		int a_x,
		int a_y,
		int a_size,
		const glm::mat3& a_rotation)
		: m_rotation{a_rotation}
		, face{a_face}
		, x{a_x}
		, y{a_y}
		, size{a_size}
		, true_size{a_size / static_cast<double>(1 << c_maxLod)}
		, center{glm::inverse(a_rotation) * glm::normalize(glm::dvec3(
			1,
			((a_x + a_size * 0.5f) / static_cast<double>(1 << c_maxLod)) * 2 - 1.0f,
			((a_y + a_size * 0.5f) / static_cast<double>(1 << c_maxLod)) * 2 - 1.0f
		))}
	{
	}

	void print(int a_level)
	{
		glm::dvec3 nc = m_rotation * center;
		std::cout.precision(4);
		std::cout
			<< std::string(a_level, '\t') << "[x: " << x << ", y: " << y << ", size: " << size << ", t: " << true_size << "]"
			<< " (x: " << nc.x << ", y: " << nc.y << ", z: " << nc.z << ")"
			<< std::endl;
		if(!children.empty())
		{
			children.at(0).print(a_level + 1);
			children.at(1).print(a_level + 1);
			children.at(2).print(a_level + 1);
			children.at(3).print(a_level + 1);
		}
	}

	void insert(const glm::dvec3& a_position)
	{
		double dist = glm::length(a_position - center) - 0.001;

		if(dist < 3 * true_size && size > 1)
		{
			createChildren();
			for(int i = 0; i < 4; i++)
			{
				children[i].insert(a_position);
			}
		}
	}

	int getSizeOfChild(int x, int y) const
	{
		for(auto& child : children)
		{
			if((x >= child.x && x < child.x + child.size)
			&& (y >= child.y && y < child.y + child.size))
			{
				return child.getSizeOfChild(x, y);
			}
		}

		return size;
	}

	int getSize()
	{
		int size = 1;
		for(auto& child : children)
		{
			size += child.getSize();
		}

		return size;
	}

private:
	void createChildren()
	{
		const int newSize = size >> 1;
		children.emplace_back(face, x          , y          , newSize, m_rotation);
		children.emplace_back(face, x + newSize, y          , newSize, m_rotation);
		children.emplace_back(face, x          , y + newSize, newSize, m_rotation);
		children.emplace_back(face, x + newSize, y + newSize, newSize, m_rotation);
	}
};

struct QuadTreePlanet
{
public:
	const glm::mat3 rotations[6]
	{
		glm::mat3_cast(glm::angleAxis(glm::radians(   0.0f), glm::vec3(0, 0, 1))), // Front
		glm::mat3_cast(glm::angleAxis(glm::radians( -90.0f), glm::vec3(0, 0, 1))), // Right
		glm::mat3_cast(glm::angleAxis(glm::radians(-180.0f), glm::vec3(0, 0, 1))), // Back
		glm::mat3_cast(glm::angleAxis(glm::radians(-270.0f), glm::vec3(0, 0, 1))), // Left
		glm::mat3_cast(glm::angleAxis(glm::radians( -90.0f), glm::vec3(0, 1, 0))), // Top
		glm::mat3_cast(glm::angleAxis(glm::radians(  90.0f), glm::vec3(0, 1, 0))), // Bottom
	};

	QuadTree trees[6]
	{
		QuadTree(0, rotations[0]),
		QuadTree(1, rotations[1]),
		QuadTree(2, rotations[2]),
		QuadTree(3, rotations[3]),
		QuadTree(4, rotations[4]),
		QuadTree(5, rotations[5]),
	};

	// Make sure neigbouring chunks are always one step
	void insert(const glm::dvec3& a_position)
	{
		// Insert the points
		trees[0].insert(a_position);
		trees[1].insert(a_position);
		trees[2].insert(a_position);
		trees[3].insert(a_position);
		trees[4].insert(a_position);
		trees[5].insert(a_position);
	}

	/// Check if a side is larger than the specified quad tree
	bool isSideLarger(const QuadTree& a_quad, int side) const
	{
		int test_x = a_quad.x;
		int test_y = a_quad.y;
		if(side == 0) // Top
		{
			test_y += a_quad.size;
		}
		else if(side == 1) // Right
		{
			test_x += a_quad.size;
		}
		else if(side == 2) // Bottom
		{
			test_y -= 1;
		}
		else if(side == 3) // Left
		{
			test_x -= 1;
		}

		// If the points are still within the (0, 0, 1 << c_maxLod, 1 << c_maxLod) range we can just return
		if((test_x >= 0 && test_x < (1 << c_maxLod))
		&& (test_y >= 0 && test_y < (1 << c_maxLod)))
		{
			int other = trees[a_quad.face].getSizeOfChild(test_x, test_y);
			// std::printf("%d %d\n", side, other);
			return other > a_quad.size;
		}

		// TODO: Between quads

		return false;
	}
};

struct recursive_data
{
	const QuadTreePlanet& planet;
	const data& data;
	const glm::mat3& rotation;
	const glm::vec3& position;
};

inline void renderFaceQuadRecursive(
	const recursive_data& a_data,
	const QuadTree& a_quadtree,
	int& r_drawnTriangles
)
{
	if(a_quadtree.children.empty())
	{
		bool flagTop    = a_data.planet.isSideLarger(a_quadtree, 0);
		bool flagRight  = a_data.planet.isSideLarger(a_quadtree, 1);
		bool flagBottom = a_data.planet.isSideLarger(a_quadtree, 2);
		bool flagLeft   = a_data.planet.isSideLarger(a_quadtree, 3);
		int meshIndex =
			(flagTop ? 1 : 0) |
			(flagRight ? 2 : 0) |
			(flagBottom ? 4 : 0) |
			(flagLeft ? 8 : 0);

		// std::printf("[x: %3d, y: %3d, s: %3d] %d%d%d%d\n", a_quadtree.x, a_quadtree.y, a_quadtree.size,
		//	 flagTop,
		//	 flagRight,
		//	 flagBottom,
		//	 flagLeft);

		// meshIndex = 0;
		glBindVertexArray(a_data.data.mesh.meshes[meshIndex].vaoId);
		glUniform4f(a_data.data.uniform_pointTranslation,
			2 * ((1 << c_maxLod) - (a_quadtree.x + a_quadtree.size)) / static_cast<double>(1 << c_maxLod),
			2 * ((1 << c_maxLod) - (a_quadtree.y + a_quadtree.size)) / static_cast<double>(1 << c_maxLod),
			a_quadtree.true_size,
			a_quadtree.true_size);
		glDrawArrays(GL_TRIANGLES, 0, a_data.data.mesh.meshes[meshIndex].vertexCount);
		r_drawnTriangles += a_data.data.mesh.meshes[meshIndex].vertexCount / 3;
	}
	else
	{
		for(auto& child : a_quadtree.children)
		{
			renderFaceQuadRecursive(a_data, child, r_drawnTriangles);
		}
	}
}

inline void render(
	const data& a_data,
	const glm::dvec3& a_position)
{
	QuadTreePlanet planet{};
	planet.insert(a_position * glm::dvec3(1, -1, -1));

	int totalDrawnTriangles = 0;
	for(int i = 0; i < 6; i++)
	{
		glUniformMatrix3fv(a_data.uniform_localRotation, 1, GL_FALSE, glm::value_ptr(planet.rotations[i]));
		renderFaceQuadRecursive(
			recursive_data {
				planet,
				a_data,
				planet.rotations[i],
				a_position
			},
			planet.trees[i],
			totalDrawnTriangles
		);
	}

	(void) totalDrawnTriangles;

	// std::cout << "Triangles: " << totalDrawnTriangles << std::endl;
}

} // render::quadsphere

#endif  // RENDERING_QUADSPHERE_RENDER
