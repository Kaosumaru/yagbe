#pragma once
#include <array>
#include "utils.hpp"
#include "vm/interrupts.hpp"

namespace yagbe
{

	class timer
	{
	public:
		timer(memory &m, interrupts &i) : _m(m), _i(i) {}

		void step(uint32_t delta_tclock)
		{
			auto delta_mclock = delta_tclock / 4;

			_div_increments_m += delta_mclock;

			{
				auto div_steps = _div_increments_m / _div_speed;
				if (div_steps > 0)
				{
					_div_increments_m %= _div_speed;
					_m.io_register.DIV += div_steps;
				}
			}
		}

		void write_at_div()
		{
			_m.io_register.DIV = 0;
		}

		void write_at_tima()
		{

		}

		void reset()
		{
			_div_increments_m = 0;
		}

	protected:
		int _div_speed = 4;
		int _div_increments_m = 0;

		interrupts &_i;
		memory &_m;
	};

}

