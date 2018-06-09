#pragma once
#include "vm/apu/base_sound.h"

namespace yagbe
{
	class noise_sound : public base_sound
	{
		class noise_generator
		{
		public:
			noise_generator(io_registers::AudioNoise& control) : _control(control) {}

			float time_step(double delta)
			{
				if (_control.initialize)
				{
					reset();
				}


				_accTime += delta;
				while (_accTime > _cycleDuration)
				{
					_accTime -= _cycleDuration;
					//TODO
				}

				return (float)0.0f;
			}

			void set_frequency(double freq)
			{
				_frequency = freq;
				_cycleDuration = (1.0 / _frequency);
			}

			void reset()
			{
				_accTime = 0.0f;
			}

		protected:


			io_registers::AudioNoise& _control;
			double _cycleDuration = 1.0;
			double _accTime = 0.0f;
			double _frequency = 1.0;
		};


	public:
		noise_sound(io_registers::AudioNoise& control, bit e, bit l, bit r)
			: base_sound(e, l, r), 
			_control(control),
			_counter(control, e),
			_envelope(control),
			_generator(control)
		{
		}

		void reset()
		{
			_generator.reset();
		}

		channels_type time_step(double delta)
		{
			_counter.step(delta);
			auto volume = _envelope.get_volume(delta);

			if (_control.initialize)
			{
				reset();
				_enabled = true;
				_control.initialize = 0;
			}

			channels_type channels = {0, 0};
			auto sample = _generator.time_step(delta);
			sample *= volume;
			apply_sample_to_output(sample, channels);
			return channels;
		}

	protected:

		io_registers::AudioNoise& _control;
		Counter<io_registers::AudioNoise>  _counter;
		Envelope<io_registers::AudioNoise> _envelope;
		noise_generator _generator;
	};
};