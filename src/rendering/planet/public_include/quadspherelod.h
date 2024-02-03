
#ifndef RENDERING_QUADSPHERE_LOD
#define RENDERING_QUADSPHERE_LOD

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

/// Single face of a quad sphere
struct quadsphere_face_mesh
{
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int vertexCount;
};

struct quadsphere_mesh
{
    quadsphere_face_mesh meshes[16];
};

quadsphere_mesh createSingleFace(int a_subdivisions)
{
    // 0 - 1 - 2
    // | \ | / |
    // 3 - 4 - 5
    // | / | \ |
    // 6 - 7 - 8

    struct triangle {
        struct point
        {
            int x;
            int y;
        } points[3];
    };
    
    std::vector<triangle> triangles = {
        // Top
        { { { 1, 1 }, { 0, 0 }, { 1, 0 } } },
        { { { 1, 1 }, { 1, 0 }, { 2, 0 } } },

        // Right
        { { { 1, 1 }, { 2, 0 }, { 2, 1 } } },
        { { { 1, 1 }, { 2, 1 }, { 2, 2 } } },

        // Bottom
        { { { 1, 1 }, { 2, 2 }, { 1, 2 } } },
        { { { 1, 1 }, { 1, 2 }, { 0, 2 } } },

        // Left
        { { { 1, 1 }, { 0, 2 }, { 0, 1 } } },
        { { { 1, 1 }, { 0, 1 }, { 0, 0 } } },
    };

    std::vector<float> bufferVertices;
    const auto addVert = [&bufferVertices, &a_subdivisions](const triangle::point& a_point, int offset_x, int offset_y)
    {
        double x_value = 2.0 * static_cast<double>(a_point.x + offset_x * 2) / (2.0 * a_subdivisions) - 1.0;
        double y_value = 2.0 * static_cast<double>(a_point.y + offset_y * 2) / (2.0 * a_subdivisions) - 1.0;
        bufferVertices.push_back(-x_value);
        bufferVertices.push_back(y_value);
    };

    quadsphere_mesh all_mesh{};

    // std::cout << "Meshes" << std::endl;
    for(int meshIndex = 0; meshIndex < 16; meshIndex++)
    {
        // Clear the vertices buffer
        bufferVertices.clear();

        // std::cout << "  - Mesh " << meshIndex << std::endl;

        for(int y = 0; y < a_subdivisions; y++)
        {
            for(int x = 0; x < a_subdivisions; x++)
            {
                // Calculate the side flags of the subdivision
                int subdivisionSide =
                    ((y == 0) ? 1 : 0) |
                    ((x + 1 == a_subdivisions) ? 2 : 0) |
                    ((y + 1 == a_subdivisions) ? 4 : 0) |
                    ((x == 0) ? 8 : 0);

                // Add the triangle buffer at the specified position
                for(size_t triangleIndex = 0; triangleIndex < triangles.size(); triangleIndex++)
                {
                    int triangleSide = 1 << static_cast<int>(triangleIndex >> 1);

                    // std::cout << "    - ["
                    //     << "x: " << x
                    //     << ", y: " << y
                    //     << ", side: " << triangleSide
                    //     << ", trig_idx: " << triangleIndex
                    //     << ", mask: " << (meshIndex & triangleSide)
                    //     << "]" << std::endl;
                    if((meshIndex & triangleSide & subdivisionSide) != 0)
                    {
                        addVert(triangles.at(triangleIndex    ).points[0], x, y);
                        addVert(triangles.at(triangleIndex    ).points[1], x, y);
                        addVert(triangles.at(triangleIndex + 1).points[2], x, y);
                        triangleIndex++;
                    }
                    else
                    {
                        addVert(triangles.at(triangleIndex).points[0], x, y);
                        addVert(triangles.at(triangleIndex).points[1], x, y);
                        addVert(triangles.at(triangleIndex).points[2], x, y);
                    }
                }
            }
        }

        auto& mesh = all_mesh.meshes[meshIndex];

        mesh.vertexCount = bufferVertices.size() / 2;
        glGenVertexArrays(1, &mesh.vaoId);
        glBindVertexArray(mesh.vaoId);
        glEnableVertexAttribArray(0);

        //r_outputMesh.vertexCount = 3;
        glGenBuffers(1, &mesh.vboId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVertices.size(), bufferVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    return all_mesh;
}

/// Creates a mesh with a specific amount of subdivisions
/// 
/// @param a_subdivisions
/// @return 
quadsphere_face_mesh createFaceMesh(int a_subdivisions)
{
    // 0 - 1 - 2
    // | \ | / |
    // 3 - 4 - 5
    // | / | \ |
    // 6 - 7 - 8

    struct triangle {
        struct point
        {
            int x;
            int y;
        } points[3];
    };
    
    std::vector<triangle> triangles = {
        // Top left
        { { { 1, 1 }, { 0, 1 }, { 0, 0 } } },
        { { { 1, 1 }, { 0, 0 }, { 1, 0 } } },

        // Top right
        { { { 1, 1 }, { 2, 0 }, { 2, 1 } } },
        { { { 1, 1 }, { 1, 0 }, { 2, 0 } } },

        // Bot right
        { { { 1, 1 }, { 2, 1 }, { 2, 2 } } },
        { { { 1, 1 }, { 2, 2 }, { 1, 2 } } },

        // Bot left
        { { { 1, 1 }, { 1, 2 }, { 0, 2 } } },
        { { { 1, 1 }, { 0, 2 }, { 0, 1 } } },
    };

    std::vector<float> bufferVertices;
    for(int y = 0; y < a_subdivisions; y++)
    {
        for(int x = 0; x < a_subdivisions; x++)
        {
            // Add the triangle buffer at the specified position
            for(auto& item : triangles)
            {
                for(size_t p = 0; p < 3; p++)
                {
                    auto& point = item.points[p];
                    bufferVertices.push_back(2 * static_cast<float>(point.x + x * 2) / (2.0 * a_subdivisions) - 1);
                    bufferVertices.push_back(2 * static_cast<float>(point.y + y * 2) / (2.0 * a_subdivisions) - 1);
                }
            }
        }
    }

    quadsphere_face_mesh mesh{};
    mesh.vertexCount = bufferVertices.size() / 2;
    glGenVertexArrays(1, &mesh.vaoId);
    glBindVertexArray(mesh.vaoId);
    glEnableVertexAttribArray(0);

    //r_outputMesh.vertexCount = 3;
    glGenBuffers(1, &mesh.vboId);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVertices.size(), bufferVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return mesh;
}

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
    const quadsphere_mesh& meshes;
    const glm::mat3& rotation;
    const glm::vec3& position;
    int uniform_localRotation;
    int uniform_pointTranslation;
};

void renderFaceQuadRecursive(
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
        //     flagTop,
        //     flagRight,
        //     flagBottom,
        //     flagLeft);
        // 
        // meshIndex = 0;
        glBindVertexArray(a_data.meshes.meshes[meshIndex].vaoId);
        glUniform4f(a_data.uniform_pointTranslation,
            2 * ((1 << c_maxLod) - (a_quadtree.x + a_quadtree.size)) / static_cast<double>(1 << c_maxLod),
            2 * ((1 << c_maxLod) - (a_quadtree.y + a_quadtree.size)) / static_cast<double>(1 << c_maxLod),
            a_quadtree.true_size,
            a_quadtree.true_size);
        glDrawArrays(GL_TRIANGLES, 0, a_data.meshes.meshes[meshIndex].vertexCount);
        r_drawnTriangles += a_data.meshes.meshes[meshIndex].vertexCount / 3;
    }
    else
    {
        for(auto& child : a_quadtree.children)
        {
            renderFaceQuadRecursive(a_data, child, r_drawnTriangles);
        }
    }
}

