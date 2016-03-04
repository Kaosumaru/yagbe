#pragma once
#include <cstdint>
#include <array>



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
		uint8_t &SB{ _a[0xFF01] }; //R/W Serial transfer data
		uint8_t &SC{ _a[0xFF02] }; //R/W SIO control

		//timers NYI
		uint8_t &DIV{ _a[0xFF04] }; //R/W Divider Register
		uint8_t &TIMA{ _a[0xFF05] }; //R/W Timer counter
		uint8_t &TMA{ _a[0xFF06] }; //R/W timer modulo
		uint8_t &TAC{ _a[0xFF07] }; //R/W timer control

		//interrupts
		uint8_t &IF{ _a[0xFF0F] }; //R/W Interrupt Flag

		//FF10-FF40 - music


		uint8_t &LCDC{ _a[0xFF40] }; //X R/W LCD Control
		uint8_t &STAT{ _a[0xFF41] }; //X R/W LCDC Status

		uint8_t &SCY{ _a[0xFF42] }; //R/W scroll y
		uint8_t &SCX{ _a[0xFF43] };	//R/W scroll x

		uint8_t &LY{ _a[0xFF44] }; //X R LCDC Y-Coordinate
		uint8_t &LYC{ _a[0xFF45] }; // X R/W LY Compare
		uint8_t &DMA{ _a[0xFF46] }; // X W DMA Transfer and Start Address 

		uint8_t &BGP{ _a[0xFF47] }; //X R/W BG & Window Palette Data
		uint8_t &OBP0{ _a[0xFF48] }; //X R/W Object Palette 0 Data
		uint8_t &OBP1{ _a[0xFF49] }; //X R/W Object Palette 1 Data

		uint8_t &WY{ _a[0xFF4A] }; //X R/W Window Y Position
		uint8_t &WX{ _a[0xFF4B] }; //X R/W Window X Position

		uint8_t &IE{ _a[0xFFFF] }; //X R/W interrupt enable

	};


};