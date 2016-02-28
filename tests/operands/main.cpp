#include <iostream>
#include <stdexcept>
#include "vm/context.hpp"
#include "vm/operands.hpp"

#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

#include "lest.hpp"



using namespace std;
using namespace yagbe;
using namespace yagbe::operands;

const lest::test specification[] =
{
	CASE("LD")
	{
		context ctx;
		ctx.registers.a = 1;
		ctx.registers.b = 2;

		auto ld_b_a = &(LD<B, A>::execute);
		ld_b_a(ctx);

		EXPECT(ctx.registers.b == 1);
	}
};


int main (int argc, char * argv[])
{
	return lest::run(specification, argc, argv);
}
