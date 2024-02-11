#include <iostream>
#include <cstdio>
#include <string_view>
#include <string>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "registry.hpp"

#include "worldrender.h"

struct transform
{
	glm::vec3 position;
	glm::quat rotation;
	std::string name;
};

struct a_device {};
struct b_device {};


int main(int argc, char** argv) {
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
