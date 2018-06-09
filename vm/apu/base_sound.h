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
			if (!_enabled) return;
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
	};
};