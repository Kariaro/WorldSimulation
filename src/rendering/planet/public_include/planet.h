
#ifndef RENDERING_PLANET_H
#define RENDERING_PLANET_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

namespace render
{

bool shader(
    const char* a_vertexShader,
    const char* a_fragmentShader,
    int& r_programId)
{
    r_programId = 0;

    // Compile vertex shader
    int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    if(vertexShaderId == 0)
    {
        std::printf("Failed to create vertex shader. glCreateShader(GL_VERTEX_SHADER) returned 0\n");
        return false;
    }

    int shaderStatus = 0;
    glShaderSource(vertexShaderId, 1, &a_vertexShader, nullptr);
    glCompileShader(vertexShaderId);
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &shaderStatus);
    if(shaderStatus == 0)
    {
        int bufferLength = 0;
        glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &bufferLength);
        char* buffer = new char[bufferLength + 1];
        buffer[bufferLength] = '\0';
        glGetShaderInfoLog(vertexShaderId, bufferLength, &bufferLength, buffer);
        std::printf("Error compiling vertex shader '%s'\n", buffer);
        return false;
    }

    // Compile fragment shader
    int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    if(fragmentShaderId == 0)
    {
        std::printf("Failed to create fragment shader. glCreateShader(GL_FRAGMENT_SHADER) returned 0\n");
        return false;
    }

    glShaderSource(fragmentShaderId, 1, &a_fragmentShader, nullptr);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderStatus);
    if(shaderStatus == 0)
    {
        int bufferLength = 0;
        glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &bufferLength);
        char* buffer = new char[bufferLength + 1];
        buffer[bufferLength] = '\0';
        glGetShaderInfoLog(fragmentShaderId, bufferLength, &bufferLength, buffer);
        std::printf("Error compiling fragment shader '%s'\n", buffer);
        return false;
    }

    int programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    r_programId = programId;
    return true;
}

struct mesh
{
    unsigned int vaoId;
    unsigned int vboId;
    unsigned int vertexCount;
};

bool createMesh(
    size_t a_subdivisions,
    mesh& r_outputMesh)
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
    for(size_t y = 0; y < a_subdivisions; y++)
    {
        for(size_t x = 0; x < a_subdivisions; x++)
        {
            // Add the triangle buffer at the specified position
            for(auto& item : triangles)
            {
                for(size_t p = 0; p < 3; p++)
                {
                    auto& point = item.points[p];
                    int xp = (point.x + x * 2) * 4095 / (2 * a_subdivisions);
                    int yp = (point.y + y * 2) * 4095 / (2 * a_subdivisions);
                    // float xy = static_cast<float>(yp * 4096 + xp);
                    (void) xp;
                    (void) yp;
                    bufferVertices.push_back(static_cast<float>(point.x + x * 2) / (2.0 * a_subdivisions) - 0.5f);
                    bufferVertices.push_back(static_cast<float>(point.y + y * 2) / (2.0 * a_subdivisions) - 0.5f);
                }
            }
        }
    }

    r_outputMesh.vertexCount = bufferVertices.size() / 2;
    glGenVertexArrays(1, &r_outputMesh.vaoId);
    glBindVertexArray(r_outputMesh.vaoId);
    glEnableVertexAttribArray(0);

    //r_outputMesh.vertexCount = 3;
    glGenBuffers(1, &r_outputMesh.vboId);
    glBindBuffer(GL_ARRAY_BUFFER, r_outputMesh.vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVertices.size(), bufferVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

void render_test()
{
    if(!glfwInit())
    {
        std::printf("Failed to init GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "World Simulation", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    // https://www.desmos.com/3d/c7e69b09cd
    int shaderProgramId = 0;
    bool shaderResult = shader(
// Vertex
R"glsl(
#version 330 core
layout (location = 0) in vec2 inPosXY;

void main() {
    gl_Position = vec4(normalize(vec3(inPosXY, 1.0)), 1.0);
    gl_Position.z = 0;
}
)glsl",

// Fragment
R"glsl(
#version 330 core
out vec4 outColor;

void main() {
	outColor = vec4(1.0, 0.0, 1.0, 1.0);
}
)glsl",
shaderProgramId);
    if(!shaderResult)
    {
        std::printf("Failed to compile shader\n");
        return;
    }
    std::printf("Shader compiled %d\n", shaderProgramId);

    mesh test_mesh{};
    if(!createMesh(40, test_mesh))
    {
        std::printf("Failed to create mesh\n");
        return;
    }
    std::printf("vaoId=%u, vboId=%u, vertices=%u\n", test_mesh.vaoId, test_mesh.vboId, test_mesh.vertexCount);

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glViewport(0, 0, 3, 3);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // glBegin(GL_TRIANGLES);
        //     glColor3f(1,0,0);
        //     glVertex2f(0, 0);
        //     glColor3f(0,1,0);
        //     glVertex2f(1, 0);
        //     glColor3f(0,0,1);
        //     glVertex2f(1, 1);
        // glEnd();


        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glUseProgram(shaderProgramId);
            glBindVertexArray(test_mesh.vaoId);
            glEnableVertexAttribArray(0);

            glDrawArrays(GL_TRIANGLES, 0, test_mesh.vertexCount);
            // glDrawElements(GL_TRIANGLES, test_mesh.vertexCount, GL_UNSIGNED_INT, nullptr);

            glDisableVertexAttribArray(0);
            glBindVertexArray(0);
            glUseProgram(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

} // render

#endif  // RENDERING_PLANET_H
