#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "vm/memory.hpp"


namespace yagbe
{
	class base_sound
	{
	public:
		using channels_type = std::array<float, 2>;
		base_sound(bit enabled, bit left_enabled, bit right_enabled) :
			_enabled(enabled), _left_enabled(left_enabled), _right_enabled(right_enabled)
		{

		}

		void apply_sample_to_output(float sample, channels_type &channels)
		{
			if (!_enabled) return; //TODO probably should pause everything in sound circuit
			sample *= _maxVolume;
			if (_left_enabled)
				channels[1] = sample;
			if (_right_enabled)
				channels[0] = sample;
		}

	protected:
		bit _enabled;
		bit _left_enabled;
		bit _right_enabled;

		float _maxVolume = 0.25f;

		// functions for circuits

		template<typename Control>
		class Counter
		{
			Control& _control;
			bit _enabled;
			double _soundLength = 0.0;
		public:
			Counter(Control& control, bit enabled) : _control(control), _enabled(enabled) {}

			void initialize()
			{
				_soundLength = (64.0 - _control.sound_length) * (1.0 / 256.0);
			}

			void step(double delta)
			{
				// Counter / Continous Selection	
				//   0:  Outputs continuous sound regardless of length data in register NR21 (sound_length). 
				//   1 : Outputs sound for the duration specified by the length data in register NR21 (sound_length). 
				//       When sound output is finished, bit 1 of register NR52, the Sound X ON flag, is reset.

				//TODO not sure that counter may be changed without sending initialize
				if (_control.initialize) initialize();

				if (!_control.counter_enabled) return;
				_soundLength -= delta;
				if (_soundLength <= 0.0)
				{
					_soundLength = 0.0f;
					_enabled = false;
				}
			}
		};

		template<typename Control>
		class Envelope
		{
			Control& _control;
			int _envelopeVolume = 0;
			int _maxEnvelopeVolume = 0b1111;
			double _envAcc = 0.0;
		public:
			Envelope(Control& control) : _control(control) {}

			void initialize()
			{
				_envelopeVolume = _control.envelope_default_volume;
				_envAcc = 0.0;
			}

			float get_volume(double delta)
			{
				if (_control.initialize) initialize();

				// "If the contents of the envelope register (NR12) needs to be changed during sound operation (ON flag set to 1), the initialize flag should be set after the value in the envelope register is set."
				// suggests that initialize is necessary to change envelope volume
				float volume = 1.0f;
				if (_control.envelope_length == 0)
				{
					_envAcc = 0.0;
					volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;
					return volume;
				}

				auto envLength = _control.envelope_length * (1.0 / 64.0);
				_envAcc += delta;
				while (_envAcc > envLength)
				{
					_envAcc -= envLength;

					// apply envelope effect
					int d = _control.envelope_add_mode ? 1 : -1;
					_envelopeVolume += d;

					if (_envelopeVolume > _maxEnvelopeVolume) _envelopeVolume = _maxEnvelopeVolume;
					if (_envelopeVolume < 0) _envelopeVolume = 0;
				}
				if (_envAcc < 0.0) _envAcc = 0.0;

				volume = (float)_envelopeVolume / (float)_maxEnvelopeVolume;
				return volume;
			}
		};

		template<typename Control>
		class Frequency
		{
			Control& _control;
			int _frequency = 0;
		public:
			Frequency(Control& control) : _control(control) {}

			template<typename T>
			void get(T&& callback)
			{
				// f = 4194304 / (4 x 2^3 x (2048 - X)) Hz
				// X = 0<->2047
				// 0 = 64
				// 2047 = 131072
				// Thus, the minimum frequency is 64 Hz and the maximum is 131.1 KHz.

				if (_control.initialize) return;

				auto controlFreq = _control.lfrequency + (_control.hfrequency << 8);
				if (controlFreq == _frequency) return;
				_frequency = controlFreq;
				auto f = 4194304.0 / (32.0 * (2048.0 - (double)_frequency));
				callback(f);
			}
		};
	};
};