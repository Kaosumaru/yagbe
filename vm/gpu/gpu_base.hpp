#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"

namespace yagbe
{

	class gpu_base
	{
	public:
		gpu_base(memory &m) : _m(m) {}
		virtual ~gpu_base(){}

		enum class mode
		{
			horizontal_blank,
			vertical_blank,
			scanline_OAM,
			scanline_VRAM,
		};

		constexpr static std::array<int,4> mode_durations() { return{ 204, 456, 80, 172 }; }

		void step(uint32_t delta)
		{
			if (delta > 80)
				throw std::out_of_range("delta is too large");

			auto &current_mode_duration = mode_durations()[(int)_mode];
			_clock += delta;

			if (_clock < current_mode_duration)
				return;

			_clock -= current_mode_duration;

			auto call_ptr = mode_end_functions()[(int)_mode];
			_mode = (this->*call_ptr)();
		}

	protected:
		virtual void push_screen()
		{
			//push buffer to a renderer
		}

		virtual void render_scanline()
		{
			//render scanline to framebuffer
		}


	private:
		using function_pointer = gpu_base::mode (gpu_base::*)();

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


		constexpr static std::array<function_pointer, 4> mode_end_functions() { return{ &gpu_base::end_horizontal_blank, &gpu_base::end_vertical_blank, &gpu_base::end_scanline_OAM, &gpu_base::end_scanline_VRAM }; }


		int _line = 0;
		mode _mode = mode::scanline_OAM;
		uint32_t _clock = 0;
		memory &_m;
	};
};