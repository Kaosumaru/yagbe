#include "context.hpp"
#include "instructions_map.hpp"
#include <fstream>
#include <iostream>

using namespace yagbe;


void context::reset()
{
	registers.a = 0x01;
	registers.f = 0xb0;
	registers.b = 0x00;
	registers.c = 0x13;
	registers.d = 0x00;
	registers.e = 0xd8;
	registers.h = 0x01;
	registers.l = 0x4d;
	registers.sp = 0xfffe;
	registers.pc = 0x100;

	auto wb = [&](uint16_t a, uint8_t b) { this->memory.raw_at(a) = b; };
	

	//for (int i = 0; i <= 0xFFFF; i++)
	//	wb(i, 0);

	wb(0xFF05, 0);
	wb(0xFF06, 0);
	wb(0xFF07, 0);
	wb(0xFF10, 0x80);
	wb(0xFF11, 0xBF);
	wb(0xFF12, 0xF3);
	wb(0xFF14, 0xBF);
	wb(0xFF16, 0x3F);
	wb(0xFF17, 0x00);
	wb(0xFF19, 0xBF);
	wb(0xFF1A, 0x7A);
	wb(0xFF1B, 0xFF);
	wb(0xFF1C, 0x9F);
	wb(0xFF1E, 0xBF);
	wb(0xFF20, 0xFF);
	wb(0xFF21, 0x00);
	wb(0xFF22, 0x00);
	wb(0xFF23, 0xBF);
	wb(0xFF24, 0x77);
	wb(0xFF25, 0xF3);
	wb(0xFF26, 0xF1);
	wb(0xFF40, 0x91);
	wb(0xFF42, 0x00);
	wb(0xFF43, 0x00);
	wb(0xFF45, 0x00);
	wb(0xFF47, 0xFC);
	wb(0xFF48, 0xFF);
	wb(0xFF49, 0xFF);
	wb(0xFF4A, 0x00);
	wb(0xFF4B, 0x00);
	wb(0xFFFF, 0x00);


	interrupt.reset();
	gpu.reset();
	timer.reset();

	cycles_elapsed = 0;
	halted = false;

	auto null_write = [](yagbe::memory &m, uint16_t a, uint8_t b) 
	{
	};
	memory.map_interceptors(0x0000, 0x7FFF, nullptr, null_write); //resetting ROM intercepts

	auto shadow_read = [](yagbe::memory &m, uint16_t a) -> uint8_t { return m.read_at(a - 0x2000); };
	auto shadow_write = [](yagbe::memory &m, uint16_t a, uint8_t b) { return m.write_byte_at(a - 0x2000, b); };

	memory.map_interceptors(0xE000, 0xFDFF, shadow_read, shadow_write); //resetting shadow RAM interceptors


	std::array<uint8_t, 256> io_write_masks;
	std::fill(io_write_masks.begin(), io_write_masks.end(), 0xFF);

	io_write_masks[0x44] = 0; //FF44 LY is read-only
	io_write_masks[0x41] = 0b1111000; //FF41 STAT has writeable 6-3 bits
	io_write_masks[0x00] = 0b0110000; //FF41 STAT has 2 writeable bits

	auto zero_write = [this, io_write_masks](yagbe::memory &m, uint16_t a, uint8_t b)
	{
		//DMA
		if (a == 0xFF46)
		{
			//throw std::runtime_error("NYI");
			auto *src = m.raw_pointer_at((int)b*0x100);
			auto *dst = m.raw_pointer_at(0xFE00);
			std::copy(src, src + 4 * 40, dst);

			return;
		}

		if (a >= 0xFF10 && a <= 0xFF10)
		{
			//sounds, NYI
			m.raw_at(a) = b;
			return;
		}

		if (a > 0xFF4B && a != 0xFFFF)
		{
			//stack memory
			m.raw_at(a) = b;
			return;
		}

		if (a == 0xFF04)
		{
			this->timer.write_at_div();
			return;
		}

		if (a == 0xFF04)
		{
			this->timer.write_at_tima();
			return;
		}

		//TODO check this
		auto &destination = m.raw_at(a);
		auto bit_mask = io_write_masks[a & 0xFF];

		auto ones_to_write = b & bit_mask;
		destination |= ones_to_write;

		auto zeroes_to_write = ~((~b) & bit_mask);
		destination &= zeroes_to_write;
	};

	memory.map_interceptors(0xFF00, 0xFFFF, nullptr, zero_write); //resetting zero RAM (IO) interceptors
}

bool context::load_rom(const std::string& path)
{
	reset();

	std::ifstream fs;
	fs.open(path, std::fstream::binary);

	if (!fs)
		return false;

	auto it_begin = std::istreambuf_iterator<char>(fs);
	auto it_end = std::istreambuf_iterator<char>();

	uint16_t a = 0;
	for (auto it = it_begin; it != it_end; it++)
	{
		memory.raw_at(a) = *it;
		a++;
	}

	return true;
}

bool context::load_rom(uint8_t *data, int size)
{
	for (uint16_t a = 0; a < size; a++)
		memory.raw_at(a) = data[a];
	return true;
}

//#define TEST_DEBUG

void context::cpu_step()
{
	

#ifdef TEST_DEBUG
	auto pc = registers.pc;
#endif

	int cycles = 4;

	if (!halted)
	{
		auto opcode = read_byte();
		auto instruction = instructions()[opcode];
		cycles = instruction(*this);		
	}

	cycles_elapsed += cycles;

	gpu.step(cycles);
	timer.step(cycles);
	key_handler.step();

#ifdef TEST_DEBUG
	auto new_pc = registers.pc;
#endif

//	key_handler.step();
}

const instructions_array& context::instructions()
{
	static instructions_array arr = { nullptr };

	if (arr[0] == nullptr)
		instructions::automap::impl::fill_instructions<>::fill(arr);

	return arr;
}
const instructions_array& context::cb_instructions()
{
	static instructions_array arr = { nullptr };

	if (arr[0] == nullptr)
		instructions::automap::impl::fill_instructions_cb<>::fill(arr);

	return arr;
}