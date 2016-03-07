#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>
#include "tile_info.hpp"
#include "palette.hpp"


namespace yagbe
{
	class tilemap
	{
	public:
		constexpr static ipoint tilemap_size() { return { 32, 32 }; }
		constexpr static ipoint tile_size() { return{ 8, 8 }; }
		constexpr static ipoint tilemap_pixel_size() { return{ tilemap_size().x*tile_size().x, tilemap_size().y*tile_size().y }; }

		tilemap(memory &m, bool window = false) : _m(m), _window_mode(window)
		{
			if (!window)
				_parent_tilemap.reset(new tilemap(_m, true));
		}

		int palette_at_point(const ipoint& p)
		{
			auto off = offset();
			int pix_x = p.x + off.x;
			int pix_y = p.y + off.y;
			
			int tile_x = (pix_x / tile_size().x);
			int tile_y = (pix_y / tile_size().y);

			if (is_wrapping())
			{
				tile_x %= tilemap_size().x;
				tile_y %= tilemap_size().y;
			}
			else
			{
				if (pix_x < 0 || tile_x >= tilemap_size().x)
					return - 1;
				if (pix_y < 0 || tile_y >= tilemap_size().y)
					return - 1;
			}


			int offset = tile_x + tile_y * tilemap_size().x;
			uint8_t tile_index = _m.raw_at(current_tilemap_address() + offset);

			//shows tiles as blocks
			//uint8_t c = 10 * tile_index;
			//return{ c,c,c,255 };

			auto info = tile_at_index(tile_index);
			auto c = info->palette_index_at(pix_x % tile_size().x, pix_y % tile_size().y);
			return c;
		}

		bool render_scanline(yagbe::color* line, int line_index, int width)
		{
			if (!enabled())
				return false;

			for (int i = 0; i < width; i++)
			{
				if (_parent_tilemap && _parent_tilemap->enabled())
				{
					auto palette_index = _parent_tilemap->palette_at_point({ i, line_index });
					if (palette_index != -1)
					{
						line[i] = color_of_index(palette_index);
						continue;
					}
				}

				auto palette_index = palette_at_point({ i, line_index });
				if (palette_index != -1)
					line[i] = color_of_index(palette_index);
			}

			return true;
		}

	public:

		//those are different for bg, and different for window
		bool enabled()
		{
			return !_window_mode ? (bool)_m.io_register.LCDC_background : (bool)_m.io_register.LCDC_window;
		}

		ipoint offset()
		{
			if (_window_mode)
				return{ -(_m.io_register.WX-7), -_m.io_register.WY };
			return{ _m.io_register.SCX, _m.io_register.SCY };
		}

		uint16_t current_tilemap_address()
		{
			if (_window_mode)
				return _m.io_register.LCDC_window_tile_map ? 0x9C00 : 0x9800;

			return _m.io_register.LCDC_background_tile_map ? 0x9C00 : 0x9800;
		}

		color color_of_index(uint8_t i)
		{
			return palette_entry{ _m.io_register.BGP }.color(i);
		}

		int tileset_index()
		{
			return _m.io_register.LCDC_background_tile_set ? 1 : 0;
		}

		bool is_wrapping()
		{
			return !_window_mode;
		}

	protected:
		tile_info *tile_at_index(uint8_t i)
		{
			tile_info *info;

			if (tileset_index() == 0)
			{
				info = (tile_info*)_m.raw_pointer_at(0x9000);
				info += (int8_t)i; //this tileset is using signed coords
			}
			else
			{
				info = (tile_info*)_m.raw_pointer_at(0x8000);
				info += i; //this tileset is using unsigned coords
			}


			return info;
		}

		std::unique_ptr<tilemap> _parent_tilemap;
		memory &_m;
		bool _window_mode = false;
	};


};