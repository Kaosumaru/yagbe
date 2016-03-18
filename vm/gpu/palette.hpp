#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "vm/utils.hpp"

namespace yagbe
{
	class palette_entry
	{
	public:
		palette_entry(uint8_t entry) : _entry(entry)
		{

		}

		uint8_t color_index(int i) const
		{
			return (_entry >> i*2) & 0b11;
		}

		yagbe::color color(int i) const
		{
			return colors()[color_index(i)];
		}

		constexpr static std::array<yagbe::color, 4> black_white_colors()
		{
			return std::array<yagbe::color, 4>{
				yagbe::color{ 250,250,250,255 },
				yagbe::color{ 116,116,116,255 },
				yagbe::color{ 188,188,188,255 },
				yagbe::color{ 0,0,0,255 },
			};
		}

		constexpr static std::array<yagbe::color, 4> lcd_colors()
		{
			return std::array<yagbe::color, 4>{
				yagbe::color{ 252,232,140,255 },
				yagbe::color{ 220,180,92,255 },
				yagbe::color{ 152,124,60,255 },
				yagbe::color{ 76,60,28,255 },
			};
		}

		constexpr static std::array<yagbe::color, 4> colors()
		{
			return lcd_colors();
		}
	protected:
		uint8_t _entry;
	};


};