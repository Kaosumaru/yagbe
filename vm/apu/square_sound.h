#pragma once
#include "vm/apu/base_sound.h"

namespace yagbe
{
	class square_sound : public base_sound
	{
		class square_generator
		{
			using Control = io_registers::AudioSquare;
		public:
			square_generator(io_registers::AudioSquare& control) : _control(control) {}

			float time_step(double delta)
			{
				set_gb_waveform(_control.waveform_duty);
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

			void set_frequency(double freq)
			{
				_frequency = freq;
				_cycleDuration[0] = (1.0 / _frequency) * _waveForm[0];
				_cycleDuration[1] = (1.0 / _frequency) * _waveForm[1];
			}

			void reset()
			{
				_accTime = 0.0f;
			}

		protected:
			void set_waveform(double w1, double w2)
			{
				_waveForm[0] = w1;
				_waveForm[1] = w2;
				set_frequency(_frequency);
			}

			void set_gb_waveform(int i)
			{
				double waveForms[] = {0.125, 0.25, 0.5, 0.75};
				if (_currentWaveForm == i) return;
				_currentWaveForm = i;
				auto l = waveForms[i];
				set_waveform(l, 1.0 - l);
			}

			io_registers::AudioSquare& _control;
			int    _cycle;
			double _accTime = 0.0f;
			double _waveForm[2] = {.5, .5};
			double _cycleDuration[2] = {1.0, 1.0};
			double _frequency;
			int    _currentWaveForm = 2;
		};

	public:
		square_sound(io_registers::AudioSquare& squareControl, bit e, bit l, bit r, bool sweepEnabled) 
			: base_sound(e, l, r), 
			_squareControl(squareControl),
			_sweepEnabled(sweepEnabled),
			_counter(squareControl, e),
			_envelope(squareControl),
			_frequency(squareControl),
			_generator(squareControl)
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

			_frequency.get([&](double f) {_generator.set_frequency(f); });
			calculate_sweep(delta);
			_counter.step(delta);
			auto volume = _envelope.get_volume(delta);

			if (_squareControl.initialize)
			{
				reset();
				_enabled = true;
				_squareControl.initialize = 0;
			}

			channels_type channels = {0, 0};
			auto sample = _generator.time_step(delta);
			sample *= volume;
			apply_sample_to_output(sample, channels);
			return channels;
		}

	protected:
		// sweep
		void calculate_sweep(double delta)
		{
			//TODO not sure that counter may be changed without sending initialize
			if (_squareControl.initialize)
			{
				_sweepAcc = 0.0;
			}

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
		

		Counter<io_registers::AudioSquare>  _counter;
		Envelope<io_registers::AudioSquare> _envelope;
		Frequency<io_registers::AudioSquare> _frequency;
		square_generator _generator;

		io_registers::AudioSquare& _squareControl;
		
	};
};