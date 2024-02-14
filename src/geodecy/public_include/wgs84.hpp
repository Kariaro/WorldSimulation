
#ifndef GEODECY_WGS84
#define GEODECY_WGS84

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>

namespace geodecy
{

constexpr const double c_PI = 3.14159265358979323846264338327950288;
constexpr const double c_deg2rad = c_PI / 180.0;
constexpr const double c_rad2deg = 180.0 / c_PI;

glm::dmat3 rpy_rad(
	double a_roll_rad,
	double a_pitch_rad,
	double a_yaw_rad)
{
	return glm::mat3_cast(
		glm::angleAxis(a_yaw_rad,   glm::dvec3(0, 0, 1)) *
		glm::angleAxis(a_pitch_rad, glm::dvec3(0, 1, 0)) *
		glm::angleAxis(a_roll_rad,  glm::dvec3(1, 0, 0))
	);
}

glm::dmat3 rpy(
	double a_roll_deg,
	double a_pitch_deg,
	double a_yaw_deg)
{
	return rpy_rad(
		a_roll_deg * c_deg2rad,
		a_pitch_deg * c_deg2rad,
		a_yaw_deg * c_deg2rad
	);
}

} // geodecy::wgs84


// TODO: Use this in the future?
#include "spheroid.hpp"
namespace geodecy {

struct wgs84_glm_allocator
{
	using Type = double;
	using Vec3 = glm::dvec3;
	using Mat3 = glm::dmat3;

	static constexpr Mat3 to_mat(
		double a_x0, double a_y0, double a_z0,
		double a_x1, double a_y1, double a_z1,
		double a_x2, double a_y2, double a_z2)
	{
		return Mat3(
			a_x0, a_y0, a_z0,
			a_x1, a_y1, a_z1,
			a_x2, a_y2, a_z2);
	}

	static constexpr Vec3 to_vec(
		double a_x, double a_y, double a_z)
	{
		return Vec3(a_x, a_y, a_z);	
	}

	static constexpr Type get_x(const Vec3& a_vec)
	{
		return a_vec.x;
	}

	static constexpr Type get_y(const Vec3& a_vec)
	{
		return a_vec.y;
	}

	static constexpr Type get_z(const Vec3& a_vec)
	{
		return a_vec.z;
	}
};

using wgs84 = spheroid<wgs84_glm_allocator, 6378137.0, 6356752.314245>;

} // geodecy

#endif  // GEODECY_WGS84
