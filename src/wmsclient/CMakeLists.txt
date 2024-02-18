add_library(wms_client)
target_sources(wms_client
	PUBLIC src/wms_client.cpp
	PUBLIC src/wms_capabilities.cpp
	)
target_include_directories(wms_client
    PUBLIC public_include
	)
target_link_libraries(wms_client
	PUBLIC pugixml
	PUBLIC httplib
	)

add_subdirectory(test/wms_capabilities)
