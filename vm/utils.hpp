#pragma once

#include <type_traits>
#include <tuple>

#ifdef YAGBE_TRAVIS
namespace std
{
	template< bool B, class T = void >
	using enable_if_t = typename std::enable_if<B, T>::type;

	template< bool B, class T1, class T2 >
	using conditional_t = typename std::conditional<B, T1, T2>::type;

	template <std::size_t I, class T>
	using tuple_element_t = typename std::tuple_element<I, T>::type;
}

#endif

namespace yagbe
{
	struct ipoint
	{
		int x;
		int y;
	};

	struct color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};


}