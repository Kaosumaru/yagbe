#pragma once
#include <cstdint>
#include <array>
#include "gpu_base.hpp"

namespace yagbe
{
	class gpu : public gpu_base
	{
	public:
		using gpu_base::gpu_base;
	protected:
		void push_screen() override
		{
			//push buffer to a renderer
		}

		void render_scanline() override
		{
			//render scanline to framebuffer
		}
	};


};