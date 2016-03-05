#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>

#include "vm/context.hpp"
#include "renderer/sdl2_renderer.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main (int argc, char * argv[])
{
	using namespace yagbe;
	std::map<SDL_Keycode, key_handler::key> keys = { 
		{ SDLK_LEFT, key_handler::key::Left },
		{ SDLK_RIGHT, key_handler::key::Right },
		{ SDLK_UP, key_handler::key::Up },
		{ SDLK_DOWN, key_handler::key::Down },

		{ SDLK_a, key_handler::key::Select },
		{ SDLK_s, key_handler::key::Start },
		{ SDLK_z, key_handler::key::A },
		{ SDLK_x, key_handler::key::B },
	};


	std::string path = YAGBE_ROMS;
	path += "opus5.gb";

	context ctx;
	if (!ctx.load_rom(path))
		return -1;

	sdl2_renderer renderer(gpu::screen_size());

	ctx.gpu.onFrameReady = [&](auto &frame) { renderer.accept_image(frame); };

	renderer.onKeyChanged = [&](SDL_Keycode c, bool v)
	{
		auto it = keys.find(c);
		if (it != keys.end())
			ctx.key_handler.set_key(it->second, v);
	};

	//renderer.accept_image(image);
	while (renderer.step())
	{
		ctx.cpu_step();
	}


	return 0;
}
