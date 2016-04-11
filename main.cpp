#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>


#include "vm/context.hpp"
#include "renderer/sdl2_renderer.hpp"
#include "serializer/serializer.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
using namespace yagbe;

context ctx;
sdl2_renderer renderer(gpu::screen_size());

#ifdef __EMSCRIPTEN__
emscripten_serializer saves(ctx);
#else
filesave_serializer saves(ctx);
#endif


bool frame_drawn = false;
bool loaded_rom = false;



void one_iter()
{
	if (!loaded_rom)
		return;
	frame_drawn = false;
	while (renderer.step() && !frame_drawn)
	{
		for(int i = 0; i < 10; i ++)
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
	//path += "sml.gb";

	path += "adjtris.gb";

	ctx.gpu.onFrameReady = [&](auto &frame)
	{
		renderer.accept_image(frame);
		frame_drawn = true;
	};

	renderer.onKeyChanged = [&](SDL_Keycode c, bool v, const sdl2_renderer::key_info& info)
	{
		if (c >= SDLK_0 && c <= SDLK_9 && v)
		{
			int index = c - SDLK_0;
			if (info.shift)
				saves.save_state(index);
			else
				saves.load_state(index);
		}

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
			auto rom_info = ctx.load_rom((uint8_t*)b, s);
			if (rom_info)
			{
				loaded_rom = true;
				std::cout << "Loaded OK, name: " << rom_info->game_title() << std::endl;
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
		std::cout << "Loaded built-in OK, name: " << rom_info->game_title() << std::endl;
		loaded_rom = ctx.load_rom(path);
	}
		


	emscripten_set_main_loop(one_iter, 0, 1);
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
- RAM & timers in MBC1-3

-sound
*/