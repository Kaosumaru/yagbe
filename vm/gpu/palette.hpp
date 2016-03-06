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


		constexpr static std::array<yagbe::color, 4> colors()
		{ 
			return std::array<yagbe::color, 4>{
				yagbe::color{ 255,255,255,255 },
				yagbe::color{ 192,192,192,255 },
				yagbe::color{ 96,96,96,255 },
				yagbe::color{ 0,0,0,255 },
			};
		}
	protected:
		uint8_t _entry;
	};


};