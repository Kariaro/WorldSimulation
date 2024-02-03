
#ifndef UTILITY_MATH_WGS84
#define UTILITY_MATH_WGS84

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>

namespace wgs84
{

constexpr const double c_PI = 3.14159265358979323846264338327950288;

constexpr const double c_A = 6378137.0;
constexpr const double c_B = 6356752.314245;
constexpr const double c_E = 0.081819190842622;

constexpr const double c_deg2rad = c_PI / 180.0;
constexpr const double c_rad2deg = 180.0 / c_PI;

constexpr glm::dvec3 lla2ecef_rad(
	double a_latitude_rad,
	double a_longitude_rad,
	double a_altitude_meter
)
{
	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);
	const double N = c_A / std::sqrt(1 - c_E * c_E * s_lat * s_lat);
	const double x = (N + a_altitude_meter) * c_lat * c_lon;
	const double y = (N + a_altitude_meter) * c_lat * s_lon;
	const double z = ((1 - c_E * c_E) * N + a_altitude_meter) * s_lat;
	return {x, y, z};
}

constexpr glm::dvec3 lla2ecef_deg(
	double a_latitude_deg,
	double a_longitude_deg,
	double a_altitude_meter
)
{
	return lla2ecef_rad(
		a_latitude_deg * c_deg2rad,
		a_longitude_deg * c_deg2rad,
		a_altitude_meter
	);
}



constexpr glm::dvec3 ecef2lla_rad(const glm::dvec3& a_ecef)
{
	const double x = a_ecef.x;
	const double y = a_ecef.y;
	const double z = a_ecef.z;

	const double rp = std::sqrt(x*x + y*y + z*z);

	double flon_rad = (std::abs(x) + std::abs(y) < 1.0e-10)
		? 0.0
		: std::atan2(y, x);

	const double p = std::sqrt(x*x + y*y);

	// on pole special case
	if(p < 1.0e-10)
	{
		double flat_rad  = (z < 0.0 ? -90 : 90.0) * c_deg2rad;
		double alt_meter = rp - c_B;
		return {
			flat_rad,
			flon_rad,
			alt_meter
		};
	}

	const double flatgc_rad = std::asin(z / rp);
	const double slat_0 = std::sin(flatgc_rad);
	const double aaa_rn = c_A / std::sqrt(1.0 - c_E * c_E * slat_0 * slat_0);

	constexpr const double c_Value = (1.0 - c_E * c_E) * (1.0 - c_E * c_E) - 1;

	// first iteration, use flatgc to get altitude
	// and alt needed to convert gc to gd lat
	double alt_meter = rp - aaa_rn * std::sqrt(1 + c_Value * slat_0 * slat_0);
	double flat_rad;
	{
		// approximation by stages 1st use gc-lat as if is gd, then correct alt dependence
		const double flatgd_rad = std::atan(std::tan(flatgc_rad) / (1 - c_E * c_E * aaa_rn / (aaa_rn + alt_meter)));

		// now use this approximation for gd-lat to get rn etc
		const double slat_1 = std::sin(flatgd_rad);
		const double rn = c_A / std::sqrt(1.0 - c_E * c_E * slat_1 * slat_1);
		flat_rad = std::atan(std::tan(flatgc_rad) / (1 - c_E * c_E * rn / (rn + alt_meter)));
	}

	double slat = std::sin(flat_rad);
	double rn   = c_A / std::sqrt(1.0 - c_E * c_E * slat * slat);

	for(int i = 0; i < 5; i++)
	{
		double tangd = (z + rn * c_E * c_E * slat) / p;
		double flatn_rad = std::atan(tangd);

		slat      = std::sin(flatn_rad);
		rn        = c_A / std::sqrt(1.0 - c_E * c_E * slat * slat);
		alt_meter = (p / std::cos(flatn_rad)) - rn;

		double dlat_rad = flatn_rad - flat_rad;
		flat_rad = flatn_rad;
		if(std::abs(dlat_rad) < 1.0e-14)
		{
			break;
		}
	}

	return {
		flat_rad,
		flon_rad,
		alt_meter
	};
}

constexpr glm::dvec3 ecef2lla_deg(const glm::dvec3& a_ecef)
{
	auto lla_rad = ecef2lla_rad(a_ecef);
	return {
		lla_rad.x * c_rad2deg,
		lla_rad.y * c_rad2deg,
		lla_rad.z // altitude
	};
}

// Calculate the viewing direction
// I call this the North West Up
// TODO: Verify function
constexpr glm::dmat3 lla2nwu_rad(
	double a_latitude_rad,
	double a_longitude_rad)
{
	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);
	// return glm::dmat3(
	// 	-s_lat * c_lon, -s_lat * s_lon,  c_lat,
	// 	-s_lon        ,  c_lon        ,  0.0,
	// 	-c_lat * c_lon, -c_lat * s_lon, -s_lat
	// );
	return glm::dmat3(
		-s_lat * c_lon, -s_lat * s_lon,  c_lat,
		 s_lon        , -c_lon        , -0.0,
		 c_lat * c_lon,  c_lat * s_lon,  s_lat
	);
}

constexpr glm::dmat3 lla2nwu_deg(
	double a_latitude_deg,
	double a_longitude_deg)
{
	return lla2nwu_rad(
		a_latitude_deg * c_deg2rad,
		a_longitude_deg * c_deg2rad
	);
}

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

/// Calculate the current yaw, pitch and roll
/*
constexpr glm::dvec3 quat2rpy(const glm::dvec3& )
{
	auto lla_rad = ecef2lla_rad(a_ecef);
	return {
		lla_rad.x * c_rad2deg,
		lla_rad.y * c_rad2deg,
		lla_rad.z // altitude
	};
}
*/

} // wgs84

#endif  // UTILITY_MATH_WGS84
