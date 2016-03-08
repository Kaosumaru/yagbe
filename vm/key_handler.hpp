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
			
		}

		void step()
		{
			bool r0 = (P1 & 0x10) != 0;
			bool r1 = (P1 & 0x20) != 0;

			uint8_t temp = 0xFF;

			if (r0)
				temp &= rows[0];
			if (r1)
				temp &= rows[1];

			update_byte_with_mask<uint8_t>(P1, temp, 0b00001111);

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
		//int _selected_column = 0;
		uint8_t rows[2] = { 0b00001111, 0b00001111 };
		interrupts &_i;
	};


};