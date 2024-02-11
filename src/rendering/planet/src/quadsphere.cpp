
#include "quadsphere.h"

#include "shader.h"

namespace render::quadsphere
{

lod_mesh_list createLodMeshes(int a_subdivisions)
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

    lod_mesh_list all_mesh{};
    for(int meshIndex = 0; meshIndex < 16; meshIndex++)
    {
        // Clear the vertices buffer
        bufferVertices.clear();

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

        glGenBuffers(1, &mesh.vboId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVertices.size(), bufferVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    return all_mesh;
}

data generateQuadsphereData(int a_subdivisions)
{
    // https://www.desmos.com/3d/c7e69b09cd
    int shaderProgramId = shader::compileShader(
// Vertex
R"glsl(
#version 330 core
layout (location = 0) in vec2 inPosXY;

uniform mat4 viewMatrix;
uniform vec4 pointTranslation;
uniform float test;

uniform sampler2D earthDepthmap;

uniform mat3 localRotation;

out vec3 fragTestPosition;

const float pixelToMeter = 0.00138737377388 * 10;
//0.00000541942880421; // (8848.86 / 256.0) / 6378137.0;

void main() {
    float x = (inPosXY.x + 1) * pointTranslation.z + pointTranslation.x - 1;
    float y = (inPosXY.y + 1) * pointTranslation.w + pointTranslation.y - 1;

    // Apply some texture stuff
    // vec3 pos = localRotation * vec3(1, x, y);
    // float px = pos.x;
    // float py = pos.y;
    // float pz = pos.z;
    // pos = vec3(
    //     px * sqrt(1 - (py*py / 2.0) - (pz*pz / 2.0) + (py*py*pz*pz / 3.0)),
    //     py * sqrt(1 - (pz*pz / 2.0) - (px*px / 2.0) + (pz*pz*px*px / 3.0)),
    //     pz * sqrt(1 - (px*px / 2.0) - (py*py / 2.0) + (px*px*py*py / 3.0))
    // );

    vec3 pos = localRotation * normalize(vec3(1, x, y));
    vec2 textureXY = vec2(
        mod(degrees(-atan(pos.y, -pos.x)) + 720, 360) / 360.0,
        mod(degrees(-asin(pos.z)) + 270, 180) / 180.0
    );
    fragTestPosition = pos;
    //pos = localRotation * vec3(1, x, y);
    pos *= 1 + texture(earthDepthmap, textureXY).x * pixelToMeter;

    gl_Position = viewMatrix * vec4(pos, 1.0);
    
}
)glsl",

// Fragment
R"glsl(
#version 330 core
out vec4 outColor;

in vec3 fragTestPosition;

uniform sampler2D earthTexture;

void main() {
    vec2 latLonDeg = vec2(
        degrees(-atan(fragTestPosition.y, -fragTestPosition.x)),
        degrees(-asin(fragTestPosition.z))
    );

    float x = mod(latLonDeg.x + 720, 360.0) / 360.0;
    float y = mod(latLonDeg.y + 270, 180.0) / 180.0;
    vec3 col = texture(earthTexture, vec2(x, y)).rgb;
    outColor = vec4(col, 1.0);
}

)glsl");

    data result{};
    if(shaderProgramId == 0)
    {
        std::printf("Failed to compile shader\n");
        return result;
    }
    std::printf("Shader compiled %d\n", shaderProgramId);

    result.mesh = createLodMeshes(a_subdivisions);
    result.shaderProgramId = shaderProgramId;
    result.uniform_pointTranslation = glGetUniformLocation(shaderProgramId, "pointTranslation");
    result.uniform_viewMatrix = glGetUniformLocation(shaderProgramId, "viewMatrix");
    result.uniform_localRotation = glGetUniformLocation(shaderProgramId, "localRotation");
    return result;
}

} // render::quadsphere
