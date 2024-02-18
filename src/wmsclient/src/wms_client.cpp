
#include "wms_client.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <algorithm>

#include <httplib.h>

namespace wms
{

const WMSCapabilities* WMSClient::openCapabilitiesFromFile(const std::string& a_pathname)
{
	std::ifstream file(a_pathname.c_str());	
	if(!file.is_open())
	{
		return nullptr;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return m_capabilities.emplace_back(std::make_unique<WMSCapabilities>(buffer.str())).get();
}

httplib::Result requestData(const std::string& a_url)
{
	// Split url
	size_t requestIdx = a_url.find("/", a_url.find("://") + 3);
	std::string url;
	std::string requestString;
	if(requestIdx == std::string::npos)
	{
		url = a_url;
		requestString = "";
	}
	else
	{
		url = a_url.substr(0, requestIdx);
		requestString = a_url.substr(requestIdx);
	}

	// std::cout << "[" << url << "]" << std::endl;
	// std::cout << "[" << requestString << "]" << std::endl;
	// std::cout << requestIdx << std::endl;
	// std::cout << a_url << std::endl;

	httplib::Client client(url);
	client.set_follow_location(true);
	client.set_read_timeout(std::chrono::milliseconds(10'000));
	auto res = client.Get(requestString, {{ "User-Agent", "harucoded-world-simulation/0.0.0" }});
	client.stop();

	return res;
}

const WMSCapabilities* WMSClient::openCapabilities(const std::string& a_url, const std::string& a_version)
{
	std::string version = a_version.empty() ? "" : ("&VERSION=" + a_version);

	// Try find version
	std::string url = a_url + "?SERVICE=WMS&REQUEST=GetCapabilities" + version;
	auto res = requestData(url);

	// std::cout << "From URL [" << url << "]" << std::endl;
	// std::cout << "Status: [" << res->status << "]" << std::endl;
	// std::cout << "Headers:" << std::endl;
	// for(auto& [key, value] : res->headers)
	// {
	// 	std::cout << "  - [" << key << "] = [" << value << "]" << std::endl;
	// }
	// std::cout << "Body: [" << std::endl << res->body << std::endl << "]" << std::endl;

	return m_capabilities.emplace_back(std::make_unique<WMSCapabilities>(res->body)).get();
}

std::vector<unsigned char> WMSClient::getImage(const WMSCapabilities* a_capabilities,
	const std::string& a_layers,
	double a_minLat,
	double a_maxLat,
	double a_minLon,
	double a_maxLon,
	int a_width,
	int a_height)
{
	const RequestType* requestType = a_capabilities->getRequestType(RequestType::Type::GetMap);
	if(requestType == nullptr)
	{
		std::cout << "Could not find GetMap request type" << std::endl;
		// Empty buffer
		return {};
	}

	std::vector<std::string> formats { "image/png", "image/jpeg" };
	std::string format;
	for(auto& lookup_format : formats)
	{
		if(requestType->supportsFormat(lookup_format))
		{
			format = lookup_format;
			break;
		}
	}

	if(format.empty())
	{
		std::cout << "Could not find supported format" << std::endl;
		// No supported format
		return {};
	}

	const std::string version = a_capabilities->version.to_string();
	const std::string url =
		requestType->get_endpoints.front() +
		"SERVICE=WMS"
		"&REQUEST=GetMap"
		"&VERSION=" + version +
		"&height=" + std::to_string(a_height) +
		"&width=" + std::to_string(a_width) +
		"&format=" + format +
		"&styles=" // TODO
		"&crs=EPSG:4326" // TODO (SRS for < 1.3.0)
		"&bbox="
		+ std::to_string(a_minLat) + ","
		+ std::to_string(a_minLon) + ","
		+ std::to_string(a_maxLat) + ","
		+ std::to_string(a_maxLon) +
		"&layers=" + a_layers;
	auto res = requestData(url);

	std::cout << url << std::endl;

	std::vector<unsigned char> buffer;
	buffer.reserve(res->body.size());
	buffer.assign(res->body.begin(), res->body.end());
	return buffer;
}


}
