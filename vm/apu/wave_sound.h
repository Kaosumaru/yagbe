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
			wave_generator(io_registers::AudioWave& control, io_registers::WaveTable& table) : _control(control), _table(table) {}

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
					_cycle++;
					_cycle %= SamplesCount;
				}

				int rawSample = _table.get(_cycle);
				
				auto level = _control.output_level;
				if (level == 0) return 0.0f;
				float volumeTable[] = {0.0f, 1.0f, 0.5f, 0.25f};

				int maxValue = 0b1000;

				auto f = (double)(rawSample - maxValue) / (double)(maxValue);
				f *= volumeTable[level];
				return (float)f;
			}

			void set_frequency(double freq)
			{
				_frequency = freq;
				_cycleDuration = (1.0 / _frequency) / (double)SamplesCount;
				_cycleDuration *= 2.0;
			}

			void reset()
			{
				_cycle = 0;
				_accTime = 0.0f;
			}

		protected:


			io_registers::AudioWave& _control;
			io_registers::WaveTable& _table;
			int    _cycle = 0;
			double _accTime = 0.0f;
			double _cycleDuration = 1.0;
			double _frequency = 1.0;
		};

	public:
		wave_sound(io_registers::AudioWave& control, bit e, bit l, bit r, io_registers::WaveTable& table)
			: base_sound(e, l, r), 
			_control(control),
			_counter(control, e),
			_frequency(control),
			_generator(control, table)
		{
			_maxVolume = 1.0f;
		}

		void reset()
		{
			
		}

		channels_type time_step(double delta)
		{
			_frequency.get([&](double f) {  _generator.set_frequency(f); });
			_counter.step(delta);
			auto volume = 1.0;

			if (_control.initialize)
			{
				reset();
				_enabled = true;
				_control.initialize = 0;
			}

			channels_type channels = {0, 0};
			if (!_control.enabled) return channels;

			auto sample = _generator.time_step(delta);
			apply_sample_to_output(sample, channels);
			return channels;
		}

	protected:

		io_registers::AudioWave& _control;
		Counter<io_registers::AudioWave>   _counter;
		Frequency<io_registers::AudioWave> _frequency;
		wave_generator                     _generator;
	};
};