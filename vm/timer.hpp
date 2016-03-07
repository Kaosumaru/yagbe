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
			_tima_increments_m += delta_mclock;

			{
				auto div_steps = _div_increments_m / _div_speed;
				if (div_steps > 0)
				{
					_div_increments_m %= _div_speed;
					_m.io_register.DIV += div_steps;
				}
			}

			if (!_m.io_register.TAC_running)
				return;

			{
				auto speed_mode = _m.io_register.TAC & 0b11;
				_tima_speed = mode_speeds()[speed_mode];

				auto tima_steps = _tima_increments_m / _tima_speed;
				_tima_increments_m %= _tima_speed;
				bool interrupt = false;

				while (tima_steps > 0)
				{
					if (_m.io_register.TIMA == 255)
						_m.io_register.TIMA = _m.io_register.TMA, interrupt = true;
					_m.io_register.TIMA ++;
					tima_steps--;
				}

				if (interrupt)
					_i.timer();
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
		//M-Clock = 1,048,576Hz

		//00 : 4096Hz   - /256
		//01 : 262144Hz - /4
		//10 : 65536Hz  - /16
		//11 : 16384Hz  - /64
		constexpr static std::array<int, 4> mode_speeds() { return{ 256, 4, 16, 64 }; }


		int _div_speed = 4;
		int _div_increments_m = 0;

		int _tima_speed = 4;
		int _tima_increments_m = 0;

		interrupts &_i;
		memory &_m;
	};

}

