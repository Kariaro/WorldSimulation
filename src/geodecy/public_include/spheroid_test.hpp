
#ifndef GEODECY_SPHEROID
#define GEODECY_SPHEROID

namespace geodecy
{

enum Axis
{
	North = 0,
	South = 1,
	East  = 2,
	West  = 3,
	Up    = 4,
	Down  = 5,
};

/// This is a fix to make sure clang can use double template values
struct db_wrp
{
public:
	constexpr db_wrp(double a_value) : value{a_value} {}
	const double value;
};

template <typename TAllocator, db_wrp A, db_wrp B>
struct spheroid
{
public:
	using Type = typename TAllocator::Type;
	using Vec3 = typename TAllocator::Vec3;
	using Mat3 = typename TAllocator::Mat3;

	static constexpr const Type c_A{A.value};
	static constexpr const Type c_B{B.value};
	static constexpr const Type c_ES{1 - (c_B * c_B) / (c_A * c_A)};
protected:
	static constexpr Type radians(Type a_degree)
	{
		return a_degree * Type(0.01745329251994329576923690768489);
	}

	static constexpr Type degrees(Type a_radians)
	{
		return a_radians * Type(57.295779513082320876798154814105);
	}

	static_assert(c_A > 0);
	static_assert(c_B > 0);

	static_assert(std::is_same_v<decltype(TAllocator::to_mat), Mat3(Type,Type,Type,Type,Type,Type,Type,Type,Type)>);
	static_assert(std::is_same_v<decltype(TAllocator::to_vec), Vec3(Type,Type,Type)>);
	static_assert(std::is_same_v<decltype(TAllocator::get_x), Type(const Vec3&)>);
	static_assert(std::is_same_v<decltype(TAllocator::get_y), Type(const Vec3&)>);
	static_assert(std::is_same_v<decltype(TAllocator::get_z), Type(const Vec3&)>);
public:
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

	/// Convert lla to ltp (local tangent plane)
	/// @tparam X the direction of the X axis
	/// @tparam Y the direction of the Y axis
	/// @tparam Z the direction of the Z axis
	/// @param a_latitude_rad the latitude in radians
	/// @param a_longitude_rad the longitude in radians
	/// @return the ltp matrix
	template<Axis X, Axis Y, Axis Z>
	static constexpr Mat3 lla2ltp(
		Type a_latitude_rad,
		Type a_longitude_rad);

	/// Convert lla to nwu (north west up)
	/// @param a_latitude_rad the latitude in radians
	/// @param a_longitude_rad the longitude in radians
	/// @return the matrix
	static constexpr auto lla2nwu = lla2ltp<Axis::North, Axis::West, Axis::Up>;
	
	/// Convert lla to ned (north east down)
	/// @param a_latitude_rad the latitude in radians
	/// @param a_longitude_rad the longitude in radians
	/// @return the matrix
	static constexpr auto lla2ned = lla2ltp<Axis::North, Axis::East, Axis::Down>;

	/// Convert lla to enu (east north up)
	/// @param a_latitude_rad the latitude in radians
	/// @param a_longitude_rad the longitude in radians
	/// @return the matrix
	static constexpr auto lla2enu = lla2ltp<Axis::East, Axis::North, Axis::Up>;
};

} // geodecy

/// Implementation of spheroid
namespace geodecy
{

template <typename TAllocator, db_wrp A, db_wrp B>
constexpr typename TAllocator::Vec3 spheroid<TAllocator, A, B>::lla2xyz(
	Type a_latitude_rad,
	Type a_longitude_rad,
	Type a_altitude_meter)
{
	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);
	const double N = c_A / std::sqrt(1 - c_ES * s_lat * s_lat);
	const double x = (N + a_altitude_meter) * c_lat * c_lon;
	const double y = (N + a_altitude_meter) * c_lat * s_lon;
	const double z = ((1 - c_ES) * N + a_altitude_meter) * s_lat;
	return TAllocator::to_vec(Type(x), Type(y), Type(z));
}

template <typename TAllocator, db_wrp A, db_wrp B>
constexpr typename TAllocator::Vec3 spheroid<TAllocator, A, B>::lla2xyz(
	const Vec3& a_lla)
{
	return rad::lla2xyz(
		TAllocator::get_x(a_lla),
		TAllocator::get_y(a_lla),
		TAllocator::get_z(a_lla));
}

