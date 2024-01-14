
#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <cstdint>
#include <array>
#include <tuple>
#include <iostream>
#include <type_traits>
#include <cstring>

#include "helper.hpp"
#include "storage.hpp"

namespace ecs
{

struct entity
{
	template<typename...> friend class registry;
	// template<typename...> friend class storage::storage;

	static constexpr size_t invalid{std::numeric_limits<size_t>::max()};

	entity() = default;
	size_t id() const
	{
		return m_id;
	}

private:
	explicit entity(size_t a_id)
		: m_id{a_id}
	{}

	size_t m_id{invalid};
};

namespace internal
{

template <typename Type>
struct internal_component
{
	using type = Type;

	entity entity_id;
	Type value;
};

template <typename... Components>
struct internal_entity
{
	entity value;
	void* components[sizeof...(Components)]{};
};

template <typename Type, template<typename> class InternalType = internal::internal_component>
struct registry_storage_iterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = Type;
	using pointer           = std::add_pointer_t<Type>;
	using reference         = std::add_lvalue_reference_t<Type>;

	using self_iterator     = registry_storage_iterator<Type>;
	using that_iterator     = storage::storage_node_iterator<InternalType<Type>>;

public:
	registry_storage_iterator(that_iterator a_ptr)
		: m_ptr{a_ptr}
	{}

	reference operator*() const { return (*m_ptr).value; }
    pointer operator->() { return &((*m_ptr).value); }

	self_iterator& operator++() {
		m_ptr.operator++();
		return *this;
	}

	friend bool operator== (const self_iterator& a, const self_iterator& b) { return a.m_ptr == b.m_ptr; };
    friend bool operator!= (const self_iterator& a, const self_iterator& b) { return a.m_ptr != b.m_ptr; };
private:
	that_iterator m_ptr;
};

template <typename Type, size_t PageSize, size_t ExpectedSize = 0>
struct registry_storage
{
	using iterator = registry_storage_iterator<Type>;

public:
	[[nodiscard]] iterator begin() { return iterator(m_storage.begin()); }
	[[nodiscard]] iterator end() { return iterator(m_storage.end()); }
	[[nodiscard]] size_t size() const { return m_storage.count(); }
	[[nodiscard]] Type* get(const size_t a_index)
	{
		return &m_storage.get(a_index)->value;
	}

	template <typename... Args>
	internal::internal_component<Type>* emplace(entity a_entity, Args&&... a_arguments)
	{
		return m_storage.emplace(a_entity, a_arguments...);
	}
private:
	storage::storage<internal::internal_component<Type>, PageSize, ExpectedSize> m_storage;
};

}

// mingw64    (1) -> 767.2480 ms
// mingw64    (4096) -> 254.9370 

// llvm-clang (1) -> 1442.4755 ms
// llvm-clang (4096) -> 808.6714 ms

constexpr const size_t TEST_SIZE = 1024 * 1024;//4096;

template <typename Type, size_t Size>
struct component
{
	using type = Type;
	using storage_type = internal::registry_storage<Type, TEST_SIZE, Size>;
	using pointer_type = std::add_pointer_t<Type>;
};

namespace details
{
	template <typename Find, size_t Index, bool Found>
	struct find_result
	{
		static constexpr size_t index{Index};
		static constexpr bool found{Found};
		using type = Find;
	};

	template <size_t Index, typename... Components>
	struct find_component {};
	
	template <size_t Index, typename Find, typename Check>
	struct find_component<Index, Find, Check>
	{
		using type = std::conditional_t<
			std::is_same_v<Find, typename Check::type>,
			find_result<Check, Index, true>,
			find_result<component<void, 0>, Index, false>>;
	};

	template <size_t Index, typename Find, typename Check, typename... Components>
	struct find_component<Index, Find, Check, Components...>
	{
		using type = std::conditional_t<
			std::is_same_v<Find, typename Check::type>,
			find_result<Check, Index, true>,
			typename find_component<Index + 1, Find, Components...>::type>;
	};

	
	template <typename Find, typename... Components>
	using find_component_t = find_component<0, Find, Components...>::type;
}

template <typename... Components>
class registry
{
public:
	template <typename Component>
	constexpr void testing()
	{
		// details::print_error<details::get_type_index<Component, Components...>>();
		// details::print_error<details::get_type_array<Component, Components...>>();
	}

	template <typename Component>
	inline constexpr auto& getComponentsOfType()
	{
		using result = details::find_component_t<Component, Components...>;
		static_assert(result::found, "Component not part of registry");
		return std::get<result::index>(m_componentStorage);
	}

	template <typename Component, typename... Args>
	constexpr Component* addComponent(entity a_entity, Args&&... a_arguments)
	{
		using result = details::find_component_t<Component, Components...>;
		static_assert(result::found, "Component not part of registry");
		if(a_entity.m_id == entity::invalid)
		{
			return nullptr;
		}

		// std::printf("Trying to get entity %zu\n", a_entity.m_id);
		auto* entity = m_entities.get(a_entity.m_id);
		void* curr = entity->components[result::index];

		// helper::print_error<decltype(entity)>();
		// std::printf("B registry.addComponent m_id=%zu entity(%p) index=%zu ptr=%p\n", a_entity.m_id, (void*) entity, result::index, (void*) curr);
		// for(size_t i = 0; i < sizeof...(Components); i++)
		// {
		// 	std::printf("- i=%ld, ptr=%p\n", i, entity->components[i]);
		// }
		if(curr == nullptr)
		{
			auto& data = std::get<result::index>(m_componentStorage);
			auto* comp = data.emplace(a_entity, a_arguments...);
			// std::printf("stack? %p\n", (void*) comp);
			curr = &comp->value;
		}
		else
		{
			*static_cast<Component*>(curr) = Component {a_arguments...};
		}
		// std::printf("aftrrr\n");

		return static_cast<Component*>(curr);
	}

	[[nodiscard]] entity createEntity()
	{
		// std::printf("\n=========================================\n");

		entity value(m_uniqueEntity++);
		auto& entity = m_entities.emplace(value, internal::internal_entity<typename Components::type...>(value))->value;
		std::memset(&entity.components, 0, sizeof(void*) * sizeof...(Components));
		// std::printf("  \nentity(%zu) -> %p\n", value.m_id, (void*) &entity);

		// auto* check = m_entities.get(value.m_id);
		// helper::print_error<decltype(check)>();

		// if(check != &entity)
		// {
		// 	std::printf("AAOSDFAOSIDUFOAISDUFOAUDFOIAUSDFOIUASDOFIUASDF\n");
		// 	std::printf("emplace %p\n", (void*) &entity);
		// 	std::printf("get     %p\n", (void*) check);
		// }

		return entity.value;
	}

	void removeEntity(entity a_entity)
	{
		std::printf("Getting index %zu\n", a_entity.m_id);
		m_entities.get(a_entity.m_id);

	}

private:
	size_t m_uniqueEntity{0};
	internal::registry_storage<internal::internal_entity<typename Components::type...>, TEST_SIZE> m_entities;
	std::tuple<typename Components::storage_type...> m_componentStorage;
};

}

#endif  // ECS_REGISTRY_H
