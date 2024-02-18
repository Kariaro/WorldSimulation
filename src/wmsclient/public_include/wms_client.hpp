
#ifndef WMS_WMS_CLIENT
#define WMS_WMS_CLIENT

#include "wms_capabilities.hpp"

#include <vector>
#include <memory>

namespace wms
{

/// A wms client that can read resources from web map servers
class WMSClient
{
public:
	WMSClient() = default;
	~WMSClient() = default;

	const WMSCapabilities* openCapabilitiesFromFile(const std::string& a_pathname);
	const WMSCapabilities* openCapabilities(const std::string& a_url);

	std::vector<unsigned char> getImage(const WMSCapabilities* a_capabilities,
		const std::string& a_layers,
		double a_minLat,
		double a_maxLat,
		double a_minLon,
		double a_maxLon,
		int a_width=128,
		int a_height=128);

private:
	std::vector<std::unique_ptr<WMSCapabilities>> m_capabilities;
};

} // wms

#endif  // WMS_WMS_CLIENT
