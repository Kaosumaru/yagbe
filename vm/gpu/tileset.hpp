#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"

namespace yagbe
{
	class tileset
	{
	public:
		constexpr static ipoint tilemap_size() { return { 32, 32 }; }

		tileset(memory &m) : _m(m)
		{

		}

		color pixel_at_point(const ipoint& p)
		{
			auto tile = tile_at_point(p);

			uint8_t c = 10 * tile;
			return { c,c,c,255};
		}

		uint8_t tile_at_point(const ipoint& p)
		{
			auto off = offset();
			int x = ((p.x + off.x) / tilemap_size().x) % tilemap_size().x;
			int y = ((p.y + off.y) / tilemap_size().y) % tilemap_size().y;


			int offset = x + y * tilemap_size().x;
			return _m.raw_at(current_tileset_address() + offset);
		}

		ipoint offset()
		{
			return { 0,0 };
		}
	protected:
		uint16_t current_tileset_address()
		{
			return true ? 0x9800 : 0x9C00;
		}

		uint8_t* tile_at_index(uint8_t i)
		{
			//auto address = 
		}


		memory &_m;
	};


};