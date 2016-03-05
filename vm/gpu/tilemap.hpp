#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>

namespace yagbe
{
	class tilemap
	{
	public:
		constexpr static ipoint tilemap_size() { return { 32, 32 }; }
		constexpr static ipoint tile_size() { return{ 8, 8 }; }

		tilemap(memory &m) : _m(m)
		{

		}

		color pixel_at_point(const ipoint& p)
		{
			auto off = offset();
			int x = ((p.x + off.x) / tile_size().x) % tilemap_size().x;
			int y = ((p.y + off.y) / tile_size().y) % tilemap_size().y;


			int offset = x + y * tilemap_size().x;
			uint8_t tile_index = _m.raw_at(current_tilemap_address() + offset);

			//shows tiles as blocks
			//uint8_t c = 10 * tile_index;
			//return{ c,c,c,255 };


			return color_of_tile_index_pixel(tile_index, (p.x + off.x) % tile_size().x, (p.y + off.y) % tile_size().y);
		}

		ipoint offset()
		{
			return { _m.io_register.SCX, _m.io_register.SCY };
		}
	protected:
		struct tile_info
		{
			memory::ByteWord rows[8];

			int palette_index_at(int x, int y)
			{
				auto row = rows[y];

				x = 7 - x;
				auto b1 = bit{ row.byte[0], x } ? 1 : 0;
				auto b2 = bit{ row.byte[1], x } ? 1 : 0;

				return b1 << 1 | b2;
			}
		};

		uint16_t current_tilemap_address()
		{
			return _m.io_register.LCDC_background_tile_map ? 0x9C00 : 0x9800; 
		}

		tile_info *tile_at_index(uint8_t i)
		{
			int tileset_index = _m.io_register.LCDC_background_tile_set ? 1 : 0;

			tile_info *info;

			if (tileset_index == 0)
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

		color color_of_tile_index_pixel(uint8_t i, int x, int y)
		{
			auto info = tile_at_index(i);
			auto c = info->palette_index_at(x, y);
			return color_of_index(c);

		}

		color color_of_index(uint8_t i)
		{
			i = i % 4;
			static color pallete[] = { 
				{ 0,0,0,255 },
				{ 96,96,96,255 },
				{ 192,192,192,255 },
				{ 255,255,255,255 },
			};
			return pallete[i];
		}

		memory &_m;
	};


};