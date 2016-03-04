#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "gpu_base.hpp"
#include "tilemap.hpp"
#include "vm/utils.hpp"

namespace yagbe
{
	class gpu : public gpu_base
	{
	public:
		constexpr static ipoint screen_size() { return{ 160, 144 }; }

		using gpu_base::gpu_base;

		std::function<void(const std::vector<color>&)> onFrameReady;
	protected:
		void push_screen() override
		{
			//push buffer to a renderer
			if (onFrameReady)
				onFrameReady(_buffer);
		}

		void render_scanline() override
		{
			auto y = line();
			for (int x = 0; x < screen_size().x; x++)
				_buffer[x + y * screen_size().x] = render_pixel(x, y);
		}

		color render_pixel(int x, int y)
		{
			return _tilemap.pixel_at_point({ x,y });
		}


		tilemap _tilemap { _m };
		std::vector<color> _buffer { (std::size_t) screen_size().x * screen_size().y };
	};


};