#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"


namespace yagbe
{
	class tileset
	{
	public:
		tileset(memory &m) : _m(m)
		{

		}

	protected:
		memory &_m;
	};


};