#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <iomanip> 

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



std::ofstream iterfile{ "d:/sl/iter.txt" };
int ix = 409;

void log_more()
{
	if (ix != 0)
	{

		iterfile << std::hex << std::setw(2) << std::setfill('0') << (int)ctx.registers.pc << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.sp << " "

			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.a << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.b << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.c << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.d << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.e << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.h << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.l << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.registers.f << " "
			<< std::setw(2) << std::setfill('0') << (int)ctx.cycles_elapsed / 4

			<< "\n";
		//iterfile << ctx.registers.pc << "\n";
	}

	if (ctx.registers.pc == 0x3c)
	{
		ix = 0;
		iterfile.close();
	}
}


void log_tst()
{
	if (ix <= 0)
	{
		iterfile.close();
		return;
	}

	static std::set<int> all;

	if (!all.insert(ctx.registers.pc).second)
		return;

	iterfile << std::hex << ctx.registers.pc 
		<< " " << (int)ctx.registers.a
		<< " " << (int)ctx.registers.b
		<< " " << (int)ctx.registers.c
		<< " " << (int)ctx.registers.d
		<< " " << (int)ctx.registers.e
		<< " " << (int)ctx.registers.h 
		<< " " << (int)ctx.registers.l
		<< " " << (int)ctx.registers.f
		<< " " << (int)(ctx.registers.sp >> 8)
		<< " " << (int)(ctx.registers.sp & 0xFF)
		<< " " << (int)ctx.memory.at(0x9800)
		<< "\n";
	ix--;
}



void one_iter()
{
	if (!loaded_rom)
		return;
	frame_drawn = false;
	while (renderer.step() && !frame_drawn)
	{
		for (int i = 0; i < 10; i++)
		{
			//log_more()
			log_tst();
			if (ctx.registers.pc == 0xc0c3)
			{
				int a = 6; 
				a++;
			}
				
			ctx.cpu_step();

		}
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

	path += "../test_roms/individual/01-special.gb"; //PASSED
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

	//path += "../test_roms/opus5.gb";

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