#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "vm/memory.hpp"
#include "vm/interrupts.hpp"
#include "vm/apu/square_sound.h"

namespace yagbe
{
	class apu
	{
	public:
		apu(memory &m) : _m(m) 
		{
			configure(48000, 100);
		}

		void configure(int sampleRate, int samples)
		{
			_maxSamples = samples;
			_outSampleRate = sampleRate;
			_sampleDuration = 1.0 / (double)_outSampleRate;
		}

		void step(uint32_t delta)
		{
			//TODO
			//4194304 / 48000 is about 87 cycles, we could use this
			auto t = delta * _m.length_of_cycle();
			_elapsedTime += t;
			if (_elapsedTime < _sampleDuration) return;
			_elapsedTime -= _sampleDuration;
			

			auto sample = generate_sample();
			for (auto channel : sample)
				_samples.push_back(channel * _masterVolume);

			if (_samples.size() < _maxSamples * sample.size()) return;

			if (onSamplesReady)
				onSamplesReady(std::move(_samples));
			_samples.clear();
		}

		base_sound::channels_type generate_sample()
		{
			base_sound::channels_type channels {0.0, 0.0};
			if (!_m.io_register.AUDIO_all_enabled) return channels;

			auto mixSound = [&](auto& sound)
			{
				auto sample = _sound1.time_step(_sampleDuration);
				channels[0] += sample[0];
				channels[1] += sample[1];
			};

			mixSound(_sound1);
			//mixSound(_sound2);

			return channels;
		}

		void reset()
		{
			_sound1.reset();
		}

		std::function<void(const std::vector<float>&&)> onSamplesReady;
	protected:
		float _masterVolume = 0.5;
		double _elapsedTime = 0.0;
		int _maxSamples = 100;
		int _outSampleRate = 1;
		double _sampleDuration = 1.0;
		std::vector<float> _samples;
		memory &_m;


		square_sound _sound1 { _m.io_register.AUDIO_square1, _m.io_register.AUDIO_s1_enabled, _m.io_register.AUDIO_s1_to_so1, _m.io_register.AUDIO_s1_to_so2, true};
		square_sound _sound2 { _m.io_register.AUDIO_square2, _m.io_register.AUDIO_s2_enabled, _m.io_register.AUDIO_s2_to_so1, _m.io_register.AUDIO_s2_to_so2, false};
		//wav_sound _sound3; NYI
		//random_sound _sound4; NYI
	};


};