// TODO: Use a known approximation
template <typename TAllocator, db_wrp A, db_wrp B>
constexpr typename TAllocator::Vec3 spheroid<TAllocator, A, B>::xyz2lla(
	Type a_x,
	Type a_y,
	Type a_z)
{
	const double x = a_x;
	const double y = a_y;
	const double z = a_z;

	const double rp = std::sqrt(x*x + y*y + z*z);
	const double p = std::sqrt(x*x + y*y);

	const double lon_rad = (std::abs(x) + std::abs(y) < 1.0e-10)
		? 0.0
		: std::atan2(y, x);

	if(p < 1.0e-10)
	{
		double lat_rad   = radians(z < 0.0 ? -90 : 90.0);
		double alt_meter = rp - c_B;
		return TAllocator::to_vec(Type(lat_rad), Type(lon_rad), Type(alt_meter));
	}

	const double flatgc_rad = std::asin(z / rp);
	const double slat_0	 = std::sin(flatgc_rad);
	const double first_rn   = c_A / std::sqrt(1.0 - c_ES * slat_0 * slat_0);

	constexpr const double c_B4_A4 = (1.0 - c_ES) * (1.0 - c_ES) - 1;
	double alt_meter = rp - first_rn * std::sqrt(1 + c_B4_A4 * slat_0 * slat_0);
	double lat_rad{};
	{
		const double flatgd_rad = std::atan(std::tan(flatgc_rad) / (1 - c_ES * first_rn / (first_rn + alt_meter)));
		const double slat_1 = std::sin(flatgd_rad);
		const double rn = c_A / std::sqrt(1.0 - c_ES * slat_1 * slat_1);
		lat_rad = std::atan(std::tan(flatgc_rad) / (1 - c_ES * rn / (rn + alt_meter)));
	}

	double slat = std::sin(lat_rad);
	double rn   = c_A / std::sqrt(1.0 - c_ES * slat * slat);

	for(int i = 0; i < 5; i++)
	{
		double lat_new_rad = std::atan((z + rn * c_ES * slat) / p);

		slat	  = std::sin(lat_new_rad);
		rn		= c_A / std::sqrt(1.0 - c_ES * slat * slat);
		alt_meter = (p / std::cos(lat_new_rad)) - rn; // Not needed in loop could be removed

		double dlat_rad = lat_new_rad - lat_rad;
		lat_rad = lat_new_rad;
		if(std::abs(dlat_rad) < 1.0e-14)
		{
			break;
		}
	}

	return TAllocator::to_vec(Type(lat_rad), Type(lon_rad), Type(alt_meter));
}

template <typename TAllocator, db_wrp A, db_wrp B>
constexpr typename TAllocator::Vec3 spheroid<TAllocator, A, B>::xyz2lla(
	const Vec3& a_xyz)
{
	return rad::xyz2lla(
		TAllocator::get_x(a_xyz),
		TAllocator::get_y(a_xyz),
		TAllocator::get_z(a_xyz));
}

template <typename TAllocator, db_wrp A, db_wrp B>
template<Axis X, Axis Y, Axis Z>
constexpr typename TAllocator::Mat3 spheroid<TAllocator, A, B>::lla2ltp(
	Type a_latitude_rad,
	Type a_longitude_rad)
{
	static_assert((X >> 1) != (Y >> 1) && (X >> 1) != (Z >> 1) && (Y >> 1) != (Z >> 1), "All axis needs to be unique");
	static_assert(X >= 0 && X < 6, "Invalid X axis");
	static_assert(Y >= 0 && Y < 6, "Invalid Y axis");
	static_assert(Z >= 0 && Z < 6, "Invalid Z axis");

	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);

	const double x0 = -s_lat * c_lon;
	const double y0 = -s_lat * s_lon;
	const double z0 = c_lat;

	const double x1 = -s_lon;
	const double y1 = c_lon;
	const double z1 = 0.0;

	const double x2 = c_lat * c_lon;
	const double y2 = c_lat * s_lon;
	const double z2 = s_lat;

	struct { double x, y, z; } values[6] {
		{  x0,  y0,  z0 }, // North
		{ -x0, -y0, -z0 }, // South
		{  x1,  y1,  z1 }, // East
		{ -x1, -y1, -z1 }, // West
		{  x2,  y2,  z2 }, // Up
		{ -x2, -y2, -z2 }, // Down
	};

	auto [vx0, vy0, vz0] = values[X];
	auto [vx1, vy1, vz1] = values[Y];
	auto [vx2, vy2, vz2] = values[Z];

	return TAllocator::to_mat(
		Type(vx0), Type(vy0), Type(vz0),
		Type(vx1), Type(vy1), Type(vz1),
		Type(vx2), Type(vy2), Type(vz2));
}

} // geodecy

#endif  // GEODECY_SPHEROID
