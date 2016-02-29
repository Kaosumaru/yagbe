#pragma once
#include <type_traits>


#ifdef YAGBE_TRAVIS
namespace std
{
	template< bool B, class T = void >
	using enable_if_t = typename std::enable_if<B, T>::type;

	template< bool B, class T1, class T2 >
	using conditional_t = typename std::conditional<B, T1, T2>::type;
}

#endif

namespace yagbe
{

}