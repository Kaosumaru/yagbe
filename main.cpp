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
bool loaded_rom = false;

void one_iter()
{
	if (!loaded_rom)
		return;
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

	//path += "../test_roms/individual/01-special.gb"; //PASSED
	//path += "../test_roms/individual/02-interrupts.gb"; //PASSED
	//path += "../test_roms/individual/03-op sp,hl.gb"; //PASSED

	//path += "../test_roms/individual/04-op r,imm.gb"; //PASSED
	//path += "../test_roms/individual/05-op rp.gb"; //PASSED
	//path += "../test_roms/individual/06-ld r,r.gb"; //PASSED

	//path += "../test_roms/individual/07-jr,jp,call,ret,rst.gb"; //PASSED

	//path += "../test_roms/individual/08-misc instrs.gb"; //PASSED

	//path += "../test_roms/individual/09-op r,r.gb"; //PASSED

	//path += "../test_roms/individual/10-bit ops.gb"; //PASSED
	
	//path += "../test_roms/individual/11-op a,(hl).gb"; //PASSED
	//path += "../test_roms/instr_timing.gb"; //FAILED, timer doesn't work properly


	//path += "hangman.gb";
	//path += "opus5.gb";

	//path += "../proms/tetris.gb";
	path += "../proms/sml.gb";
	
	/*
	0x2ed

	LD A, 0xFF85
	AND A
	JR Z, loop

	ff85 is probably written to at 205 or 2F3

	seems like a wrong order, 0x205 is setting ff85 to 1, and 0x2f3 is setting it to 0

	this loops goes on and on, blocking writes to 0xff85, and fixing it at 1 ends loops, but game is buggy
	*/



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
	std::cout << "Starting emulator..." << std::endl;
	if (argc > 1)
	{
		std::cout << "Getting rom from: " << argv[1] << std::endl;

		auto onLoad = [](void*, void* b, int s)
		{
			std::cout << "Done." << std::endl;
			if (rom_info)
			{
				loaded_rom = true;
				std::cout << "Loaded OK, name: " << rom_info->name << std::endl;
				return;
			}
			std::cout << "Loading failed" << std::endl;
		};

		auto onError = [](void*)
		{
			std::cout << "Error." << std::endl;
		};


		emscripten_async_wget_data(argv[1], nullptr, onLoad, onError);
	}
	else
	{
		auto rom_info = ctx.load_rom(path);
		if (!rom_info)
			return -1;
		loaded_rom = ctx.load_rom(path);
	}
		


	emscripten_set_main_loop(one_iter, 60, 1);
#else
	auto rom_info = ctx.load_rom(path);
	if (!rom_info)
		return -1;
	loaded_rom = true;

	while (renderer.running())
		one_iter();
#endif

	return 0;
}


/*
Left to do:
- 16h sprites

-sound
*/