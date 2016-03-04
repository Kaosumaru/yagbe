#pragma once
#include <cstdint>
#include <array>
#include "vm/memory.hpp"
#include "vm/interrupts.hpp"

namespace yagbe
{

	class gpu_base
	{
	public:
		gpu_base(memory &m, interrupts &i) : _m(m), _i(i) {}
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
			delta /= 4; //convert to m
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


		void reset()
		{
			_line = 0;
			_mode = mode::scanline_OAM;
			_clock = 0;
		}
	protected:
		void set_line(int l)
		{
			_line = l;
			_m.io_register.LY = l;
		}

		virtual void push_screen()
		{
			//push buffer to a renderer
		}

		virtual void render_scanline()
		{
			//render scanline to framebuffer
		}

		auto line()
		{
			return _line;
		}


	private:
		using function_pointer = gpu_base::mode (gpu_base::*)();

		mode end_horizontal_blank()
		{
			set_line(_line+1);
			if (_line == 143)
			{
				// Enter vblank
				_i.vblank();
				push_screen();
				return mode::vertical_blank;
			}
			return mode::scanline_OAM;
		}

		mode end_vertical_blank()
		{
			// Vblank (10 lines)
			set_line(_line + 1);
			if (_line > 153) 
			{ 
				// Restart scanning modes
				set_line(0);
				return  mode::scanline_OAM;
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
	protected:
		interrupts &_i;
		memory &_m;
	};
};