#include "interrupts.hpp"
#include "context.hpp"

using namespace yagbe;

void interrupts::reset()
{
    enabled = 1;
    _c.memory.io_register.IE = 0;
    _c.memory.io_register.IF = 0;
}

void interrupts::acknowledge_interrupt(type t)
{
    auto bit = bit_for_interrupt(t);
    _c.memory.io_register.IF |= bit;

    if ((bit & _c.memory.io_register.IE) != 0)
        _c.halted = false;
}

int interrupts::step()
{
    if (_steps_to_enable > 0 && --_steps_to_enable == 0)
        enabled = 1;

    if ((_c.memory.io_register.IF & _c.memory.io_register.IE) == 0)
        return 0;

    if (enabled == 0)
        return 0;

    for (int i = 0; i < 5; i++)
    {
        auto interrupt_type = (type)i;
        auto bit = bit_for_interrupt(interrupt_type);

        if ((_c.memory.io_register.IF & bit) == 0)
            continue;
        if ((_c.memory.io_register.IE & bit) == 0)
            continue;

        _c.memory.io_register.IF &= ~bit; //reset that bit

        enabled = 0;
        auto address = address_of_interrupt(interrupt_type);

        // push address
        _c.push(_c.registers.pc);
        _c.registers.pc = address;

        return 20;
    }

    return 0;
}