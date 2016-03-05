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

namespace yagbe
{
	class sdl2_renderer
	{
	public:
		sdl2_renderer(const ipoint &size, int scale = 4) : _w(size.x), _h(size.y)
		{
			if (SDL_Init(SDL_INIT_VIDEO) != 0)
				throw std::runtime_error("SDL_Init");

			_window.reset(SDL_CreateWindow("YAGBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _w * scale, _h * scale, SDL_WINDOW_SHOWN));
			if (!_window) throw std::runtime_error("SDL_CreateWindow");

			_renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
			if (!_renderer) throw std::runtime_error("SDL_CreateRenderer");

			_texture.reset(SDL_CreateTexture(_renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, _w, _h));
			if (!_texture) throw std::runtime_error("SDL_CreateTexture");
		}

		~sdl2_renderer()
		{
			SDL_Quit();
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

		bool running() { return _running; }

		std::function<void(SDL_Keycode, bool)> onKeyChanged;
	protected:
		void accept_raw_image(const uint8_t *input)
		{
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
					if (onKeyChanged)
						onKeyChanged(key_code, e.type == SDL_KEYDOWN);
				}
			}
		}

		int _w, _h;
		const static int channels = 4;
		std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window = { nullptr, SDL_DestroyWindow };
		std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer = { nullptr, SDL_DestroyRenderer };
		std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _texture = { nullptr, SDL_DestroyTexture };
		bool _running = true;
	};


};