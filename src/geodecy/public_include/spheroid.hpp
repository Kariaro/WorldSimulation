
#ifndef GEODECY_SPHEROID
#define GEODECY_SPHEROID

#pragma message "This geodecy include is experimental"

namespace geodecy
{

enum Axis
{
	North = 1,
	South,
	East,
	West,
	Up,
	Down
};

template <typename TType, TType A, TType B,
	typename TVec3,
	typename TMat3>
struct spheroid
{
public:
	using Type = TType;
	using Vec3 = TVec3;
	using Mat3 = TMat3;

	static constexpr const Type c_A{A};
	static constexpr const Type c_B{B};
	static constexpr const Type c_E2{1 - (c_B * c_B) / (c_A * c_A)};
protected:
	static constexpr Type radians(Type a_degree)
	{
		return a_degree * Type(0.01745329251994329576923690768489);
	}

	static constexpr Type degrees(Type a_radians)
	{
		return a_radians * Type(57.295779513082320876798154814105);
	}

	static constexpr Type get_x(const Vec3& a_vec3);
	static constexpr Type get_y(const Vec3& a_vec3);
	static constexpr Type get_z(const Vec3& a_vec3);
	static constexpr Vec3 to_vec(
		Type a_x, Type a_y, Type a_z);
	static constexpr Mat3 to_mat(
		Type a_x0, Type a_y0, Type a_z0,
		Type a_x1, Type a_y1, Type a_z1,
		Type a_x2, Type a_y2, Type a_z2);

public:
	struct rad
	{
		/// Convert lla to xyz coordinates in meters 
		/// @param a_latitude_rad the latitude in radians
		/// @param a_longitude_rad the longitude in radians
		/// @param a_altitude_meter the altitude in meters
		/// @return the xyz coordinate in meters
		static constexpr Vec3 lla2xyz(
			Type a_latitude_rad,
			Type a_longitude_rad,
			Type a_altitude_meter);

		/// Convert lla to xyz coordinates in meters
		/// @param a_lla the latitude, longitude in radians and altitude in meters
		/// @return the xyz coordinate in meters
		static constexpr Vec3 lla2xyz(
			const Vec3& a_lla);

		/// Convert xyz to lla
		/// @param a_x the x coordinate in meters
		/// @param a_y the y coordinate in meters
		/// @param a_z the z coordinate in meters
		/// @return the lla in radians and meters
		static constexpr Vec3 xyz2lla(
			Type a_x,
			Type a_y,
			Type a_z);

		/// Convert xyz to lla
		/// @param a_xyz the xyz coordiantes in meters
		/// @return the lla in radians and meters
		static constexpr Vec3 xyz2lla(
			const Vec3& a_xyz);

		/// Convert lla to nwu (north west up)
		/// @param a_latitude_rad the latitude in radians
		/// @param a_longitude_rad the longitude in radians
		/// @return the nwu matrix
		static constexpr Mat3 lla2nwu(
			Type a_latitude_rad,
			Type a_longitude_rad);

		template<Axis X, Axis Y, Axis Z>
		static constexpr Mat3 lla2ltp(
			Type a_latitude_rad,
			Type a_longitude_rad);
	};

	struct deg
	{
		/// Convert lla to xyz coordinates in meters 
		/// @param a_latitude_deg the latitude in degrees
		/// @param a_longitude_deg the longitude in degrees
		/// @param a_altitude_meter the altitude in meters
		/// @return the xyz coordinate in meters
		static constexpr Vec3 lla2xyz(
			Type a_latitude_deg,
			Type a_longitude_deg,
			Type a_altitude_meter);

		/// Convert lla to xyz coordinates in meters
		/// @param a_lla the latitude, longitude in degrees and altitude in meters
		/// @return the xyz coordinate in meters
		static constexpr Vec3 lla2xyz(const Vec3& a_lla);

		/// Convert xyz to lla
		/// @param a_x the x coordinate in meters
		/// @param a_y the y coordinate in meters
		/// @param a_z the z coordinate in meters
		/// @return the lla in radians and meters
		static constexpr Vec3 xyz2lla(
			Type a_x,
			Type a_y,
			Type a_z);

		/// Convert xyz to lla
		/// @param a_xyz the xyz coordiantes in meters
		/// @return the lla in degrees and meters
		static constexpr Vec3 xyz2lla(const Vec3& a_xyz);

		/// Convert lla to nwu (north west up)
		/// @param a_latitude_deg the latitude in degrees
		/// @param a_longitude_deg the longitude in degrees
		/// @return the nwu matrix
		static constexpr Mat3 lla2nwu(
			Type a_latitude_deg,
			Type a_longitude_deg);
	};
};

} // geodecy

#include "spheroid.inl"
#endif  // GEODECY_SPHEROID
