
#ifndef RENDERING_PLANE
#define RENDERING_PLANE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "wgs84.h"


namespace render::plane
{

struct Plane
{
    // Position xyz in 3d
    glm::dvec3 position{};
    glm::dquat rotation{glm::identity<glm::dquat>()};

    [[nodiscard]] glm::dmat3 getLocalTangentPlane() const
    {
        auto lla = wgs84::ecef2lla_deg(position);
        return wgs84::lla2nwu_deg(lla.x, lla.y);
    }

    [[nodiscard]] glm::dvec3 getForwardVector() const
    {
        return rotation * glm::dvec3(1, 0, 0);
    }

    [[nodiscard]] double getAltitude() const
    {
        auto lla = wgs84::ecef2lla_deg(position);
        return lla.z;
    }

    [[nodiscard]] double getLatitude() const
    {
        auto lla = wgs84::ecef2lla_deg(position);
        return lla.x;
    }

    [[nodiscard]] double getLongitude() const
    {
        auto lla = wgs84::ecef2lla_deg(position);
        return lla.y;
    }
};

} // render::plane

#endif  // RENDERING_PLANE
