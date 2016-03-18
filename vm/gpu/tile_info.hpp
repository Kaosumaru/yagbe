#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>

namespace yagbe
{
	struct tile_info
	{
		memory::ByteWord rows[8];

		int palette_index_at(int x, int y)
		{
			auto row = rows[y];

			x = 7 - x;
			auto b1 = bit{ row.byte[0], x } ? 1 : 0;
			auto b2 = bit{ row.byte[1], x } ? 1 : 0;

			return b1 | b2 << 1;
		}
	};
};