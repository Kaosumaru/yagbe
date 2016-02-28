#include <iostream>
#include <stdexcept>
#include <sstream>

#include "vm/context.hpp"
#include "vm/operands.hpp"
#include "vm/operands_map.hpp"


int main (int argc, char * argv[])
{
	using namespace yagbe;
	using namespace yagbe::operands;

	
	{
		context ctx;
		ctx.registers.a = 1;
		ctx.registers.b = 2;

		auto ld_b_a = &(LD<B, A>::execute);
		ld_b_a(ctx);

		std::cout << "B is now " << (int)ctx.registers.b << std::endl;
	}

	{
		using namespace yagbe::operands::automap;
		context ctx;
		ctx.registers.a = 1;
		ctx.registers.b = 2;

		auto ld_b_a = &(operand<0x47>::execute);
		ld_b_a(ctx);

		std::cout << "B is now " << (int)ctx.registers.b << std::endl;
	}

	return 0;
}
