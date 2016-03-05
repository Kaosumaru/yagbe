#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "gpu_base.hpp"
#include "tilemap.hpp"
#include "spritemap.hpp"
#include "vm/utils.hpp"

namespace yagbe
{
	class gpu : public gpu_base
	{
	public:
		constexpr static ipoint screen_size() { return{ 160, 144 }; }

		gpu(memory &m, interrupts &i) : gpu_base(m, i) {}

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

			auto line_address = _buffer.data() + y * screen_size().x;

			_tilemap.render_scanline(line_address, y, screen_size().x);
			_spritemap.render_scanline(line_address, y, screen_size().x);

		}


		
		tilemap _tilemap     { _m };
		spritemap _spritemap { _m };
		std::vector<color> _buffer { (std::size_t) screen_size().x * screen_size().y };
	};


};