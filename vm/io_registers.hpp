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
		uint8_t &SC{ _a[0xFF02] }; //X R/W SIO control

		//timers NYI
		uint8_t &DIV{ _a[0xFF04] }; // R/W Divider Register
		uint8_t &TIMA{ _a[0xFF05] }; // R/W Timer counter
		uint8_t &TMA{ _a[0xFF06] }; // R/W timer modulo
		uint8_t &TAC{ _a[0xFF07] }; // R/W timer control

		bit TAC_running{ TAC, 2 };

		//interrupts
		uint8_t &IF{ _a[0xFF0F] }; //R/W Interrupt Flag

		//FF10-FF40 - music


		uint8_t &LCDC{ _a[0xFF40] }; //R/W LCD Control
		bit LCDC_background { LCDC, 0 };
		bit LCDC_sprites { LCDC, 1 };
		bit LCDC_sprites_size { LCDC, 2 };
		bit LCDC_background_tile_map { LCDC, 3 };
		bit LCDC_background_tile_set { LCDC, 4 };
		bit LCDC_window{ LCDC, 5 }; //NYI
		bit LCDC_window_tile_map{ LCDC, 6 }; //NYI
		bit LCDC_display{ LCDC, 7 }; //NYI


		uint8_t &STAT{ _a[0xFF41] }; //R/W LCDC Status
		bit STAT_mode_0bit{ STAT, 0 };
		bit STAT_mode_1bit{ STAT, 1 };
		bit STAT_ly_equals_lyc{ STAT, 2 };
		bit STAT_interrupt_mode00{ STAT, 3 };
		bit STAT_interrupt_mode01{ STAT, 4 };
		bit STAT_interrupt_mode10{ STAT, 5 };
		bit STAT_interrupt_ly_equals_lyc{ STAT, 6 };


		uint8_t &SCY{ _a[0xFF42] }; //R/W scroll y
		uint8_t &SCX{ _a[0xFF43] };	//R/W scroll x

		uint8_t &LY{ _a[0xFF44] }; //R LCDC Y-Coordinate
		uint8_t &LYC{ _a[0xFF45] }; //R/W LY Compare
		uint8_t &DMA{ _a[0xFF46] }; //W DMA Transfer and Start Address 

		uint8_t &BGP{ _a[0xFF47] }; //R/W BG & Window Palette Data
		uint8_t &OBP0{ _a[0xFF48] }; //R/W Object Palette 0 Data
		uint8_t &OBP1{ _a[0xFF49] }; //R/W Object Palette 1 Data

		uint8_t &WY{ _a[0xFF4A] }; //R/W Window Y Position
		uint8_t &WX{ _a[0xFF4B] }; //R/W Window X Position

		//cbg
		uint8_t &KEY1{ _a[0xFF4D] }; //X cpu speed
		uint8_t &VBK{ _a[0xFF4F] }; //X vram bank

		uint8_t &HDMA1{ _a[0xFF51] }; //X HDMA1
		uint8_t &HDMA2{ _a[0xFF52] }; //X HDMA2
		uint8_t &HDMA3{ _a[0xFF53] }; //X HDMA3
		uint8_t &HDMA4{ _a[0xFF54] }; //X HDMA4
		uint8_t &HDMA5{ _a[0xFF55] }; //X HDMA5

		uint8_t &RP{ _a[0xFF56] }; //X infrared

		uint8_t &BCPS{ _a[0xFF68] }; //X palette BG write spec
		uint8_t &BCPD{ _a[0xFF69] }; //X palette BG write data

		uint8_t &OCPS{ _a[0xFF6A] }; //X palette OBJ write spec
		uint8_t &OCPD{ _a[0xFF6B] }; //X palette OBJ write data

		uint8_t &SVBK{ _a[0xFF70] }; //X WRAM bank spec


		uint8_t &IE{ _a[0xFFFF] }; //R/W interrupt enable

	};


};