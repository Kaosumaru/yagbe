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
					shift_noise();
				}

				//bool b = rand() % 2;
				bool b = _hiddenNoise & 1;

				return b ? -1.0f : 1.0f;
			}

			void shift_noise()
			{
				auto set = [](auto& p, int v, int n)
				{
					p ^= (-v ^ p) & (1 << n);
				};

				auto get = [](auto p, int n)
				{
					return (p >> n) & 1;
				};

				int x = get(_hiddenNoise, 0) ^ get(_hiddenNoise, 1);
				set(_hiddenNoise, x, 15);

				if (_control.poly_width)
				{
					//read about LFSR, this was suggested by the guy on nesdev, but seems wrong
					//int x = get(_hiddenNoise, 0) ^ get(_hiddenNoise, 6);
					//set(_hiddenNoise, x, 15);
					set(_hiddenNoise, x, 6);
				}
				_hiddenNoise >>= 1;
			}

			void set_frequency()
			{
				//why this is so cryptic...
				//http://problemkaputt.de/pandocs.htm#soundcontroller
				//http://forums.nesdev.com/viewtopic.php?f=3&t=13767
				//http://gbdev.gg8.se/wiki/articles/Gameboy_sound_hardware

				double magic_frequency = 524288.0;

				double r = _control.poly_division;
				if (r == 0) r = 0.5;
				double s = 2 << (_control.poly_shift + 1);

				//524288 Hz / r / 2 ^ (s + 1);
				_frequency = magic_frequency / r / s;
				_cycleDuration = (1.0 / _frequency);
			}

			void reset()
			{
				set_frequency();
				_accTime = 0.0f;
			}

		protected:
			io_registers::AudioNoise& _control;
			uint16_t _hiddenNoise = 1;
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