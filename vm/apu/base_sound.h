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

		void apply_sample_to_output(float sample, channels_type &channels)
		{
			sample *= _maxVolume;

			channels[0] = sample;
			channels[1] = sample;
		}

	protected:
		float _maxVolume = 0.25f;
	};
};