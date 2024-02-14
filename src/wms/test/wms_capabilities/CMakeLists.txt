set(TEST_NAME "gtest_wms_capabilities")
add_executable(${TEST_NAME}
	test_wms_capabilities.cpp
)
target_link_libraries(${TEST_NAME}
	wms_client
	gtest
	gtest_main)
set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_RUNTIME_OUTPUT_DIRECTORY}")
add_test(${TEST_NAME} ${TEST_NAME})
