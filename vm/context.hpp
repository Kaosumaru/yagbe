#pragma once
#include <array>
#include "registers.hpp"
#include "memory.hpp"
#include "vm/gpu/gpu.hpp"
#include "zero_ram.hpp"

namespace yagbe
{
	class context;
	using instruction_pointer = int(*)(context &c);
	using instructions_array = std::array<instruction_pointer, 256>;

	class context
	{
	protected:


	public:
		context() : flags(registers.f)
		{
			reset();
		}

		void reset();
		bool load_rom(const std::string& path);

		yagbe::registers registers;
		yagbe::flags     flags;
		yagbe::memory    memory;
		yagbe::gpu       gpu = { memory };
		yagbe::zero_ram  zero_ram = { memory };

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
			return memory.read_at(registers.pc++);
		}

		//reads a word at PC, and increments PC
		uint16_t read_word()
		{
			memory::ByteWord w;
			w.byte[0] = read_byte();
			w.byte[1] = read_byte();
			return w.word;
		}

		void push(uint16_t word)
		{
			memory::ByteWord w = { word };

			memory.write_byte_at(registers.sp--, w.byte[0]);
			memory.write_byte_at(registers.sp--, w.byte[1]);
		}

		uint16_t pop()
		{
			memory::ByteWord w;

			w.byte[1] = memory.read_at(registers.sp++);
			w.byte[0] = memory.read_at(registers.sp++);

			return w.word;
		}

	};
};