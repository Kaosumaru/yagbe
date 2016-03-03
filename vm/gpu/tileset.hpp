#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"

namespace yagbe
{
	class tileset
	{
	public:
		tileset(memory &m) : _m(m)
		{

		}

		color pixel_at_point(uint8_t x, uint8_t y)
		{
			return {0,0,255,255};
		}

	protected:
		memory &_m;
	};


};