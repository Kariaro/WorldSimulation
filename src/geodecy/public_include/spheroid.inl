/// Implementation of spheroid

namespace geodecy
{

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::rad::lla2xyz(
	Type a_latitude_rad,
	Type a_longitude_rad,
	Type a_altitude_meter)
{
	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);
	const double N = c_A / std::sqrt(1 - c_E2 * s_lat * s_lat);
	const double x = (N + a_altitude_meter) * c_lat * c_lon;
	const double y = (N + a_altitude_meter) * c_lat * s_lon;
	const double z = ((1 - c_E2) * N + a_altitude_meter) * s_lat;
	return to_vec(Type(x), Type(y), Type(z));
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::rad::lla2xyz(
	const Vec3& a_lla)
{
	return rad::lla2xyz(get_x(a_lla), get_y(a_lla), get_z(a_lla));
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::rad::xyz2lla(
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

	// on pole special case
	if(p < 1.0e-10)
	{
		double lat_rad   = radians(z < 0.0 ? -90 : 90.0);
		double alt_meter = rp - c_B;
		return to_vec(Type(lat_rad), Type(lon_rad), Type(alt_meter));
	}

	const double flatgc_rad = std::asin(z / rp);
	const double slat_0     = std::sin(flatgc_rad);
	const double first_rn   = c_A / std::sqrt(1.0 - c_E2 * slat_0 * slat_0);

	constexpr const double c_B4_A4 = (1.0 - c_E2) * (1.0 - c_E2) - 1;

	// first iteration, use flatgc to get altitude
	// and alt needed to convert gc to gd lat
	double alt_meter = rp - first_rn * std::sqrt(1 + c_B4_A4 * slat_0 * slat_0);
	double lat_rad;
	{
		// approximation by stages 1st use gc-lat as if is gd, then correct alt dependence
		const double flatgd_rad = std::atan(std::tan(flatgc_rad) / (1 - c_E2 * first_rn / (first_rn + alt_meter)));

		// now use this approximation for gd-lat to get rn etc
		const double slat_1 = std::sin(flatgd_rad);
		const double rn = c_A / std::sqrt(1.0 - c_E2 * slat_1 * slat_1);
		lat_rad = std::atan(std::tan(flatgc_rad) / (1 - c_E2 * rn / (rn + alt_meter)));
	}

	double slat = std::sin(lat_rad);
	double rn   = c_A / std::sqrt(1.0 - c_E2 * slat * slat);

	for(int i = 0; i < 5; i++)
	{
		double lat_new_rad = std::atan((z + rn * c_E2 * slat) / p);

		slat      = std::sin(lat_new_rad);
		rn        = c_A / std::sqrt(1.0 - c_E2 * slat * slat);
		alt_meter = (p / std::cos(lat_new_rad)) - rn; // Not needed in loop could be removed

		double dlat_rad = lat_new_rad - lat_rad;
		lat_rad = lat_new_rad;
		if(std::abs(dlat_rad) < 1.0e-14)
		{
			break;
		}
	}

	return to_vec(Type(lat_rad), Type(lon_rad), Type(alt_meter));
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::rad::xyz2lla(
	const Vec3& a_xyz)
{
	return rad::xyz2lla(get_x(a_xyz), get_y(a_xyz), get_z(a_xyz));
}


template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TMat3 spheroid<TType, A, B, TVec3, TMat3>::rad::lla2nwu(
	Type a_latitude_rad,
	Type a_longitude_rad)
{
	const double c_lat = std::cos(a_latitude_rad);
	const double s_lat = std::sin(a_latitude_rad);
	const double c_lon = std::cos(a_longitude_rad);
	const double s_lon = std::sin(a_longitude_rad);

	return to_mat(
		Type(-s_lat * c_lon), Type(-s_lat * s_lon), Type(c_lat),
		Type( s_lon        ), Type(-c_lon        ), Type(  0.0),
		Type( c_lat * c_lon), Type( c_lat * s_lon), Type(s_lat)
	);
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::deg::lla2xyz(
	Type a_latitude_deg,
	Type a_longitude_deg,
	Type a_altitude_meter)
{
	return rad::lla2xyz(
		radians(a_latitude_deg),
		radians(a_longitude_deg),
		a_altitude_meter);
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::deg::lla2xyz(
	const Vec3& a_lla)
{
	return deg::lla2xyz(get_x(a_lla), get_y(a_lla), get_z(a_lla));
}


template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::deg::xyz2lla(
	Type a_x,
	Type a_y,
	Type a_z)
{
	const Vec3 value = rad::xyz2lla(a_x, a_y, a_z);
	return to_vec(
		degrees(get_x(value)),
		degrees(get_y(value)),
		get_z(value)
	);
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TVec3 spheroid<TType, A, B, TVec3, TMat3>::deg::xyz2lla(
	const Vec3& a_xyz)
{
	return deg::xyz2lla(get_x(a_xyz), get_y(a_xyz), get_z(a_xyz));
}


template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
constexpr TMat3 spheroid<TType, A, B, TVec3, TMat3>::deg::lla2nwu(
	Type a_latitude_deg,
	Type a_longitude_deg)
{
	return rad::lla2nwu(
		radians(a_latitude_deg),
		radians(a_longitude_deg)
	);
}

template <Axis V>
static constexpr void ltp_get_axis(
	double a_x0, double a_y0, double a_z0,
	double a_x1, double a_y1, double a_z1,
	double a_x2, double a_y2, double a_z2,
	double& r_x, double& r_y, double& r_z)
{
	if constexpr (V == Axis::North)
	{
		r_x = a_x0;
		r_y = a_y0;
		r_z = a_z0;
	}
	else if constexpr (V == Axis::South)
	{
		r_x = -a_x0;
		r_y = -a_y0;
		r_z = -a_z0;
	}

	else if constexpr (V == Axis::East)
	{
		r_x = a_x1;
		r_y = a_y1;
		r_z = a_z1;
	}
	else if constexpr (V == Axis::West)
	{
		r_x = -a_x1;
		r_y = -a_y1;
		r_z = -a_z1;
	}

	else if constexpr (V == Axis::Up)
	{
		r_x = a_x2;
		r_y = a_y2;
		r_z = a_z2;
	}
	else if constexpr (V == Axis::Down)
	{
		r_x = -a_x2;
		r_y = -a_y2;
		r_z = -a_z2;
	}
	else
	{
		static_assert(false, "Invalid axis");
	}
}

template <typename TType, TType A, TType B, typename TVec3, typename TMat3>
template<Axis X, Axis Y, Axis Z>
constexpr TMat3 spheroid<TType, A, B, TVec3, TMat3>::rad::lla2ltp(
	Type a_latitude_rad,
	Type a_longitude_rad)
{
	static_assert(X != Y && X != Z && Y != Z, "All axis needs to be unique");

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

	double vx0, vy0, vz0;
	double vx1, vy1, vz1;
	double vx2, vy2, vz2;

	ltp_get_axis<X>(x0, y0, z0, x1, y1, z1, x2, y2, z2, vx0, vy0, vz0);
	ltp_get_axis<Y>(x0, y0, z0, x1, y1, z1, x2, y2, z2, vx1, vy1, vz1);
	ltp_get_axis<Z>(x0, y0, z0, x1, y1, z1, x2, y2, z2, vx2, vy2, vz2);

	return to_mat(
		Type(vx0), Type(vy0), TType(vz0),
		Type(vx1), Type(vy1), TType(vz1),
		Type(vx2), Type(vy2), TType(vz2)
	);
}


} // geodecy
