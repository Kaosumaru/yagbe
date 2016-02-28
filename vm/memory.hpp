#pragma once
#include <cstdint>

namespace yagbe
{
	class memory
	{
	public:
		auto& get(uint16_t address)
		{
			return data[address];
		}

		uint8_t data[0xFFFF];
	};
};