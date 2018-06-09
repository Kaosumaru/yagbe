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
				_cycleDuration[0] = (1.0 / _frequency) * _waveForm[0];
				_cycleDuration[1] = (1.0 / _frequency) * _waveForm[1];
			}

			void set_waveform(double w1, double w2)
			{
				_waveForm[0] = w1;
				_waveForm[1] = w2;
				set_frequency(_frequency);
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
			double _waveForm[2] = {.5, .5};
			double _cycleDuration[2] = {1.0, 1.0};
			double _frequency;
		};

	public:
		square_sound(io_registers::AudioSquare& squareControl, bit e, bit l, bit r, bool sweepEnabled) : base_sound(e, l, r), _squareControl(squareControl), _sweepEnabled(sweepEnabled)
		{
			int x = sizeof(io_registers::AudioSquare);
			_generator.set_frequency(5000);
		}

		void reset()
		{
			_generator.reset();
		}

		channels_type time_step(double delta)
		{
			// Overview:
			// This channel creates square wave, with given frequency (_squareControl.lfrequency/_squareControl.hfrequency)
			// You can also change waveform (_squareControl.waveform_duty).
			// It have own timer (calculate_counter) which can disable sound after certain amount of time.
			// It also have sweep (only sound1) & envelope function.
			// envelope function can set volume, and optionally modify (increase/decrease) it in time.

			channels_type channels;
			if (_squareControl.initialize)
			{
				reset();
				_enabled = true;
				_soundLength = (64.0 - _squareControl.sound_length) * (1.0 / 256.0);
				_envelopeVolume = _squareControl.envelope_default_volume;
				_envAcc = 0.0;
				_sweepAcc = 0.0;
				_squareControl.initialize = 0;
			}

			calculate_frequency();
			calculate_sweep(delta);
			calculate_counter(delta);
			calculate_waveform();
			auto volume = calculate_envelope_volume(delta);

			auto sample = _generator.time_step(delta);
			sample *= volume;
			apply_sample_to_output(sample, channels);
			return channels;
		}

	protected:
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
		int _frequency = 0;

		void calculate_sweep(double delta)
		{
			//TODO not sure that counter may be changed without sending initialize

			if (!_sweepEnabled) return;
			if (_squareControl.sweep_time == 0)
			{
				_sweepAcc = 0.0;
				return;
			}
			int add = _squareControl.sweep_negate ? -1 : 1;

			double sweepTime = (double)_squareControl.sweep_time * (1.0/128.0);
			_sweepAcc += sweepTime;

			while (_sweepAcc > sweepTime)
			{
				_sweepAcc -= sweepTime;
			}
			// NYI
		}
		bool _sweepEnabled = false;
		double _sweepAcc = 0.0;

		void calculate_counter(double delta)
		{
			// Counter / Continous Selection	
			//   0:  Outputs continuous sound regardless of length data in register NR21 (sound_length). 
			//   1 : Outputs sound for the duration specified by the length data in register NR21 (sound_length). 
			//       When sound output is finished, bit 1 of register NR52, the Sound X ON flag, is reset.

			//TODO not sure that counter may be changed without sending initialize

			if (!_squareControl.counter_enabled) return;
			_soundLength -= delta;
			if (_soundLength <= 0.0)
			{
				_soundLength = 0.0f;
				_enabled = false;
			}
		}
		double _soundLength = 0.0;


		void calculate_waveform()
		{
			double waveForms[] = { 0.125, 0.25, 0.5, 0.75 };
			if (_currentWaveForm == _squareControl.waveform_duty) return;
			_currentWaveForm = _squareControl.waveform_duty;
			auto l = waveForms[_currentWaveForm];
			_generator.set_waveform(l, 1.0 - l);
		}
		int _currentWaveForm = 2;


		float calculate_envelope_volume(double delta)
		{
			// "If the contents of the envelope register (NR12) needs to be changed during sound operation (ON flag set to 1), the initialize flag should be set after the value in the envelope register is set."
			// suggests that initialize is necessary to change envelope volume
			float volume = 1.0f;
			if (_squareControl.envelope_length == 0)
			{
				_envAcc = 0.0;
				volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;
				return volume;
			}

			auto envLength = _squareControl.envelope_length * (1.0 / 64.0);
			_envAcc += delta;
			while (_envAcc > envLength)
			{
				_envAcc -= envLength;

				// apply envelope effect
				int d = _squareControl.envelope_add_mode ? 1 : -1;
				_envelopeVolume += d;

				if (_envelopeVolume > _maxEnvelopeVolume) _envelopeVolume = _maxEnvelopeVolume;
				if (_envelopeVolume < 0) _envelopeVolume = 0;
			}
			if (_envAcc < 0.0) _envAcc = 0.0;

			volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;
			return volume;
		}
		int _envelopeVolume = 0;
		int _maxEnvelopeVolume = 0b1111;
		double _envAcc = 0.0;


		
		io_registers::AudioSquare& _squareControl;
		square_generator _generator;
	};
};