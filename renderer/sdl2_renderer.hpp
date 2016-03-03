#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include <vector>
#include <algorithm>
#include "SDL.h"


namespace yagbe
{
	class sdl2_renderer
	{
	public:
		sdl2_renderer(int w, int h) : _w(w), _h(h)
		{
			if (SDL_Init(SDL_INIT_VIDEO) != 0)
				throw std::runtime_error("SDL_Init");

			_window.reset(SDL_CreateWindow("YAGBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN));
			if (!_window) throw std::runtime_error("SDL_CreateWindow");

			_renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
			if (!_renderer) throw std::runtime_error("SDL_CreateRenderer");

			_texture.reset(SDL_CreateTexture(_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h));
			if (!_texture) throw std::runtime_error("SDL_CreateTexture");
		}

		~sdl2_renderer()
		{
			SDL_Quit();
		}

		bool step()
		{
			handle_events();

			auto ren = _renderer.get();

			SDL_RenderCopy(ren, _texture.get(), NULL, NULL);
			SDL_RenderPresent(ren);

			return _running;
		}

		void accept_image(const std::vector<uint8_t>& image)
		{
			if (image.size() != _w * _h * channels)
				throw std::runtime_error("accept_image wrong size");

			auto texture = _texture.get();
			uint8_t *pixels;
			int pitch;
			if (SDL_LockTexture(texture, nullptr, &(void*&)pixels, &pitch) != 0)
				throw std::runtime_error("SDL_LockTexture");

			auto it = image.begin();
			for (auto y = 0; y < _h; y++)
			{
				std::copy(it, it + _w*channels, pixels);
				it += _w*channels;
				pixels += pitch;
			}

			SDL_UnlockTexture(texture);
		}

	protected:
		void handle_events()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e)) 
			{
				if (e.type == SDL_QUIT) 
					_running = false;

				if (e.type == SDL_KEYDOWN) 
				{
					
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