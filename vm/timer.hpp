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

		void step(uint32_t delta_tclock, uint32_t cycles_elapsed)
		{

			_div_increments += delta_tclock;
			while (_div_increments >= _div_speed)
			{
				_div_increments -= _div_speed;
				_m.io_register.DIV++;
			}

			_m.io_register.DIV = cycles_elapsed / 256;

			if (!_m.io_register.TAC_running)
				return;

			auto speed_mode = _m.io_register.TAC & 0b11;
			auto tima_speed = mode_speeds()[speed_mode];
			_tima_increments += delta_tclock;

			while (_tima_increments >= tima_speed)
			{
				if (_m.io_register.TIMA == 255)
				{
					_m.io_register.TIMA = _m.io_register.TMA;
					_i.timer();
				}
				else
					_m.io_register.TIMA++;

				_tima_increments -= tima_speed;
			}
		}

		void write_at_div()
		{
			_m.io_register.DIV = 0;
		}

		void reset()
		{
			_tima_increments = 0;
			_div_increments = 0;
		}

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar(_tima_increments, _div_increments, _div_speed);
		}

	protected:
		//M-Clock = 1,048,576Hz

		//00 : 4096Hz   - /256
		//01 : 262144Hz - /4
		//10 : 65536Hz  - /16
		//11 : 16384Hz  - /64
		constexpr static std::array<int, 4> mode_speeds() { return{ 256*4, 4 * 4, 16 * 4, 64 * 4 }; }


		int _tima_increments = 0;

		int _div_increments = 0;
		int _div_speed = 256;

		interrupts &_i;
		memory &_m;
	};

}

