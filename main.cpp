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
using namespace yagbe;

context ctx;
sdl2_renderer renderer(gpu::screen_size());
bool frame_drawn = false;

void one_iter()
{
	frame_drawn = false;
	while (renderer.step() && !frame_drawn)
	{
		ctx.cpu_step();
	}
}

int main(int argc, char * argv[])
{

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

	//path += "individual/01-special.gb"; //hangs?
	//path += "individual/02-interrupts.gb"; //failing EI
	//path += "individual/03-op sp,hl.gb"; //failing 39 e8 e8 f8 f8

	//path += "individual/04-op r,imm.gb"; //failing C6 CE DE
	//path += "individual/05-op rp.gb"; //failing 09 19 29
	//path += "individual/06-ld r,r.gb"; //PASSED

	path += "individual/07-jr,jp,call,ret,rst.gb"; //HANGS? important

	//path += "individual/08-misc instrs.gb"; //failing

	//path += "individual/09-op r,r.gb"; //failing

	//path += "individual/10-bit ops.gb"; //PASSED
	
	//path += "individual/11-op a,(hl).gb"; //FAILED



	if (!ctx.load_rom(path))
		return -1;


	ctx.gpu.onFrameReady = [&](auto &frame)
	{
		renderer.accept_image(frame);
		frame_drawn = true;
	};

	renderer.onKeyChanged = [&](SDL_Keycode c, bool v)
	{
		auto it = keys.find(c);
		if (it != keys.end())
			ctx.key_handler.set_key(it->second, v);
	};


#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(one_iter, 60, 1);
#else
	while (renderer.running())
		one_iter();
#endif

	return 0;
}


/*
Left to do:
- more interrupts
- window
- more io_registers
- 16h sprites, background sprites

- more unit tests on GPU & CPU

-sound
*/