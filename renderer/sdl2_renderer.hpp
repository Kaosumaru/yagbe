#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include <vector>
#include <algorithm>

#ifdef __EMSCRIPTEN__
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif


#include "vm/utils.hpp"
#include "vm/context.hpp"

namespace yagbe
{
	class sdl2_renderer
	{
	public:
		sdl2_renderer(const ipoint &size, int scale = 4) : _w(size.x), _h(size.y)
		{
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
				throw std::runtime_error("SDL_Init");

			_window.reset(SDL_CreateWindow("YAGBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _w * scale, _h * scale, SDL_WINDOW_SHOWN));
			if (!_window) throw std::runtime_error("SDL_CreateWindow");

			_renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
			if (!_renderer) throw std::runtime_error("SDL_CreateRenderer");

			_texture.reset(SDL_CreateTexture(_renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, _w, _h));
			if (!_texture) throw std::runtime_error("SDL_CreateTexture");

			open_audio();
		}

		~sdl2_renderer()
		{
			SDL_CloseAudioDevice(_audio);
			SDL_Quit();
		}

		void initialize(yagbe::context &ctx)
		{
			ctx.gpu.onFrameReady = [&](auto& frame)
			{
				accept_image(frame);
			};

			ctx.apu.configure(_audioSpec.freq, _audioSpec.samples / 4);
			ctx.apu.onSamplesReady = [&](auto&& samples)
			{
				accept_samples(std::move(samples));
			};
		}

		bool frame_drawn()
		{
			return _frame_drawn;
		}

		void next_frame()
		{
			_frame_drawn = false;
		}

		bool step()
		{
			handle_events();
			return _running;
		}

		void accept_image(const std::vector<uint8_t>& image)
		{
			if (image.size() != _w * _h * channels)
				throw std::runtime_error("accept_image wrong size");

			accept_raw_image(image.data());
		}

		void accept_image(const std::vector<uint32_t>& image)
		{
			if (image.size() != _w * _h)
				throw std::runtime_error("accept_image wrong size");

			accept_raw_image((uint8_t*)image.data());
		}

		void accept_image(const std::vector<color>& image)
		{
			if (image.size() != _w * _h)
				throw std::runtime_error("accept_image wrong size");

			accept_raw_image((uint8_t*)image.data());
		}

		void accept_samples(const std::vector<float>&& samples)
		{
			SDL_QueueAudio(_audio, samples.data(), (Uint32)samples.size() * sizeof(float));
		}

		bool running() { return _running; }

		struct key_info
		{
			bool shift = false;
		};

		std::function<void(SDL_Keycode, bool, const key_info&)> onKeyChanged;
	protected:
		void audio_callback(float* buff, int len) 
		{
			
			int channels = 2;
			for (int i = 0; i < len; i += channels)
			{
				auto& outLeft = buff[i];
				auto& outRight = buff[i + 1];
				

				auto sine = [=](double freq) {
					static double secondsFromStart = 0;
					secondsFromStart += _audioStep;
					return (float)sin(secondsFromStart * freq * M_PI * 2.0);
				};

				auto square = [=](double freq) {
					static double len = 1.0 / freq / 2.0;
					static double acc = 0.0;
					static double v = 0.25;
					acc += _audioStep;
					if (acc > len) { acc = 0.0; v = -v; };
					return v;
				};

				auto silence = [=]() {
					return 0.0;
				};

				//square 1000


				outLeft = outRight = (float)silence();
			}
		}

		void open_audio()
		{
			SDL_AudioSpec want;

			SDL_memset(&_audioSpec, 0, sizeof(_audioSpec));
			SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
			want.freq = 48000;
			want.format = AUDIO_F32;
			want.channels = 2;
			want.samples = 4096;
			want.userdata = this;
			want.callback = nullptr;

			_audio = SDL_OpenAudioDevice(NULL, 0, &want, &_audioSpec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
			if (_audio == 0) {
				SDL_Log("Failed to open audio: %s", SDL_GetError());
			}
			else {
				_audioStep = 1.0 / (double)_audioSpec.freq;
				SDL_PauseAudioDevice(_audio, 0); /* start audio playing. */
			}
		}

		void accept_raw_image(const uint8_t *input)
		{
			_frame_drawn = true;
			auto texture = _texture.get();
			uint8_t *pixels;
			int pitch;
			if (SDL_LockTexture(texture, nullptr, &(void*&)pixels, &pitch) != 0)
				throw std::runtime_error("SDL_LockTexture");

			auto it = input;
			for (auto y = 0; y < _h; y++)
			{
				std::copy(it, it + _w*channels, pixels);
				it += _w*channels;
				pixels += pitch;
			}

			SDL_UnlockTexture(texture);

			auto ren = _renderer.get();

			SDL_RenderCopy(ren, _texture.get(), NULL, NULL);
			SDL_RenderPresent(ren);
		}

		void handle_events()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e)) 
			{
				if (e.type == SDL_QUIT) 
					_running = false;

				if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				{
					auto key_code = e.key.keysym.sym;
					key_info info;
					info.shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;

					if (onKeyChanged)
						onKeyChanged(key_code, e.type == SDL_KEYDOWN, info);
					return;
				}
			}
		}

		int _w, _h;
		bool _frame_drawn = false;
		const static int channels = 4;
		std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window = { nullptr, SDL_DestroyWindow };
		std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer = { nullptr, SDL_DestroyRenderer };
		std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _texture = { nullptr, SDL_DestroyTexture };
		SDL_AudioDeviceID _audio = 0;
		SDL_AudioSpec _audioSpec = {};
		double _audioStep = 0.0f;
		bool _running = true;
	};


};