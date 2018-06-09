#pragma once
#include "vm/apu/base_sound.h"

namespace yagbe
{
	class square_sound : public base_sound
	{
		class square_generator
		{
		public:

			void set_frequency(double freq)
			{
				_frequency = freq;
				_cycleDuration[0] = 1.0 / freq / 2.0;
				_cycleDuration[1] = 1.0 / freq / 2.0;
			}

			float time_step(double delta)
			{
				static float cycleOut[2] = {1.0f, -1.0f};
				auto &currentCycleDuration = _cycleDuration[_cycle];

				_accTime += delta;
				if (_accTime < currentCycleDuration)
					return cycleOut[_cycle];
				
				_accTime -= currentCycleDuration;

				_cycle++;
				_cycle %= 2;

				return cycleOut[_cycle];
			}

			void reset()
			{
				_accTime = 0.0f;
			}

		protected:
			int    _cycle;
			double _accTime = 0.0f;
			double _cycleDuration[2] = {1.0, 1.0};
			double _frequency;
		};

	public:
		square_sound()
		{
			_generator.set_frequency(2500);
		}

		void reset()
		{
			_generator.reset();
		}

		void time_step(double delta, channels_type &channels)
		{
			auto sample = _generator.time_step(delta);
			apply_sample_to_output(sample, channels);
		}

		square_generator _generator;
	};
};