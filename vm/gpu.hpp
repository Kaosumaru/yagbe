#pragma once
#include <cstdint>
#include <array>

namespace yagbe
{
	class gpu
	{
	public:

		enum class mode
		{
			horizontal_blank,
			vertical_blank,
			scanline_OAM,
			scanline_VRAM,
		};

		constexpr static int mode_durations[4] = { 204, 456, 80, 172 };

		void step(uint32_t delta)
		{
			if (delta > 80)
				throw std::out_of_range("delta is too large");

			auto &current_mode_duration = mode_durations[(int)_mode];
			_clock += delta;

			if (_clock < current_mode_duration)
				return;

			_clock -= current_mode_duration;

			auto call_ptr = mode_end_functions[(int)_mode];
			_mode = (this->*call_ptr)();
		}

	protected:
		using function_pointer = gpu::mode (gpu::*)();

		mode end_horizontal_blank()
		{
			_line++;
			if (_line == 143)
			{
				// Enter vblank
				push_screen();
				return mode::vertical_blank;
			}
			return mode::scanline_OAM;
		}

		mode end_vertical_blank()
		{
			// Vblank (10 lines)
			_line++;
			if (_line > 153) 
			{ 
				// Restart scanning modes
				_line = 0; 
				_mode = mode::scanline_OAM;
			}
			return mode::vertical_blank;
		}

		mode end_scanline_OAM()
		{
			return mode::scanline_VRAM;
		}

		mode end_scanline_VRAM()
		{
			render_scanline();
			return mode::horizontal_blank;
		}





		void push_screen()
		{
			//push buffer to a renderer
		}

		void render_scanline()
		{
			//render scanline to framebuffer
		}

		constexpr static function_pointer mode_end_functions[4] = { &gpu::end_horizontal_blank, &gpu::end_vertical_blank, &gpu::end_scanline_OAM, &gpu::end_scanline_VRAM };


		int _line = 0;
		mode _mode = mode::scanline_OAM;
		uint32_t _clock = 0;
	};
};