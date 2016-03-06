#pragma once
#include <cstdint>
#include <array>
#include "utils.hpp"


namespace yagbe
{
	class io_registers
	{
		uint8_t *_a;
	public:
		io_registers(uint8_t *a) : _a(a)
		{

		}

		//controls NYI
		uint8_t &P1{ _a[0xFF00] }; //R/W Register for reading joy pad info and determining system type

		//serial io NYI
		uint8_t &SB{ _a[0xFF01] }; //X R/W Serial transfer data
		uint8_t &SC{ _a[0xFF02] }; //R/W SIO control

		//timers NYI
		uint8_t &DIV{ _a[0xFF04] }; //X R/W Divider Register
		uint8_t &TIMA{ _a[0xFF05] }; //X R/W Timer counter
		uint8_t &TMA{ _a[0xFF06] }; //X R/W timer modulo
		uint8_t &TAC{ _a[0xFF07] }; //X R/W timer control

		//interrupts
		uint8_t &IF{ _a[0xFF0F] }; //R/W Interrupt Flag

		//FF10-FF40 - music


		uint8_t &LCDC{ _a[0xFF40] }; //X R/W LCD Control
		bit LCDC_background { LCDC, 0 };
		bit LCDC_sprites { LCDC, 1 };
		bit LCDC_sprites_size { LCDC, 2 };
		bit LCDC_background_tile_map { LCDC, 3 };
		bit LCDC_background_tile_set { LCDC, 4 };
		bit LCDC_window{ LCDC, 5 }; //NYI
		bit LCDC_window_tile_map{ LCDC, 6 }; //NYI
		bit LCDC_display{ LCDC, 7 }; //NYI


		uint8_t &STAT{ _a[0xFF41] }; //X R/W LCDC Status
		bit STAT_mode_0bit{ STAT, 0 };
		bit STAT_mode_1bit{ STAT, 1 };
		bit STAT_ly_equals_lyc{ STAT, 2 };
		bit STAT_interrupt_mode00{ STAT, 3 };
		bit STAT_interrupt_mode01{ STAT, 4 };
		bit STAT_interrupt_mode10{ STAT, 5 };
		bit STAT_interrupt_ly_equals_lyc{ STAT, 6 };


		uint8_t &SCY{ _a[0xFF42] }; //R/W scroll y
		uint8_t &SCX{ _a[0xFF43] };	//R/W scroll x

		uint8_t &LY{ _a[0xFF44] }; //X R LCDC Y-Coordinate
		uint8_t &LYC{ _a[0xFF45] }; //X R/W LY Compare
		uint8_t &DMA{ _a[0xFF46] }; //X W DMA Transfer and Start Address 

		uint8_t &BGP{ _a[0xFF47] }; //R/W BG & Window Palette Data
		uint8_t &OBP0{ _a[0xFF48] }; //R/W Object Palette 0 Data
		uint8_t &OBP1{ _a[0xFF49] }; //R/W Object Palette 1 Data

		uint8_t &WY{ _a[0xFF4A] }; //X R/W Window Y Position
		uint8_t &WX{ _a[0xFF4B] }; //X R/W Window X Position

		uint8_t &IE{ _a[0xFFFF] }; //X R/W interrupt enable

	};


};