int lastSize = 0;
void renderFaceQuad(
    const quadsphere_mesh& a_mesh,
    const glm::dvec3& a_position,
    const int a_uniform_localRotation,
    const int a_uniform_pointTranslation)
{
    QuadTreePlanet planet{};
    planet.insert(a_position * glm::dvec3(1, -1, -1));

    int totalDrawnTriangles = 0;
    for(int i = 0; i < 6; i++)
    {
        glUniformMatrix3fv(a_uniform_localRotation, 1, GL_FALSE, glm::value_ptr(planet.rotations[i]));
        renderFaceQuadRecursive(
            recursive_data {
                planet,
                a_mesh,
                planet.rotations[i],
                a_position,
                a_uniform_localRotation,
                a_uniform_pointTranslation
            },
            planet.trees[i],
            totalDrawnTriangles
        );
    }

    std::cout << "Triangles: " << totalDrawnTriangles << std::endl;

    // glUniform4f(a_uniform_pointTranslation, 0.0f, 0.0f, 1.0f, 1.0f);
    // glUniformMatrix3fv(a_uniform_localRotation, 1, GL_FALSE, glm::value_ptr(a_rotation));
    // glDrawArrays(GL_TRIANGLES, 0, a_mesh.vertexCount);
}

} // render::quadsphere

#endif  // RENDERING_QUADSPHERE_LOD
