#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>
#include "tile_info.hpp"

namespace yagbe
{
	class spritemap
	{
	public:

		spritemap(memory &m) : _m(m)
		{

		}

		color pixel_at_point(const ipoint& p)
		{
			return{ 0,0,0,0 };
		}
	protected:
		struct sprite_info
		{
			uint8_t sprite_y;
			uint8_t sprite_x;

			uint8_t tile_index;

			uint8_t flags;

			int screen_x() { return (int)sprite_x - 8; }
			int screen_y() { return (int)sprite_y - 16; }

			uint8_t palette_index() const
			{
				return is_bit_set( flags, 4);
			}

			bool is_flipped_x() const
			{
				return is_bit_set(flags, 5);
			}

			bool is_flipped_y() const
			{
				return is_bit_set(flags, 6);
			}

			bool below_bg() const
			{
				return is_bit_set(flags, 7);
			}

		};

		template<typename T>
		void iterate_over_sprites(T&& t)
		{
			auto oam_address = 0xFE00;
			sprite_info* oam = (sprite_info*) _m.raw_pointer_at(oam_address);

			for (int i = 0; i < 40; i++)
				t(i, *(oam+i));
		}

		std::vector<sprite_info*> gather_all_visible_sprites_in_line(int y)
		{
			std::vector<sprite_info*> ret;
			auto g = [&](int i, sprite_info& info) 
			{
				if (info.screen_y() > y)
					return;
				if (info.screen_y() <= y-4)
					return;
				ret.push_back(&info);
			};

			iterate_over_sprites(g);

			return ret;
		}
	


		memory &_m;
	};


};