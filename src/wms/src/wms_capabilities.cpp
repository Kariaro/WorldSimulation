
#include "wms_capabilities.hpp"

#include <pugixml.hpp>
#include <iostream>

namespace wms
{

enum class code
{
	success = 0,
	top_level_element_not_found,
	top_level_element_version_not_found,
};

RequestType parse_request_type(pugi::xml_node a_node)
{
	RequestType result{};
	result.name = a_node.name();

	std::cout << "Parsing request type: '" << result.name << "'" << std::endl;

	for(pugi::xpath_node item : a_node.select_nodes("./Format"))
	{
		result.formats.push_back(item.node().text().as_string(""));
	}

	for(pugi::xpath_node item : a_node.select_nodes("./DCPType/HTTP"))
	{
		pugi::xml_node get_node = item.node().child("Get");
		const char* get_href = get_node.child("OnlineResource").attribute("xlink:href").as_string(nullptr);
		if(get_href != nullptr)
		{
			result.get_endpoints.push_back(get_href);
		}

		// Right now we do not care of post endpoints
		/*
		pugi::xml_node post_node = item.node().child("Post");
		const char* post_href = post_node.child("OnlineResource").attribute("xlink:href").as_string(nullptr);
		if(post_href != nullptr)
		{
			result.endpoints.push_back({
				post_href,
				RequestType::Endpoint::Type::Post
			});
		}
		*/
	}

	return result;
}

code try_parse(WMSCapabilities& a_capabilities, pugi::xml_node a_root)
{
	// Get the wms capabilities node
	pugi::xml_node capabilties = a_root.child("WMS_Capabilities");
	if(capabilties.type() == pugi::xml_node_type::node_null)
	{
		return code::top_level_element_not_found;
	}

	const char* version_text = capabilties.attribute("version").as_string(nullptr);
	a_capabilities.version = Version(version_text);

	// Not needed right now
	pugi::xml_node service = capabilties.child("Service");
	(void) service;

	pugi::xml_node capability = capabilties.child("Capability");
	(void) capability;

	pugi::xml_node request = capability.child("Request");
	(void) request;

	std::cout << "Version: " << a_capabilities.version.to_string() << std::endl;

	for(pugi::xml_node child : request.children())
	{
		auto result = parse_request_type(child);

		std::cout << "Request:\n" << result.to_string() << std::endl;
	}

	return code::success;
}

WMSCapabilities::WMSCapabilities(const std::string& a_xml)
{
	pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(a_xml.c_str());
    if(!result)
	{
		// Faileed to load wms capabilities
		return;
	}

	try_parse(*this, doc.root());
}

} // wms
