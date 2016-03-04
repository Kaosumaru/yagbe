#pragma once
#include <cstdint>
#include <array>



namespace yagbe
{
	class io_registers
	{
		uint8_t *_a;
	public:
		io_registers(uint8_t *a) : _a(a)
		{

		}

		uint8_t &scy{ _a[0xFF42] };
		uint8_t &scx{ _a[0xFF43] };	
	};


};