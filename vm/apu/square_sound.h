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
				_cycleDuration[0] = 1.0 / _frequency / 2.0;
				_cycleDuration[1] = 1.0 / _frequency / 2.0;
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
		square_sound(io_registers::AudioSquare& squareControl, bit e, bit l, bit r) : base_sound(e, l, r), _squareControl(squareControl)
		{
			int x = sizeof(io_registers::AudioSquare);
			_generator.set_frequency(5000);
		}

		void reset()
		{
			_generator.reset();
		}

		void time_step(double delta, channels_type &channels)
		{
			if (_squareControl.initialize)
			{
				reset();
				_enabled = true;
				_soundLength = (64.0 - _squareControl.sound_length) * (1.0 / 256.0);
				_envelopeVolume = _squareControl.envelope_default_volume;
				_envAcc = 0.0;
				_squareControl.initialize = 0;
			}

			calculate_frequency();
			//sweep
			calculate_counter(delta); //counter
			//waveform
			auto volume = calculate_envelope_volume(delta);

			auto sample = _generator.time_step(delta);
			sample *= volume;
			apply_sample_to_output(sample, channels);
		}


		void calculate_frequency()
		{
			// f = 4194304 / (4 x 2^3 x (2048 - X)) Hz
			// X = 0<->2047
			// 0 = 64
			// 2047 = 131072
			// Thus, the minimum frequency is 64 Hz and the maximum is 131.1 KHz.

			auto controlFreq = _squareControl.lfrequency + (_squareControl.hfrequency << 8);
			if (controlFreq == _frequency) return;
			_frequency = controlFreq;
			auto f = 4194304.0 / (32.0 * (2048.0 - (double)_frequency));
			_generator.set_frequency(f);
		}


		void calculate_counter(double delta)
		{
			// Counter / Continous Selection	
			//   0:  Outputs continuous sound regardless of length data in register NR21 (sound_length). 
			//   1 : Outputs sound for the duration specified by the length data in register NR21 (sound_length). 
			//       When sound output is finished, bit 1 of register NR52, the Sound X ON flag, is reset.

			if (!_squareControl.counter_enabled) return;
			_soundLength -= delta;
			if (_soundLength <= 0.0)
			{
				_soundLength = 0.0f;
				_enabled = false;
			}
		}

		float calculate_envelope_volume(double delta)
		{
			// TODO not sure if envelope volume can be modified always, or change will be only acknowledged after _squareControl.initialize
			// "If the contents of the envelope register (NR12) needs to be changed during sound operation (ON flag set to 1), the initialize flag should be set after the value in the envelope register is set."
			// suggests that initialize is necessary
			float volume = 1.0f;
			if (_squareControl.envelope_length == 0)
			{
				volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;
				_envAcc = 0.0;
				return volume;
			}

			auto envLength = _squareControl.envelope_length * (1.0 / 64.0);
			_envAcc += delta;
			while (_envAcc > envLength)
			{
				_envAcc -= envLength;

				//apply envelope effect
				int d = _squareControl.envelope_add_mode ? 1 : -1;
				_envelopeVolume += d;

				if (_envelopeVolume > _maxEnvelopeVolume) _envelopeVolume = _maxEnvelopeVolume;
				if (_envelopeVolume < 0) _envelopeVolume = 0;

				volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;

			}
			if (_envAcc < 0.0) _envAcc = 0.0;

			return volume;
		}

		double _soundLength = 0.0;


		int _envelopeVolume = 0;
		int _maxEnvelopeVolume = 0b1111;
		double _envAcc = 0.0;


		int _frequency = 0;
		io_registers::AudioSquare& _squareControl;
		square_generator _generator;
	};
};