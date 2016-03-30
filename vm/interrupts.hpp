#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"

namespace yagbe
{
	class context;

	class interrupts
	{
	public:
		interrupts(context &c) : _c(c) {}
	
		
		void vblank() { acknowledge_interrupt(type::vblank); }
		void lcd_status() { acknowledge_interrupt(type::lcd_status); }
		void timer() { acknowledge_interrupt(type::timer); }
		void serial() { acknowledge_interrupt(type::serial); }
		void joypad() { acknowledge_interrupt(type::joypad); }

		uint8_t enabled = 1;

		void reset();

		int step();

		void delay_enable(int steps) { _steps_to_enable = steps == 2 && _steps_to_enable == 2 ? 1 : steps; }

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar(_steps_to_enable, enabled);
		}
	protected:
		enum class type
		{
			vblank = 0,
			lcd_status,
			timer,
			serial,
			joypad
		};

		int bit_for_interrupt(type t)
		{
			return 1 << (int)t;
		}
		

		void acknowledge_interrupt(type t);



		static uint16_t address_of_interrupt(type t)
		{
			static std::array<uint16_t, 5> address{ 0x0040, 0x0048, 0x0050, 0x0058, 0x0060 };
			return address[(int)t];
		}

		void process_interrupt(type t);

	protected:
		int _steps_to_enable = 0;
		context &_c;
	};
};