#pragma once
#include <cstdint>
#include <array>
#include "memory.hpp"


namespace yagbe
{
	class zero_ram
	{
	public:
		zero_ram(memory &m) : _m(m)
		{

		}

	protected:
		memory &_m;
	};


};