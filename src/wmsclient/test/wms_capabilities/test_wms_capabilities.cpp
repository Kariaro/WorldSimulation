#include <gtest/gtest.h>

#include "wms_capabilities.hpp"

// Just make sure the version handles edge cases
TEST(test_wms_capabilities, version_fuzzing)
{
	wms::Version a(nullptr);
	wms::Version b("1.3.");
	wms::Version c("1...");
	wms::Version d("1.2.3.........");
	wms::Version e("......");
	wms::Version f("5");

	EXPECT_TRUE(a.equals(0, 0, 0));
	EXPECT_TRUE(b.equals(1, 3, 0));
	EXPECT_TRUE(c.equals(1, 0, 0));
	EXPECT_TRUE(d.equals(1, 2, 3));
	EXPECT_TRUE(e.equals(0, 0, 0));
	EXPECT_TRUE(f.equals(5, 0, 0));
}
