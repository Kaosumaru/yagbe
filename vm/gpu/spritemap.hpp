#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>
#include <algorithm>
#include "tile_info.hpp"
#include "palette.hpp"
#include "tilemap.hpp"

namespace yagbe
{
	class spritemap
	{
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
				return is_bit_set(flags, 4);
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

		using sprite_list = std::vector<sprite_info*>;

	public:

		spritemap(memory &m, tilemap& tm) : _m(m), _tm(tm)
		{

		}

		void render_scanline(yagbe::color* line, int line_index, int width)
		{
			if (!_m.io_register.LCDC_sprites)
				return;

			auto sprites = gather_all_visible_sprites_in_line(line_index, width);

			if (sprites.empty())
				return;

			for (int x = 0; x < width; x++)
			{
				auto &pixel = line[x];
				fill_pixel_at(x, line_index, pixel, sprites);
			}
		}

	protected:
		int get_sprite_height()
		{
			return _m.io_register.LCDC_sprites_size ? 16 : 8;
		}

		int get_sprite_width()
		{
			return 8;
		}

		//check if v is in range [s, s+w)
		constexpr static bool in_range(int s, int w, int v)
		{
			return v >= s && v < s + w;
		}

		void fill_pixel_at(int x, int y, yagbe::color& pixel, const sprite_list& sprites)
		{
			auto sprite_width = get_sprite_width();
			auto sprite_height = get_sprite_height();

			auto finder = [&](sprite_info* s)
			{
				return in_range(s->screen_x(), sprite_width, x);
			};


			auto it = std::find_if(sprites.begin(), sprites.end(), finder);


			while (true)
			{
				if (it == sprites.end())
					return;

				auto &sprite = **it;

				//ok, we found corresponding sprite, now few checks...
				if (get_sprite_height() != 8)
					throw std::runtime_error("NYI");

				if (sprite.below_bg())
				{
					if (_tm.palette_at_point({ x, y }) != 0)
						return;
				}

				auto tile = tile_at_index(sprite.tile_index);

				int tile_x = x - sprite.screen_x();
				int tile_y = y - sprite.screen_y();

				if (sprite.is_flipped_x())
					tile_x = 7 - tile_x;
				if (sprite.is_flipped_y())
					tile_y = 7 - tile_y;

				auto palette_index = tile->palette_index_at(tile_x, tile_y);
				if (palette_index == 0)
				{
					it++; //this sprite is transparent here, maybe next will fit
					it = std::find_if(it, sprites.end(), finder);
					continue;
				}
					
				pixel = color_of_index(sprite, palette_index);
				return;;
			}
		}

		color color_of_index(sprite_info& sprite, uint8_t i)
		{
			auto raw_palette = sprite.palette_index() == 0 ? _m.io_register.OBP0 : _m.io_register.OBP1;
			return sprite_palette_entry{ raw_palette }.color(i);
		}


		template<typename T>
		void iterate_over_sprites(T&& t)
		{
			auto oam_address = 0xFE00;
			sprite_info* oam = (sprite_info*) _m.raw_pointer_at(oam_address);

			for (int i = 0; i < 40; i++)
				t(*(oam+i));
		}

		std::vector<sprite_info*> gather_all_visible_sprites_in_line(int y, int screen_width)
		{
			auto sprite_width = get_sprite_width();
			auto sprite_height = get_sprite_height();

			std::vector<sprite_info*> ret;
			auto g = [&](sprite_info& info) 
			{
				if (!in_range(info.screen_y(), sprite_height, y))
					return;

				//TODO gather only first 10 sprites

				if (info.screen_x() >= screen_width)
					return;
				if (info.screen_x() <= -sprite_width)
					return;

				ret.push_back(&info);
			};

			iterate_over_sprites(g);

			std::sort(ret.begin(), ret.end(), [](sprite_info* i1, sprite_info* i2)
			{
				return std::make_pair(i1->screen_x(), i1) < std::make_pair(i2->screen_x(), i2);
			});

			return ret;
		}
	

		tile_info *tile_at_index(uint8_t i)
		{
			return  (tile_info*)_m.raw_pointer_at(0x8000) + i;
		}


		memory&  _m;
		tilemap& _tm;
	};


};