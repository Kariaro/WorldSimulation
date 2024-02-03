
#ifndef RENDERING_PLANET_H
#define RENDERING_PLANET_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "image.h"
#include "quadspherelod.h"

#include <thread>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <stb_image.h>

#include "wgs84.h"
#include "geometry.h"

#include "plane.h"

#include <glm/gtx/vector_angle.hpp>

namespace render
{


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

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));

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

const float pixelToMeter = 0.00138737377388;
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
    if(shaderProgramId == 0)
    {
        std::printf("Failed to compile shader\n");
        return;
    }
    std::printf("Shader compiled %d\n", shaderProgramId);
    int uniform_pointTranslation = glGetUniformLocation(shaderProgramId, "pointTranslation");
    int uniform_viewMatrix = glGetUniformLocation(shaderProgramId, "viewMatrix");
    int uniform_localRotation = glGetUniformLocation(shaderProgramId, "localRotation");

    quadsphere::quadsphere_face_mesh face_mesh = quadsphere::createFaceMesh(4);
    std::printf("vaoId=%u, vboId=%u, vertices=%u\n", face_mesh.vaoId, face_mesh.vboId, face_mesh.vertexCount);

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);

    camera::Camera renderCamera;
    camera::Camera lodCamera;

    using namespace std::chrono;

    int frames = 0;

    auto geometry_test_data = geometry::test();

    glActiveTexture(GL_TEXTURE0);
    unsigned int earthTextureId = image::loadImage("assets/earth_texture_10k.jpg");
    std::printf("Read texture %u\n", earthTextureId);

    glActiveTexture(GL_TEXTURE1);
    unsigned int earthDepthmapId = image::loadImage("assets/earth_depthmap_10k.jpg");
    std::printf("Read texture %u\n", earthDepthmapId);

    glUseProgram(shaderProgramId);
    glUniform1i(glGetUniformLocation(shaderProgramId, "earthTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgramId, "earthDepthmap"), 1);
    glUseProgram(0);

    bool isDown = false;

    int speedZoom = 18;
    bool show_wireframe = false;
    auto test = quadsphere::createSingleFace(4);

    renderCamera.position     = wgs84::lla2ecef_deg(0, 0, 3'000'000);
    renderCamera.rotationTest = glm::quat_cast(wgs84::lla2nwu_deg(90, 0));

    int cameraMode = 0;

    
    plane::Plane testPlane;
    testPlane.position = wgs84::lla2ecef_deg(0, 0, 1000);
    testPlane.rotation = glm::quat_cast(wgs84::lla2nwu_deg(0, 0));

    {
        double lat = 27.986065;
        double lon = 86.922623;
        testPlane.position = wgs84::lla2ecef_deg(lat, lon, 1000);
        testPlane.rotation = glm::quat_cast(wgs84::lla2nwu_deg(lat, lon));
    }

    constexpr double targetFps = 60;
    auto fpsDuration = nanoseconds(static_cast<uint64_t>(1'000'000'000 / targetFps));
    auto nextSecond = high_resolution_clock::now();
    auto nextFrame = high_resolution_clock::now();
    while(!glfwWindowShouldClose(window))
    {
        std::this_thread::sleep_until(nextFrame);
        nextFrame += fpsDuration;

        frames ++;
        if(high_resolution_clock::now() > nextSecond)
        {
            std::printf("Frames %d\n", frames);
            frames = 0;
            nextSecond += seconds(1);
        }
        
        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        int yawDir = 0; (void) yawDir;
        constexpr double rotationSpeed = 2;
        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) renderCamera.rotation.y -= rotationSpeed;
        if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) renderCamera.rotation.y += rotationSpeed;
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { yawDir++; renderCamera.rotation.z += rotationSpeed; }
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { yawDir--; renderCamera.rotation.z -= rotationSpeed; }

        int zz = 0;
        int xx = 0;
        int yy = 0;
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) xx++;
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) xx--;
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) yy++;
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) yy--;
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) zz++;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) zz--;
        

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if(!isDown)
            {
                isDown = true;
                speedZoom++;
            }
        }
        else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if(!isDown)
            {
                isDown = true;
                speedZoom--;
            }
        }
        else if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            if(!isDown)
            {
                isDown = true;
                show_wireframe = !show_wireframe;
            }
        }
        else if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        {
            if(!isDown)
            {
                isDown = true;
                cameraMode += 1;
                cameraMode %= 3;
            }
        }
        else
        {
            isDown = false;
        }

        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        const double speed_mps = std::pow(2, speedZoom) / 60.0; (void) speed_mps;

        {
            glm::dvec3 forwardVector = testPlane.getForwardVector();
            glm::dvec3 newPosition =
                testPlane.position
                + (forwardVector * glm::dvec3(xx * speed_mps));

            // Calculate fly height
            double flyHeight = testPlane.getAltitude() + (zz * speed_mps / 2.0);
            flyHeight = std::max(-1'000'000.0, flyHeight);

            // Calculate gravity up and clamp position
            glm::dvec3 gravityUp;
            {
                auto nwu = testPlane.getLocalTangentPlane();
                auto lla = wgs84::ecef2lla_deg(newPosition);

                gravityUp = nwu * glm::vec3(0, 0, 1);
                newPosition = wgs84::lla2ecef_deg(lla.x, lla.y, flyHeight);
            }
            
            testPlane.position = newPosition;

            // Rotate around
            testPlane.rotation = glm::quat_cast(
                glm::rotate(glm::dmat4(1.0), glm::radians(yawDir * 2 + 0.0), gravityUp)
                * glm::mat4_cast(testPlane.rotation)
            );

            // Apply curvature fix
            {
                glm::dvec3 planeUp = testPlane.rotation * glm::dvec3(0, 0, 1);

                glm::dvec3 aFrom = planeUp;
                glm::dvec3 aTo = gravityUp;

                glm::dvec3 axis = glm::normalize(glm::cross(aFrom, aTo));

                // If angles are exactly the same we do not need to correct rotation
                if(!glm::any(glm::isnan(axis)))
                {
                    double angle = glm::angle(aFrom, aTo);
                    glm::dquat fromToRotation = glm::angleAxis(angle, glm::normalize(axis));
                    testPlane.rotation = fromToRotation * testPlane.rotation;
                }
            }

            // Calculate planes yaw pitch roll
            {
                auto nwu = testPlane.getLocalTangentPlane();
                auto localForward = glm::inverse(glm::quat_cast(nwu)) * (testPlane.rotation * glm::dvec3(1, 0, 0));
                
                double yaw_rad = std::atan2(localForward.y, localForward.x);
                testPlane.rotation = 
                    glm::angleAxis(yaw_rad, gravityUp)
                    * glm::quat_cast(nwu)
                ;
            }

            if(cameraMode == 0) // Top down
            {
                auto lla = wgs84::ecef2lla_deg(testPlane.position);
                renderCamera.position = wgs84::lla2ecef_deg(lla.x, lla.y, lla.z + 3'000'000);
                renderCamera.rotationTest = glm::quat_cast(testPlane.getLocalTangentPlane()
                    * glm::dmat3(glm::angleAxis(glm::radians(90.0), glm::dvec3(0, 1, 0))));
            }
            else if(cameraMode == 1) // FOV
            {
                renderCamera.rotationTest = testPlane.rotation;
                renderCamera.position = testPlane.position;
            }
        }

        {
            auto lla = wgs84::ecef2lla_deg(testPlane.position);
            std::printf("lat: %.4f, lon: %.4f, alt: %.4f, speed: %.4f m/s\n",
                lla.x,
                lla.y,
                lla.z,
                speed_mps * 60.0);
        }

        const double s_z = glm::sin(glm::radians(renderCamera.rotation.z));
        const double c_z = glm::cos(glm::radians(renderCamera.rotation.z));
        double xa = c_z * xx - s_z * yy; (void) xa;
        double ya = s_z * xx + c_z * yy; (void) ya;
        // renderCamera.position.x += xa * speed / 60.0;
        // renderCamera.position.y += ya * speed / 60.0;
        // renderCamera.position.z += zz * speed / 60.0;

        // Clamp camera rotation
        // renderCamera.rotation.y = std::clamp(renderCamera.rotation.y, -90.0, 90.0);
        // renderCamera.rotation.z = std::fmod(renderCamera.rotation.z, 360.0);

        glm::mat4 viewMatrix = glm::translate(glm::dmat4(renderCamera.getPerspectiveMatrix(
            windowWidth,
            windowHeight
        )
        * glm::inverse(renderCamera.getCameraMatrix())
        ), glm::dvec3(-renderCamera.position) * glm::dvec3(1.0 / wgs84::c_A));

        {
            // Draw plane
            glUseProgram(geometry_test_data.programId);
            glm::dmat4 lodViewMatrixDouble = glm::translate(
                glm::dmat4(viewMatrix),
                glm::dvec3(testPlane.position) * glm::dvec3(1.0 / wgs84::c_A, 1.0 / wgs84::c_A, 1.0 / wgs84::c_B));
            lodViewMatrixDouble = glm::scale(lodViewMatrixDouble, glm::dvec3(0.2));

            lodViewMatrixDouble = lodViewMatrixDouble * glm::dmat4(testPlane.rotation);

            glm::mat4 lodViewMatrix = lodViewMatrixDouble;
            glUniformMatrix4fv(geometry_test_data.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(lodViewMatrix));
            if(cameraMode != 1)
            {
                geometry_test_data.xyz_arrow.draw();
            }
        }

        geometry::test_draw(
            geometry_test_data,
            glm::identity<glm::mat4>(),
            viewMatrix,
            windowWidth,
            windowHeight
        );

        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        {
            /*
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUseProgram(shaderProgramId);
            glEnableVertexAttribArray(0);
            glUniformMatrix3fv(uniform_localRotation, 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat3>()));
            for(size_t i = 0; i < 16; i++)
            {
                auto& item = test.meshes[i];
                glBindVertexArray(item.vaoId);
                glUniform4f(uniform_pointTranslation, 0.0f, 0.0f, 1.0f, 1.0f);
                glUniformMatrix4fv(uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(glm::translate(viewMatrix, glm::vec3(0, 2 * i, 2))));
                glDrawArrays(GL_TRIANGLES, 0, item.vertexCount);
            }
            */

            camera::Camera& lodCam = renderCamera; // lodCamera
            /*
            {
                glUseProgram(geometry_test_data.programId);
                glm::dmat4 lodViewMatrixDouble = glm::translate(
                    glm::dmat4(viewMatrix),
                    glm::dvec3(lodCam.position) * glm::dvec3(1.0 / wgs84::c_A));
                lodViewMatrixDouble = glm::scale(lodViewMatrixDouble, glm::dvec3(0.1));

                auto lla_deg = wgs84::ecef2lla(lodCam.position);
                auto nuw_view = wgs84::lla2nwu(lla_deg.x, lla_deg.y); (void) nuw_view;
                lodViewMatrixDouble = lodViewMatrixDouble * glm::dmat4(nuw_view);

                glm::mat4 lodViewMatrix = lodViewMatrixDouble;
                glUniformMatrix4fv(geometry_test_data.uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(lodViewMatrix));
                geometry_test_data.xyz_arrow.draw();
            }
            */

            if(show_wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            glUseProgram(shaderProgramId);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, earthTextureId);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, earthDepthmapId);
            glm::mat4 test_matrix = glm::scale(
                viewMatrix, glm::vec3(1, 1, wgs84::c_B / wgs84::c_A)
            );
            glUniformMatrix4fv(uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(test_matrix)); // viewMatrix));
            quadsphere::renderFaceQuad(test, lodCam.position * glm::dvec3(1.0 / wgs84::c_A), uniform_localRotation, uniform_pointTranslation);
            glUseProgram(0);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

} // render

#endif  // RENDERING_PLANET_H
