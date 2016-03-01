#pragma once
#include <array>
#include "registers.hpp"
#include "memory.hpp"

namespace yagbe
{
	class context;
	using instruction_pointer = int(*)(context &c);
	using instructions_array = std::array<instruction_pointer, 256>;

	class context
	{
	protected:
		//TODO endianess
		union ByteWord
		{
			uint16_t word;
			uint8_t byte[2];
		};

	public:
		context() : flags(registers.f)
		{
			reset();
		}

		void reset();
		bool load_rom(const std::string& path);

		yagbe::registers registers;
		yagbe::flags flags;
		yagbe::memory memory;

		struct 
		{
			uint8_t enabled;
			uint8_t enable_specific;
			uint8_t flags;
		} interrupt;

		uint32_t cycles_elapsed;
		bool     stopped;


		const instructions_array& instructions();
		const instructions_array& cb_instructions();
		void cpu_step();

		//reads a byte at PC, and increments PC
		uint8_t read_byte()
		{
			return memory.at(registers.pc++);
		}

		void write_byte_at(uint16_t address, uint8_t byte)
		{
			memory.at(address) = byte;
		}

		void write_word_at(uint16_t address, uint16_t word)
		{
			ByteWord w = { word };

			memory.at(address) = w.byte[0];
			memory.at(address+1) = w.byte[1];
		}

		//reads a word at PC, and increments PC
		uint16_t read_word()
		{
			ByteWord w;
			w.byte[0] = read_byte();
			w.byte[1] = read_byte();
			return w.word;
		}

		void push(uint16_t word)
		{
			ByteWord w = { word };

			memory.at(registers.sp--) = w.byte[0];
			memory.at(registers.sp--) = w.byte[1];
		}

		uint16_t pop()
		{
			ByteWord w;

			w.byte[1] = memory.at(registers.sp++);
			w.byte[0] = memory.at(registers.sp++);

			return w.word;
		}

	};
};