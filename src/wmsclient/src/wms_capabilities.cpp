
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

	if(result.name == "GetMap")
	{
		result.type = RequestType::Type::GetMap;
	}
	else if(result.name == "GetCapabilities")
	{
		result.type = RequestType::Type::GetCapabilities;
	}
	else if(result.name == "GetFeatureInfo")
	{
		result.type = RequestType::Type::GetFeatureInfo;
	}
	else
	{
		result.type = RequestType::Type::_ExtendedOperation;
	}

	for(pugi::xpath_node item : a_node.select_nodes("./Format"))
	{
		result.formats.push_back(item.node().text().as_string(""));
	}

	for(pugi::xpath_node item : a_node.select_nodes("./DCPType/HTTP"))
	{
		const char* get_href = item.node()
			.child("Get")
			.child("OnlineResource")
			.attribute("xlink:href")
			.as_string(nullptr);
		if(get_href != nullptr)
		{
			result.get_endpoints.push_back(get_href);
		}
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

	for(pugi::xml_node child : capability.child("Request").children())
	{
		a_capabilities.requestTypes.push_back(parse_request_type(child));
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

const RequestType* WMSCapabilities::getRequestType(
	const RequestType::Type& a_type,
	const char* a_name) const
{
	for(auto& item : requestTypes)
	{
		if(item.type == a_type && (a_name == nullptr || (item.name == a_name)))
		{
			return &item;
		}
	}

	return nullptr;
}

std::string WMSCapabilities::to_string() const
{
	std::stringstream ss;

	ss << "Version: " << version.to_string() << "\n";
	for(auto& item : requestTypes)
	{
		ss << item.to_string() << "\n\n";
	}

	return ss.str();
}

} // wms
