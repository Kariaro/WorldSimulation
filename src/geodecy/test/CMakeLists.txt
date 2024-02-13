set(TEST_NAME "gtest_wgs84")
add_executable(${TEST_NAME}
	wgs84_test.cpp
)
target_link_libraries(${TEST_NAME}
	geodecy
	gtest
	gtest_main)
set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_RUNTIME_OUTPUT_DIRECTORY}")
add_test(${TEST_NAME} ${TEST_NAME})
