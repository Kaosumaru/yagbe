#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include "vm/context.hpp"
#include "renderer/sdl2_renderer.hpp"



int main (int argc, char * argv[])
{
	using namespace yagbe;

	std::string path = YAGBE_ROMS;
	path += "opus5.gb";

	context ctx;
	if (!ctx.load_rom(path))
		return -1;

	sdl2_renderer renderer(gpu::screen_size());

	ctx.gpu.onFrameReady = [&](auto &frame) { renderer.accept_image(frame); };
	//renderer.accept_image(image);
	while (renderer.step())
	{
		ctx.cpu_step();
	}


	return 0;
}
