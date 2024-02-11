#include <gtest/gtest.h>

#include <tuple>

#include <glm/gtx/string_cast.hpp>

#include <random>

#include "wgs84.h"

/// Returns the X, Y and Z axis of the matrix
std::tuple<glm::dvec3, glm::dvec3, glm::dvec3> getAxis(const glm::dmat3& a_matrix)
{
	return std::make_tuple(
		a_matrix * glm::dvec3(1, 0, 0),
		a_matrix * glm::dvec3(0, 1, 0),
		a_matrix * glm::dvec3(0, 0, 1)
	);
}

TEST(wgs84_test, lla2nwu)
{
	{
		// We are currently at latitude, longitude (0 deg, 0 deg)
		auto [forward, right, up] = getAxis(wgs84::lla2nwu_deg(0, 0));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}

	{
		// We are currently at latitude, longitude (0 deg, 90 deg)
		auto [forward, right, up] = getAxis(wgs84::lla2nwu_deg(0, 90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 1, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}

	{
		// We are currently at latitude, longitude (0 deg, 180 deg)
		auto [forward, right, up] = getAxis(wgs84::lla2nwu_deg(0, 180));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, 1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(-1, 0, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}

	{
		// We are currently at latitude, longitude (90 deg, 0 deg)
		auto [forward, right, up] = getAxis(wgs84::lla2nwu_deg(90, 0));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(-1, 0, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
}


TEST(wgs84_test, lla2nwu_rpy)
{
	{
		auto [forward, right, up] = getAxis(
			wgs84::lla2nwu_deg(0, 0) *
			wgs84::rpy(0, 0, -90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, 1, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
	{
		auto [forward, right, up] = getAxis(
			wgs84::lla2nwu_deg(0, 0) *
			wgs84::rpy(0, -90, 0));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 0, -1)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
	{
		auto [forward, right, up] = getAxis(
			wgs84::lla2nwu_deg(0, 0) *
			wgs84::rpy(0, -90, -90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
	{
		auto [forward, right, up] = getAxis(
			wgs84::lla2nwu_deg(0, 0) *
			wgs84::rpy(-90, -90, -90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, 1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
}

TEST(wgs84_test, rpy)
{
	GTEST_SKIP();

	{
		auto [forward, right, up] = getAxis(wgs84::rpy(0, 0, 0));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(0, 1, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}

	{
		auto [forward, right, up] = getAxis(wgs84::rpy(0, 0, -90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(1, 0, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}

	{
		auto [forward, right, up] = getAxis(wgs84::rpy(0, -90, -90));

		EXPECT_NEAR(glm::length(forward - glm::dvec3(0, 0, 1)), 0, 0.001)
			<< "Forward vector is wrong: " << glm::to_string(forward);
		EXPECT_NEAR(glm::length(right - glm::dvec3(-1, 0, 0)), 0, 0.001)
			<< "Right vector is wrong: " << glm::to_string(right);
		EXPECT_NEAR(glm::length(up - glm::dvec3(0, -1, 0)), 0, 0.001)
			<< "Up vector is wrong: " << glm::to_string(up);
	}
}


TEST(wgs84_test, ecef2lla)
{
	std::minstd_rand random{};
	std::cout << random.min() << "," << random.max() << std::endl;
	
	for(int i = 0; i < 100; i++)
	{
		double v0 = (random() - random.min()) / static_cast<double>(random.max() - random.min());
		double v1 = (random() - random.min()) / static_cast<double>(random.max() - random.min());
		double v2 = (random() - random.min()) / static_cast<double>(random.max() - random.min());

		double lat_deg = v0 * 180.0 -  90.0;
		double lon_deg = v1 * 360.0 - 180.0;
		double alt_met = v2 * 100'000.0;

		auto ecef = wgs84::lla2ecef_deg(lat_deg, lon_deg, alt_met);
		auto lla = wgs84::ecef2lla_deg(ecef);

		// lla.x = std::fmod(lla.x + 720, 180.0);
		// lla.y = std::fmod(lla.y + 720, 360.0);

		EXPECT_NEAR(lat_deg, lla.x, 0.001);
		EXPECT_NEAR(lon_deg, lla.y, 0.001);
		EXPECT_NEAR(alt_met, lla.z, 0.001);

		std::printf("==========================================\n");
		std::printf("original lat: %.8f, lon: %.8f, alt: %.8f\n", lat_deg, lon_deg, alt_met);
		std::printf("back     lat: %.8f, lon: %.8f, alt: %.8f\n", lla.x, lla.y, lla.z);
		std::printf("diff     lat: %.24f, lon: %.24f, alt: %.24f\n", lla.x - lat_deg, lla.y - lon_deg, lla.z - alt_met);
	}
}