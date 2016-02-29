#pragma once
#include <cstdint>

namespace yagbe
{

template<uint8_t n>
struct flag
{
	flag(uint8_t &p) : _p(p) {}


	flag& operator =(bool b)
	{
		int x = b ? 1 : 0;
		_p ^= (-x ^ _p) & (1 << n);
		return *this;
	}

	explicit operator bool() const
	{
		return (_p >> x) & 1;
	}
protected:
	uint8_t &_p;
};

struct flags
{
	flags(uint8_t &p) : z(p), n(p), h(p), c(p) {}

	flag<7> z;
	flag<6> n;
	flag<5> h;
	flag<4> c;
};


struct registers
{
	struct {
		union {
			struct {
				uint8_t f;
				uint8_t a;
			};
			uint16_t af;
		};
	};
 
	struct {
		union {
			struct {
				uint8_t c;
				uint8_t b;
			};
			uint16_t bc;
		};
	};
 
	struct {
		union {
			struct {
				uint8_t e;
				uint8_t d;
			};
			uint16_t de;
		};
	};
 
	struct {
		union {
			struct {
				uint8_t l;
				uint8_t h;
			};
			uint16_t hl;
		};
	};
 
	uint16_t sp = 0xFFFE;
	uint16_t pc = 0x100;
};
 


}