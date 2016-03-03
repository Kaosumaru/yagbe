#include <iostream>
#include <stdexcept>
#include <sstream>

#include "vm/context.hpp"
#include "renderer/sdl2_renderer.hpp"

int main (int argc, char * argv[])
{
	using namespace yagbe;
	sdl2_renderer renderer(256,256);

	std::vector<uint8_t> bytes;

	bytes.resize(256 * 256 * 4);

	for (int y = 0; y < 256; y ++)
		for (int x = 0; x < 256; x++)
		{
			auto o = x * 4 + y * 256 * 4;
			bytes[o + 0] = 255; //A
			bytes[o + 1] = y;   //B
			bytes[o + 2] = y;   //G
			bytes[o + 3] = y;   //R
		}

	renderer.accept_image(bytes);
	while (renderer.step())
	{

	}


	return 0;
}
