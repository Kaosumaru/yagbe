#pragma once
#include "vm/apu/base_sound.h"

namespace yagbe
{
	class wave_sound : public base_sound
	{
		class wave_generator
		{
			using Control = io_registers::AudioWave;
			static constexpr int SamplesCount = 32;
		public:
			wave_generator(io_registers::AudioWave& control) : _control(control) {}

			float time_step(double delta)
			{
				if (_control.initialize)
				{
					_cycle = 0;
				}

				static float cycleOut[2] = {1.0f, -1.0f};
				

				_accTime += delta;
				while (_accTime > _cycleDuration)
				{
					_accTime -= _cycleDuration;
					_cycle++;
					_cycle %= SamplesCount;
				}


				



				return cycleOut[_cycle];
			}

			void set_frequency(double freq)
			{
				_frequency = freq;
				_cycleDuration = (1.0 / _frequency) / (double)SamplesCount;
			}

			void reset()
			{
				_accTime = 0.0f;
			}

		protected:


			io_registers::AudioWave& _control;
			int    _cycle = 0;
			double _accTime = 0.0f;
			double _waveForm[2] = {.5, .5};
			double _cycleDuration = 1.0;
			double _frequency = 1.0;
		};

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