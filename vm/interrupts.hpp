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
	
		
		void vblank() { process_interrupt(type::vblank); }
		void lcd_status() { process_interrupt(type::lcd_status); }
		void timer() { process_interrupt(type::timer); }
		void serial() { process_interrupt(type::serial); }
		void joypad() { process_interrupt(type::joypad); }

		uint8_t enabled = 1;

		void reset();
	protected:
		enum class type 
		{
			vblank = 0,
			lcd_status,
			timer,
			serial,
			joypad
		};

		static uint16_t address_of_interrupt(type t)
		{
			static std::array<uint16_t, 5> address{ 0x0040, 0x0048, 0x0050, 0x0058, 0x0060 };
			return address[(int)t];
		}

		void process_interrupt(type t);

	protected:
		context &_c;
	};
};