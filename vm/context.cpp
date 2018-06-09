#include "context.hpp"
#include "instructions_map.hpp"
#include <fstream>
#include <iostream>

using namespace yagbe;


void context::reset()
{
	registers.a = is_cbg() ? 0x11 : 0x01;
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

	uint8_t io_starting_state[] = {
		0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
		0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
		0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
		0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
		0x45, 0xEC, 0x52, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
		0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE, 0x46, 0x97, 0x33, 0x5E,
		0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5,
		0xD0, 0x7A, 0x00, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
		0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00 
	};

	for (int i = 0; i < 0x100; i++)
		wb(0xFF00 + i, io_starting_state[i]);

	/*
	//from no$gmb
	wb(0xFF00, 0xCE);
	wb(0xFF01, 0);
	wb(0xFF02, 0x7E);
	wb(0xFF03, 0xFF);
	wb(0xFF04, 0xAF);

	wb(0xFF05, 0);
	wb(0xFF06, 0);
	wb(0xFF07, 0xF8); //from no$gmb, was 0

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
	*/

	interrupt.reset();
	gpu.reset();
	apu.reset();
	timer.reset();
	_mbc_handler.reset();

	cycles_elapsed = 0;
	halted = false;

	{
		auto mbc_read = [](context &ctx, yagbe::memory &m, uint16_t a) -> uint8_t
		{
			if (m.c().current_mbc_handler())
				return m.c().current_mbc_handler()->handle_read(a);
			return m.raw_at(a);
		};

		auto mbc_write = [](context &ctx, yagbe::memory &m, uint16_t a, uint8_t b)
		{
			if (m.c().current_mbc_handler())
				m.c().current_mbc_handler()->handle_write(a, b);
		};

		memory.map_interceptors(0x0000, 0x7FFF, mbc_read, mbc_write); //resetting ROM intercepts
	}

	{
		auto mbc_ram_read = [](context &ctx, yagbe::memory &m, uint16_t a) -> uint8_t
		{
			if (m.c().current_mbc_handler())
				return m.c().current_mbc_handler()->handle_ram_read(a);
			return m.raw_at(a);
		};

		auto mbc_ram_write = [](context &ctx, yagbe::memory &m, uint16_t a, uint8_t b)
		{
			if (m.c().current_mbc_handler())
				m.c().current_mbc_handler()->handle_ram_write(a, b);
			else
				m.raw_at(a) = b;
		};

		memory.map_interceptors(0xA000, 0xBFFF, mbc_ram_read, mbc_ram_write); //resetting ROM intercepts
	}



	{
		auto shadow_read = [](context &ctx, yagbe::memory &m, uint16_t a) -> uint8_t { return m.read_at(a - 0x2000); };
		auto shadow_write = [](context &ctx, yagbe::memory &m, uint16_t a, uint8_t b) { return m.write_byte_at(a - 0x2000, b); };

		memory.map_interceptors(0xE000, 0xFDFF, shadow_read, shadow_write); //resetting shadow RAM interceptors
	}



	static std::array<uint8_t, 256> io_write_masks;
	std::fill(io_write_masks.begin(), io_write_masks.end(), 0xFF);

	io_write_masks[0x44] = 0; //FF44 LY is read-only
	io_write_masks[0x41] = 0b1111000; //FF41 STAT has writeable 6-3 bits
	io_write_masks[0x00] = 0b0110000; //FF41 STAT has 2 writeable bits

	io_write_masks[0x07] = 0b0111; //FF07 TAC has 3 writable bits


    io_write_masks[0x51] = 0b11111111; //HDMA1
    io_write_masks[0x52] = 0b11110000; //HDMA2
    io_write_masks[0x53] = 0b00011111; //HDMA3
    io_write_masks[0x54] = 0b11110000; //HDMA4


	auto zero_write = [](context &ctx, yagbe::memory &m, uint16_t a, uint8_t b)
	{
		//DMA
		if (a == 0xFF46)
		{
            auto src_address = (int)b * 0x100;
            auto dest_address = 0xFE00;
            int size = 4 * 40;

            for (int i = 0; i < size; i++)
            {
                auto data = m.read_at(src_address + i);
                m.write_byte_at(dest_address + i, data);
            }

#if 0       //this doesn't take banking into account
			auto *src = m.raw_pointer_at((int)b*0x100);
			auto *dst = m.raw_pointer_at(0xFE00);
			std::copy(src, src + 4 * 40, dst);
#endif
			return;
		}

		//LYC
		if (a == 0xFF45)
		{
			m.raw_at(a) = b;
			ctx.gpu.on_updated_ly_lyc();
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
			m.c().timer.write_at_div();
			return;
		}


		auto &destination = m.raw_at(a);
		auto bit_mask = io_write_masks[a & 0xFF];

		update_byte_with_mask(destination, b, bit_mask);
	};

	memory.map_interceptors(0xFF00, 0xFFFF, nullptr, zero_write); //resetting zero RAM (IO) interceptors
}

rom_info* context::load_rom(const std::string& path)
{
	reset();

	std::ifstream fs;
	fs.open(path, std::fstream::binary);

	if (!fs)
		return nullptr;

	auto it_begin = std::istreambuf_iterator<char>(fs);
	auto it_end = std::istreambuf_iterator<char>();


	std::vector<uint8_t> data;
	
	std::streampos fsize = 0;
	fs.seekg(0, std::ios::end);
	fsize = fs.tellg();
	fs.seekg(0, std::ios::beg);

	data.resize(fsize);
	std::copy(it_begin, it_end, data.begin());
	return load_rom(std::move(data));

}

rom_info* context::load_rom(uint8_t *data, int size)
{
	std::vector<uint8_t> vdata;
	vdata.resize(size);
	std::copy(data, data+size, vdata.begin());

	return load_rom(std::move(vdata));
}

rom_info* context::load_rom(std::vector<uint8_t>&& data)
{
	auto kb_rom_size = 0x8000;
	for (uint16_t a = 0; a < kb_rom_size; a++) //load first 32kb directly into memory, mbc handler is getting full data
		memory.raw_at(a) = data[a]; 

	auto info = current_rom_info();
	_mbc_handler = mbc_handler::create_for(info, memory, std::move(data));
	if (!_mbc_handler)
		return nullptr;
	return info;
}



//#define TEST_DEBUG

void context::cpu_step()
{


#ifdef TEST_DEBUG
	auto pc = registers.pc;
#endif

	int cycles = 4;

	auto passCycles = [&](int c) {
		cycles_elapsed += c;

		this->gpu.step(c);
		this->apu.step(c);
		this->timer.step(c, cycles_elapsed);
	};

	if (!halted)
	{
		auto opcode = read_byte();
		auto instruction = instructions()[opcode];
		cycles = instruction(*this);
	}



	passCycles(cycles);
	key_handler.step();

	{
		cycles = interrupt.step();
		if (cycles)
		{
			passCycles(cycles);
		}
	}
	

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