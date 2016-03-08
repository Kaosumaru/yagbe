#pragma once

#include <type_traits>
#include <tuple>

//travis has old version of gcc & clang, and updating them takes time, so here is few "polyfills" that yagbe will work on older compilers
#ifdef YAGBE_TRAVIS
namespace std
{
	template< bool B, class T = void >
	using enable_if_t = typename std::enable_if<B, T>::type;

	template< bool B, class T1, class T2 >
	using conditional_t = typename std::conditional<B, T1, T2>::type;

	template <std::size_t I, class T>
	using tuple_element_t = typename std::tuple_element<I, T>::type;

	template< class T, class... Args >
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
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

	template<typename T>
	constexpr bool is_bit_set(T&& t, int bit)
	{
		return (t >> bit) & 1;
	}

	struct bit
	{
		bit(uint8_t &p, uint8_t N) : _p(p), n(N) {}
		bit(uint8_t &p, int N) : _p(p), n(N) {}

		bit& operator =(bool b)
		{
			int x = b ? 1 : 0;
			_p ^= (-x ^ _p) & (1 << n);
			return *this;
		}

		explicit operator bool() const
		{
			return (_p >> n) & 1;
		}
	protected:
		uint8_t &_p;
		uint8_t n;
	};

	//only bits that are 1 in mask will be written from source to d
	template<typename T>
	void update_byte_with_mask(T &d, T source, T bit_mask)
	{
		auto ones_to_write = source & bit_mask;
		d |= ones_to_write;

		auto zeroes_to_write = ~((~source) & bit_mask);
		d &= zeroes_to_write;
	}

}