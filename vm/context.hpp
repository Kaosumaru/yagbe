#pragma once

#include "registers.hpp"
#include "memory.hpp"

namespace yagbe
{
	class context
	{
	public:
		context() : flags(registers.f)
		{

		}

		yagbe::registers registers;
		yagbe::flags flags;
		yagbe::memory memory;

		//reads a byte at PC, and increments PC
		uint8_t read_byte()
		{
			return memory.at(registers.pc++);
		}

		//reads a word at PC, and increments PC
		uint16_t read_word()
		{
			union
			{
				uint8_t bytes[2];
				uint16_t word;
			};

			//TODO endianess
			bytes[0] = read_byte();
			bytes[1] = read_byte();
			return word;
		}
	};
};