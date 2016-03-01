#include "context.hpp"
#include "instructions_map.hpp"

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

	interrupt.enabled = 1;
	interrupt.enable_specific = 0;
	interrupt.flags = 0;

	write_byte_at(0xFF05, 0);
	write_byte_at(0xFF06, 0);
	write_byte_at(0xFF07, 0);
	write_byte_at(0xFF10, 0x80);
	write_byte_at(0xFF11, 0xBF);
	write_byte_at(0xFF12, 0xF3);
	write_byte_at(0xFF14, 0xBF);
	write_byte_at(0xFF16, 0x3F);
	write_byte_at(0xFF17, 0x00);
	write_byte_at(0xFF19, 0xBF);
	write_byte_at(0xFF1A, 0x7A);
	write_byte_at(0xFF1B, 0xFF);
	write_byte_at(0xFF1C, 0x9F);
	write_byte_at(0xFF1E, 0xBF);
	write_byte_at(0xFF20, 0xFF);
	write_byte_at(0xFF21, 0x00);
	write_byte_at(0xFF22, 0x00);
	write_byte_at(0xFF23, 0xBF);
	write_byte_at(0xFF24, 0x77);
	write_byte_at(0xFF25, 0xF3);
	write_byte_at(0xFF26, 0xF1);
	write_byte_at(0xFF40, 0x91);
	write_byte_at(0xFF42, 0x00);
	write_byte_at(0xFF43, 0x00);
	write_byte_at(0xFF45, 0x00);
	write_byte_at(0xFF47, 0xFC);
	write_byte_at(0xFF48, 0xFF);
	write_byte_at(0xFF49, 0xFF);
	write_byte_at(0xFF4A, 0x00);
	write_byte_at(0xFF4B, 0x00);
	write_byte_at(0xFFFF, 0x00);

	cycles_elapsed = 0;
	stopped = false;
}


void context::cpu_step()
{
	if (stopped) return;


	auto opcode = read_byte();
	auto instruction = instructions()[opcode];
	cycles_elapsed += instruction(*this);
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