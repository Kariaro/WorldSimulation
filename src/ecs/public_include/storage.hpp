
#ifndef ECS_STORAGE_H
#define ECS_STORAGE_H

#include <cstdint>
#include <array>
#include <type_traits>
#include <memory>

#include <iostream>

namespace ecs::storage
{

namespace internal
{
	template <auto Number>
	constexpr const bool is_power_of_two = ((Number & (Number - 1)) == 0);

	template <auto Number>
	constexpr int log2()
	{
		if constexpr(Number == 0)
		{
			return -1;
		}

		int index = 0;
		while(index < 64)
		{
			if(((Number >> index) & 1) != 0)
			{
				return index;
			}

			index++;
		}

		// Infinite loop?
		return -1;
	}

	template <auto Number, auto Other>
	constexpr const auto max = Number > Other ? Number : Other;
}

template <typename Type>
struct storage_node
{
	using type = Type;
	storage_node<Type>* next;
	storage_node<Type>* prev;
	Type data;
};

template <typename Type>
struct storage_node_iterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = Type;
	using pointer           = std::add_pointer_t<Type>;
	using reference         = std::add_lvalue_reference_t<Type>;

	using self_iterator     = storage_node_iterator<Type>;
	using node_type         = storage_node<Type>;

public:
	explicit storage_node_iterator(node_type* a_ptr)
		: m_ptr{a_ptr}
	{}

	reference operator*() const { return m_ptr->data; }
    pointer operator->() { return &m_ptr->data; }

	self_iterator& operator++() {
		m_ptr = m_ptr->next;
		return *this;
	}

	friend bool operator== (const self_iterator& a, const self_iterator& b) { return a.m_ptr == b.m_ptr; };
    friend bool operator!= (const self_iterator& a, const self_iterator& b) { return a.m_ptr != b.m_ptr; };
private:
	node_type* m_ptr;
};

template <typename Type, size_t PageSize, size_t ExpectedSize = 0>
struct storage
{
	static constexpr int PageShift = internal::log2<PageSize>();
	static constexpr int PageMask = ~((~0) << internal::max<PageShift - 1, 0>);
	static_assert(internal::is_power_of_two<PageSize> && PageShift >= 0, "PageSize is not a power of two");
	static_assert((1 << PageShift) == PageSize);

	using iterator = storage_node_iterator<Type>;
	using element_type = Type;
	using node_type = storage_node<Type>;
	using page_type = std::array<node_type, PageSize>;

	storage()
	{
		create_page<true>();
	}

private:
	size_t m_count{0};
	node_type* m_tailPrev{nullptr};
	node_type* m_head{nullptr};
	node_type* m_tail{nullptr}; // Element to be written to
	std::vector<std::unique_ptr<page_type>> m_pages;

	template <bool First>
	constexpr void create_page()
	{
		auto& page = *m_pages.emplace_back(std::make_unique<page_type>());

		auto& first = page[0];
		auto& last = page[PageSize - 1];
		if constexpr(First)
		{
			first.prev = nullptr;
			m_head = &first;
		}
		else
		{
			first.prev = m_tailPrev;
			m_tailPrev->next = &first;
		}

		m_tail = &first;
		if constexpr(PageSize > 1)
		{
			first.next = &page[1];
			last.prev = &page[PageSize - 2];
		}

		last.next = nullptr;
		for(size_t i = 1; i < PageSize - 1; i++)
		{
			auto& item = page[i];
			item.prev = &(page[i - 1]);
			item.next = &(page[i + 1]);
		}
	}
public:
	iterator begin() { return iterator(m_head); }
	iterator end() { return iterator(m_tail); }
	size_t size() const { return m_count; }
	
	/// Returns an element at the specified position 
	Type* get(size_t a_index)
	{
		auto& page = *(m_pages[a_index >> PageShift]);
		auto result = &page[a_index & PageMask].data;
		return result;
	}

	template <typename... Args>
	Type* emplace(Args&&... a_arguments)
	{
		size_t pages_requred = m_count >> PageShift;
		if(pages_requred >= m_pages.size())
		{
			create_page<false>();
		}

		m_count++;
		auto* current = m_tail;
		current->data = typename node_type::type {a_arguments...};

		m_tailPrev = m_tail;
		m_tail = m_tail->next;
		return &current->data;
	}
};

}

#endif  // ECS_STORAGE_H
