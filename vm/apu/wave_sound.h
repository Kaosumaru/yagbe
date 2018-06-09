#pragma once
#include "vm/apu/base_sound.h"

namespace yagbe
{
	class wave_sound : public base_sound
	{

	public:
		wave_sound(io_registers::AudioWave& control, bit e, bit l, bit r)
			: base_sound(e, l, r), 
			_control(control),
			_counter(control, e),
			_frequency(control)
		{

		}

		void reset()
		{
			
		}

		channels_type time_step(double delta)
		{


			_frequency.get([&](double f) { });
			_counter.step(delta);
			auto volume = 1.0;

			if (_control.initialize)
			{
				reset();
				_enabled = true;
				_control.initialize = 0;
			}

			channels_type channels = {0, 0};

			return channels;
		}

	protected:

		io_registers::AudioWave& _control;
		Counter<io_registers::AudioWave>  _counter;
		Frequency<io_registers::AudioWave> _frequency;
	};
};