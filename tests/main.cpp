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

		//LD B,A
		{
			ctx.registers.a = 1;
			ctx.registers.b = 2;

			using ld_b_a = LD<B, A>;
			ld_b_a::execute(ctx);

			EXPECT(ctx.registers.b == 1);
			EXPECT(ctx.registers.b == ctx.registers.a);

			EXPECT(ld_b_a::size() == 1);
			EXPECT(ld_b_a::cycles() == 4);
		}

		//LD B,(HL)
		{
			using ld_b_hl = LD<B, HL_pointer>;
			EXPECT(ld_b_hl::cycles() == 8);
		}

		//LD BC,d16
		{
			using ld_bc_d16 = LD<BC, d16>;

			ctx.registers.pc = 0;
			ctx.memory.at(0) = 6;
			ctx.memory.at(1) = 0;

			ld_bc_d16::execute(ctx);

			EXPECT(ctx.registers.bc == 6);
			EXPECT(ld_bc_d16::cycles() == 12);
		}
		
		//LD (HLI)/(HLD),A 
		{
			using ld_hli_a = LD<HLI, A>;
			using ld_hld_a = LD<HLD, A>;

			ctx.memory.at(0) = 6;
			ctx.memory.at(1) = 6;

			ctx.registers.a = 5;
			ctx.registers.hl = 0;
			
			ld_hli_a::execute(ctx);

			EXPECT(ctx.registers.hl == 1);
			EXPECT(ctx.memory.at(0) == 5);

			ctx.registers.a = 1;
			ld_hld_a::execute(ctx);

			EXPECT(ctx.registers.hl == 0);
			EXPECT(ctx.memory.at(1) == 1);

			EXPECT(ld_hli_a::cycles() == 8);
			EXPECT(ld_hld_a::cycles() == 8);
		}

	}
};


int main (int argc, char * argv[])
{
	return lest::run(specification, argc, argv);
}
