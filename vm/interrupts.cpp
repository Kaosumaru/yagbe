#include "interrupts.hpp"
#include "context.hpp"

using namespace yagbe;


void interrupts::reset()
{
	enabled = 1;
	_c.memory.io_register.IE = 0;
	_c.memory.io_register.IF = 0;
}

void interrupts::process_interrupt(type t)
{
	//return;
	auto address = address_of_interrupt(t);

	//1 IF shall be set
	_c.memory.io_register.IF = 1 << (int)t;

	//2 if IME is set, and corresponding IE is set...
	if (enabled == 0 || (_c.memory.io_register.IF & _c.memory.io_register.IE) == 0)
		return;

	_c.halted = false;

	// reset IME
	enabled = 0;

	// push address
	_c.push(_c.registers.pc);
	_c.registers.pc = address;
}