
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

#include "wgs84.hpp"

using namespace geodecy;

namespace render::plane
{

struct Plane
{
	glm::dvec3 position{};
	glm::dquat rotation{glm::identity<glm::dquat>()};

	[[nodiscard]] glm::dmat3 getLocalTangentPlane() const
	{
		auto lla = wgs84::deg::xyz2lla(position);
		return wgs84::deg::lla2nwu(lla.x, lla.y);
	}

	[[nodiscard]] glm::dvec3 getForwardVector() const
	{
		return rotation * glm::dvec3(1, 0, 0);
	}

	[[nodiscard]] double getAltitude() const
	{
		auto lla = wgs84::deg::xyz2lla(position);
		return lla.z;
	}

	[[nodiscard]] double getLatitude() const
	{
		auto lla = wgs84::deg::xyz2lla(position);
		return lla.x;
	}

	[[nodiscard]] double getLongitude() const
	{
		auto lla = wgs84::deg::xyz2lla(position);
		return lla.y;
	}

	[[nodiscard]] glm::dvec3 getLLA() const
	{
		return wgs84::deg::xyz2lla(position);
	}

	/// Get local tangent plane (roll pitch yaw)
	[[nodiscard]] glm::dvec3 getLTP_RPY_rad() const
	{
		// rotation = nwu * rpy
		// localRotation = (nwu ^ -1) * nwu * rpy = rpy
		const auto nwu = getLocalTangentPlane();
		const auto localRotation = glm::inverse(glm::quat_cast(nwu)) * rotation;

		// GLM assumes XYZ we use YZX therefore, we need to move these methods arround
		double roll = glm::pitch(localRotation);
		double pitch = glm::yaw(localRotation);
		double yaw = glm::roll(localRotation);

		// TODO: Is this needed?
		if(!std::isfinite(roll)) roll = 0;
		if(!std::isfinite(pitch)) pitch = 0;
		if(!std::isfinite(yaw)) yaw = 0;

		return { roll, pitch, yaw };
	}

	/// Get local tangent plane (roll pitch yaw)
	[[nodiscard]] glm::dvec3 getLTP_RPY_deg()
	{
		const auto rpy = getLTP_RPY_rad();
		return {
			rpy.x * geodecy::c_rad2deg,
			rpy.y * geodecy::c_rad2deg,
			rpy.z * geodecy::c_rad2deg
		};
	}
};

} // render::plane

#endif  // RENDERING_PLANE
