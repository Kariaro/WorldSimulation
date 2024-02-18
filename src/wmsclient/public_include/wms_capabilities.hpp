
#ifndef WMS_WMS_CAPABILITIES
#define WMS_WMS_CAPABILITIES

#include <cstring>
#include <string>
#include <vector>

#include <sstream>

namespace wms
{

struct Version
{
	std::string name;
	int major;
	int minor;
	int patch;

	Version()
	{
		major = 0;
		minor = 0;
		patch = 0;
		name = "0.0.0";
	}

	explicit Version(const char* a_text)
	{
		if(a_text == nullptr)
		{
			major = 0;
			minor = 0;
			patch = 0;
		}
		else
		{
			char* dup = strdup(a_text);
			char* word = nullptr;
			word = strtok(dup, ".");
			major = atoi(word);

			word = strtok(nullptr, ".");
			minor = atoi(word);

			word = strtok(nullptr, ".");
			patch = atoi(word);
			free(dup);
		}

		name = 
			std::to_string(major) + "." +
			std::to_string(minor) + "." +
			std::to_string(patch);
	};

	bool equals(int a_major, int a_minor, int a_patch) const
	{
		return major == a_major && minor == a_minor && patch == a_patch;
	}

	const std::string& to_string() const
	{
		return name;
	}
};

enum class CRS
{
	EPSG_4326   = 1, // WGS 84
	EPSG_3857   = 2, // Web Mercator
	EPSG_900913 = 2, // Web Mercator
};

// struct OGC_WMS_

struct BoundingBox
{
	/// The crs of the bounding box
	std::string crs;
	double values[4];
};

struct datatype_Box
{
	std::string CRS;
	double MaxX;
	double MaxY;
	double MinX;
	double MinY;
};

/// A connection type is a collection of endpoint connections
struct RequestType
{
	/// The endpoint of the request
	std::string name;

	/// The type of this endpoint
	enum class Type
	{
		GetCapabilities,
		GetMap,
		GetFeatureInfo,
		_ExtendedOperation
	} type;

	/// The formats the endpoint supports
	std::vector<std::string> formats;

	/// Get endpoints
	std::vector<std::string> get_endpoints;

	bool supportsFormat(const std::string& a_format) const
	{
		// TODO: use std::find
		for(auto& item : formats)
		{
			if(item == a_format)
			{
				return true;
			}
		}
		return false;
	}

	/// @return a temporary to_string method
	std::string to_string() const
	{
		std::stringstream ss;
		ss << "request: [" << name << "]\n";
		for(auto& format : formats)
		{
			ss << "  - format: [" << format << "]\n";
		}

		for(auto& endpoint : get_endpoints)
		{
			ss << "  - (Get) endpoint: [" << endpoint << "]\n";
		}

		return ss.str();
	}
};

struct WMSLayer
{
	/// Name of this wms layer
	std::string name;

	/// Title of this wms layer
	std::string title;
};

struct WMSCapabilities
{
	/// The version of the wms capabilities
	Version version;

	/// Request types (GetCapabilities, GetMap, ...)
	std::vector<RequestType> requestTypes;

	/// Layers
	std::vector<WMSLayer> layers;

	WMSCapabilities() = default;
	WMSCapabilities(const std::string& a_xml);

	const RequestType* getRequestType(
		const RequestType::Type& a_type,
		const char* a_name = nullptr) const;

	std::string to_string() const;
};

} // wms

#endif  // WMS_WMS_CAPABILITIES
