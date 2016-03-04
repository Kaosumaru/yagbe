#pragma once
#include <cstdint>
#include <array>
#include "utils.hpp"
#include "vm/interrupts.hpp"

namespace yagbe
{
	class key_handler
	{
		uint8_t &P1;
	public:
		enum class key
		{
			A = 0,
			B = 1,
			Select = 2,
			Start = 3,

			Right = 4,
			Left = 5,
			Up = 6,
			Down = 7,
		};

		key_handler(uint8_t &p1, interrupts &i) : P1(p1), _i(i)
		{
			P1 = 0;
		}

		void step()
		{
			//int offset = P1 & 0x20 ? 4 : P1 & 0x10 ? 0 : -1;
			
			P1 = rows[1];
			
		}

		void set_key(key k, bool v)
		{
			int i = (int)k;
			int r = i / 4;
			i %= 4;

			bit b(rows[r], i);
			b = !v; //on bit = released, off - pressed
			_i.joypad();
		}

	protected:
		uint8_t rows[2] = { 0xF, 0xF };
		interrupts &_i;
	};


};