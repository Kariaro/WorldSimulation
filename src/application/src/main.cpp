#include <iostream>
#include <cstdio>
#include <string_view>
#include <string>
#include <chrono>

#include <sstream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "registry.hpp"

#include "worldrender.h"

#include "wms_capabilities.hpp"

struct transform
{
	glm::vec3 position;
	glm::quat rotation;
	std::string name;
};

struct a_device {};
struct b_device {};


int main(int argc, char** argv) {
	{
		std::ifstream nasa_capabilities("assets/wms.cgi.xml");
		std::stringstream buffer;
		buffer << nasa_capabilities.rdbuf();
		nasa_capabilities.close();
	
		std::string file_content = buffer.str();
		wms::WMSCapabilities capabilities(file_content);
	}
	/*
	wms::WMSCapabilities capabilities(
R"capabilities(
<?xml version="1.0" ?>
<WMS_Capabilities xmlns="http://www.opengis.net/wms" xmlns:sld="http://www.opengis.net/sld" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.3.0" xsi:schemaLocation="http://www.opengis.net/wms http://schemas.opengis.net/wms/1.3.0/capabilities_1_3_0.xsd http://www.opengis.net/sld http://schemas.opengis.net/sld/1.1.0/sld_capabilities.xsd">
<Service>
  <Name>WMS</Name>
  <Title>OpenStreetMap WMS</Title>
  <Abstract>OpenStreetMap WMS, bereitgestellt durch terrestris GmbH und Co. KG. Beschleunigt mit MapProxy (http://mapproxy.org/)</Abstract>
  <OnlineResource xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="http://www.terrestris.de"/>
  <ContactInformation>
      <ContactPersonPrimary>
        <ContactPerson>Johannes Weskamm</ContactPerson>
        <ContactOrganization>terrestris GmbH und Co. KG</ContactOrganization>
      </ContactPersonPrimary>
      <ContactPosition>Technical Director</ContactPosition>
      <ContactAddress>
        <AddressType>postal</AddressType>
        <Address>Kölnstr. 99</Address>
        <City>Bonn</City>
        <StateOrProvince></StateOrProvince>
        <PostCode>53111</PostCode>
        <Country>Germany</Country>
      </ContactAddress>
      <ContactVoiceTelephone>+49(0)228 962 899 51</ContactVoiceTelephone>
      <ContactFacsimileTelephone>+49(0)228 962 899 57</ContactFacsimileTelephone>
      <ContactElectronicMailAddress>info@terrestris.de</ContactElectronicMailAddress>
  </ContactInformation>
    <Fees>None</Fees>
    <AccessConstraints>(c) OpenStreetMap contributors (http://www.openstreetmap.org/copyright) (c) OpenStreetMap Data (http://openstreetmapdata.com) (c) Natural Earth Data (http://www.naturalearthdata.com) (c) GEBCO Compilation Group (2021) GEBCO 2021 Grid (doi:10.5285/c6612cbe-50b3-0cff-e053-6c86abc09f8f) (c) SRTM 450m by ViewfinderPanoramas (http://viewfinderpanoramas.org/) (c) Great Lakes Bathymetry by NGDC (http://www.ngdc.noaa.gov/mgg/greatlakes/) (c) SRTM 30m by NASA EOSDIS Land Processes Distributed Active Archive Center (LP DAAC, https://lpdaac.usgs.gov/) by using this service you agree to the privacy policy mentioned at https://www.terrestris.de/en/datenschutzerklaerung/</AccessConstraints>
</Service>
<Capability>
  <Request>
    <GetCapabilities>
      <Format>text/xml</Format>
      <DCPType>
        <HTTP>
          <Get><OnlineResource xlink:href="https://ows.terrestris.de/osm/service?"/></Get>
        </HTTP>
      </DCPType>
    </GetCapabilities>
    <GetMap>
      <Format>image/jpeg</Format>
      <Format>image/png</Format>
      <DCPType>
        <HTTP>
          <Get><OnlineResource xlink:href="https://ows.terrestris.de/osm/service?"/></Get>
        </HTTP>
      </DCPType>
    </GetMap>
    <GetFeatureInfo>
      <Format>text/plain</Format>
      <Format>text/html</Format>
      <Format>text/xml</Format>
      <DCPType>
        <HTTP>
          <Get><OnlineResource xlink:href="https://ows.terrestris.de/osm/service?"/></Get>
        </HTTP>
      </DCPType>
    </GetFeatureInfo>
    <sld:GetLegendGraphic>
      <Format>image/jpeg</Format>
      <Format>image/png</Format>
      <DCPType>
        <HTTP>
          <Get><OnlineResource xlink:href="https://ows.terrestris.de/osm/service?"/></Get>
        </HTTP>
      </DCPType>
    </sld:GetLegendGraphic>
  </Request>
  <Exception>
    <Format>XML</Format>
    <Format>INIMAGE</Format>
    <Format>BLANK</Format>
  </Exception>
  <Layer queryable="1">
    <Title>OpenStreetMap WMS</Title>
    <CRS>EPSG:900913</CRS>
    <CRS>EPSG:3857</CRS>
    <CRS>EPSG:25832</CRS>
    <CRS>EPSG:25833</CRS>
    <CRS>EPSG:29192</CRS>
    <CRS>EPSG:29193</CRS>
    <CRS>EPSG:31466</CRS>
    <CRS>EPSG:31467</CRS>
    <CRS>EPSG:31468</CRS>
    <CRS>EPSG:32648</CRS>
    <CRS>EPSG:4326</CRS>
    <CRS>EPSG:4674</CRS>
    <CRS>EPSG:3068</CRS>
    <CRS>EPSG:3034</CRS>
    <CRS>EPSG:3035</CRS>
    <CRS>EPSG:2100</CRS>
    <CRS>EPSG:31463</CRS>
    <CRS>EPSG:4258</CRS>
    <CRS>EPSG:4839</CRS>
    <CRS>EPSG:2180</CRS>
    <CRS>EPSG:21781</CRS>
    <CRS>EPSG:2056</CRS>
    <CRS>EPSG:4647</CRS>
    <CRS>EPSG:4686</CRS>
    <CRS>EPSG:5243</CRS>
    <EX_GeographicBoundingBox>
      <westBoundLongitude>-180</westBoundLongitude>
      <eastBoundLongitude>180</eastBoundLongitude>
      <southBoundLatitude>-89.999999</southBoundLatitude>
      <northBoundLatitude>89.999999</northBoundLatitude>
    </EX_GeographicBoundingBox>
    <BoundingBox CRS="CRS:84" minx="-180" miny="-89.999999" maxx="180" maxy="89.999999" />
    <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-147730762.67" maxx="20037508.3428" maxy="147730758.195" />
    <BoundingBox CRS="EPSG:4326" minx="-90.0" miny="-180.0" maxx="90.0" maxy="180.0" />
    <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-147730762.67" maxx="20037508.3428" maxy="147730758.195" />
    <Layer queryable="1">
      <Name>OSM-WMS</Name>
      <Title>OpenStreetMap WMS - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-88</southBoundLatitude>
        <northBoundLatitude>88</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-88" maxx="180" maxy="88" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <BoundingBox CRS="EPSG:4326" minx="-88" miny="-180" maxx="88" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="344">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=OSM-WMS&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer queryable="1">
      <Name>OSM-WMS-no-labels</Name>
      <Title>OSM WMS w/o labels - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-88</southBoundLatitude>
        <northBoundLatitude>88</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-88" maxx="180" maxy="88" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <BoundingBox CRS="EPSG:4326" minx="-88" miny="-180" maxx="88" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
    </Layer>
    <Layer queryable="1">
      <Name>OSM-Overlay-WMS</Name>
      <Title>OSM Overlay WMS - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-88</southBoundLatitude>
        <northBoundLatitude>88</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-88" maxx="180" maxy="88" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <BoundingBox CRS="EPSG:4326" minx="-88" miny="-180" maxx="88" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="160">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=OSM-Overlay-WMS&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer queryable="1">
      <Name>TOPO-WMS</Name>
      <Title>Topographic WMS - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-88</southBoundLatitude>
        <northBoundLatitude>88</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-88" maxx="180" maxy="88" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <BoundingBox CRS="EPSG:4326" minx="-88" miny="-180" maxx="88" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="241">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=TOPO-WMS&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer queryable="1">
      <Name>TOPO-OSM-WMS</Name>
      <Title>Topographic OSM WMS - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-88</southBoundLatitude>
        <northBoundLatitude>88</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-88" maxx="180" maxy="88" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <BoundingBox CRS="EPSG:4326" minx="-88" miny="-180" maxx="88" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-25819498.5135" maxx="20037508.3428" maxy="25819498.5135" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="394">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=TOPO-OSM-WMS&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer>
      <Name>SRTM30-Hillshade</Name>
      <Title>SRTM30 Hillshade - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-56</southBoundLatitude>
        <northBoundLatitude>60</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-56" maxx="180" maxy="60" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <BoundingBox CRS="EPSG:4326" minx="-56" miny="-180" maxx="60" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
    </Layer>
    <Layer>
      <Name>SRTM30-Colored</Name>
      <Title>SRTM30 Colored - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-56</southBoundLatitude>
        <northBoundLatitude>60</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-56" maxx="180" maxy="60" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <BoundingBox CRS="EPSG:4326" minx="-56" miny="-180" maxx="60" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="241">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=SRTM30-Colored&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer>
      <Name>SRTM30-Colored-Hillshade</Name>
      <Title>SRTM30 Colored Hillshade - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-56</southBoundLatitude>
        <northBoundLatitude>60</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-56" maxx="180" maxy="60" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <BoundingBox CRS="EPSG:4326" minx="-56" miny="-180" maxx="60" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="155" height="241">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=SRTM30-Colored-Hillshade&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
    <Layer>
      <Name>SRTM30-Contour</Name>
      <Title>SRTM30 Contour Lines - by terrestris</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-56</southBoundLatitude>
        <northBoundLatitude>60</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-56" maxx="180" maxy="60" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
      <BoundingBox CRS="EPSG:4326" minx="-56" miny="-180" maxx="60" maxy="180" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-7558415.65608" maxx="20037508.3428" maxy="8399737.88982" />
    </Layer>
    <Layer>
      <Name>Dark</Name>
      <Title>Dark</Title>
      <EX_GeographicBoundingBox>
        <westBoundLongitude>-180</westBoundLongitude>
        <eastBoundLongitude>180</eastBoundLongitude>
        <southBoundLatitude>-89.999999</southBoundLatitude>
        <northBoundLatitude>89.999999</northBoundLatitude>
      </EX_GeographicBoundingBox>
      <BoundingBox CRS="CRS:84" minx="-180" miny="-89.999999" maxx="180" maxy="89.999999" />
      <BoundingBox CRS="EPSG:900913" minx="-20037508.3428" miny="-147730762.67" maxx="20037508.3428" maxy="147730758.195" />
      <BoundingBox CRS="EPSG:4326" minx="-90.0" miny="-180.0" maxx="90.0" maxy="180.0" />
      <BoundingBox CRS="EPSG:3857" minx="-20037508.3428" miny="-147730762.67" maxx="20037508.3428" maxy="147730758.195" />
      <Style>
        <Name>default</Name>
        <Title>default</Title>
        <LegendURL width="20" height="20">
          <Format>image/png</Format>
          <OnlineResource xlink:type="simple" xlink:href="https://ows.terrestris.de/osm/service?styles=&amp;layer=Dark&amp;service=WMS&amp;format=image%2Fpng&amp;sld_version=1.1.0&amp;request=GetLegendGraphic&amp;version=1.1.1" />
        </LegendURL>
      </Style>
    </Layer>
  </Layer>
</Capability>
</WMS_Capabilities>
)capabilities");
	*/

	ecs::registry<
		ecs::component<transform, 1000>
		, ecs::component<a_device, 2000>
		// , ecs::component<b_device, 3000>
		>
		registry;

	constexpr const size_t ent_count = 1'000'000;
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	for(size_t i = 0; i < ent_count; i++)
	{
		const std::string name = "value." + std::to_string(i);
		auto testEntity = registry.createEntity();
		transform* value = registry.addComponent<transform>(testEntity, glm::vec3(0), glm::identity<glm::quat>(), name);
		//auto* value = registry.addComponent<a_device>(testEntity);
		(void) value;
		/*
		{
			transform* value = registry.addComponent<transform>(testEntity);
			value->name = "value.A";
		}
		{
			transform* value = registry.addComponent<transform>(testEntity);
			value->name = "value.B";
		}
		{
			transform* value = registry.addComponent<transform>(testEntity, glm::vec3(0), glm::identity<glm::quat>(), "value.C");
		}
		*/
	}
	auto end = high_resolution_clock::now();
	std::printf("Creating %zu entities took : %.4f ms\n",
		ent_count,
		static_cast<double>(duration_cast<nanoseconds>(end - start).count()) / 1'000'000.0
	);

	std::cout << std::endl;

	render::WorldRenderer renderer;
	if(!renderer.init())
	{
		std::printf("Failed to init world renderer!\n");
		return EXIT_FAILURE;
	}

	renderer.run();

	auto& components = registry.getComponentsOfType<transform>();
	(void) components;
	// ecs::helper::print_error<decltype(components.begin())::reference>();

	//for(auto& item : components)
	//{
	//	std::printf("transform: %s\n", item.name.c_str());
	//}

	ecs::entity entity = registry.createEntity();
	a_device*  device = registry.addComponent<a_device>(entity);
	(void) device;

	return 0;
}
