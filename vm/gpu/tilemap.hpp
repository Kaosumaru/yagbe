#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"

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
			uint8_t tile_index = _m.raw_at(current_tileset_address() + offset);

			//shows tiles as blocks
			//uint8_t c = 10 * tile_index;
			//return{ c,c,c,255 };


			return color_of_tile_index_pixel(tile_index, (p.x + off.x) % tile_size().x, (p.y + off.y) % tile_size().y);
		}

		ipoint offset()
		{
			return { 0,0 }; //TODO
		}
	protected:
		uint16_t current_tileset_address()
		{
			return true ? 0x9800 : 0x9C00; //TODO
		}

		uint8_t* tile_at_index(uint8_t i)
		{
			const uint16_t tile_size = 2 * 8;
			int tileset_index = 1; //TODO

			uint16_t tile_address;
			if (tileset_index == 0)
				tile_address = 0x9000 + (int16_t)((int8_t)i)*tile_size;
			else
				tile_address = 0x8000 + i*tile_size;

			auto &sprite_bytes = _m.raw_at(tile_address);
			return &sprite_bytes;
		}

		color color_of_tile_index_pixel(uint8_t i, int x, int y)
		{
			auto data = tile_at_index(i);
			data += y * 2;

			x = 7 - x;
			auto d1 = *data;
			auto d2 = *(data+1);

			auto b1 = (d1 >> x) & 1;
			auto b2 = (d2 >> x) & 1;

			//TODO check hi/low
			auto c = b1 << 1 | b2;

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