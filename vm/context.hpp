#pragma once

#include "registers.hpp"
#include "memory.hpp"

namespace yagbe
{
	class context
	{
	public:
		yagbe::registers registers;
		yagbe::memory memory;
	};
};