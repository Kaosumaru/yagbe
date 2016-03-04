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
			int offset = P1 & 0x20 ? 4 : P1 & 0x10 ? 0 : -1;

			if (offset == -1)
			{
				P1 = 0xF;
				return;
			}

			for (int i = 0; i < 4; i++)
			{
				bit b(P1, i);
				b = !keys[offset + i];
			}
		}

		void set_key(key k, bool v)
		{
			keys[(int)k] = v;
			if (v)
				_i.joypad();
		}

	protected:
		bool keys[8] = { 0 };
		interrupts &_i;
	};


};