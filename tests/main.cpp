#include <iostream>
#include <stdexcept>
#include "vm/context.hpp"
#include "vm/instructions.hpp"
#include "vm/instructions_map.hpp"

#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

#include "lest.hpp"



using namespace std;
using namespace yagbe;
using namespace yagbe::instructions;
using namespace yagbe::instructions::automap;

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

	},
	CASE("JP")
	{
		context ctx;

		//JP d16
		{
			using jp_d16 = JP<condition::_, d16>;

			ctx.registers.pc = 0;
			ctx.memory.at(0) = 6;
			ctx.memory.at(1) = 0;

			auto cycles = jp_d16::execute(ctx);

			EXPECT(ctx.registers.pc == 6);
			EXPECT(cycles == 16);
		}

		//JP (HL)
		{
			using jp_HL = JP<condition::_, HL>;

			ctx.registers.pc = 0;
			ctx.registers.hl = 6;

			auto cycles = jp_HL::execute(ctx);

			EXPECT(ctx.registers.pc == 6);
			EXPECT(cycles == 4);
		}

		//JP NC,d16
		{
			using JP_NC_HL = instructions::JP<condition::NC, d16>;

			{
				ctx.registers.pc = 0;
				ctx.memory.at(0) = 6;
				ctx.memory.at(1) = 0;
				ctx.flags.c = 1;

				auto cycles = JP_NC_HL::execute(ctx);
				EXPECT(ctx.registers.pc == 2);
				EXPECT(cycles == 12);
			}

			{
				ctx.registers.pc = 0;
				ctx.memory.at(0) = 6;
				ctx.memory.at(1) = 0;
				ctx.flags.c = 0;

				auto cycles = JP_NC_HL::execute(ctx);
				EXPECT(ctx.registers.pc == 6);
				EXPECT(cycles == 16);
			}
		}

	},
	CASE("JR")
	{
		context ctx;

		//JR r8
		{
			using JR_D8 = JR<condition::_, r8>;

			ctx.registers.pc = 0;
			ctx.memory.at(0) = 6;

			auto cycles = JR_D8::execute(ctx);

			EXPECT(ctx.registers.pc == 7);
			EXPECT(cycles == 12);
		}

		//JR NC,r8
		{
			using JR_NC_D8 = instructions::JR<condition::NC, r8>;

			{
				ctx.registers.pc = 0;
				ctx.memory.at(0) = 6;
				ctx.flags.c = 1;

				auto cycles = JR_NC_D8::execute(ctx);
				EXPECT(ctx.registers.pc == 1);
				EXPECT(cycles == 8);
			}

			{
				ctx.registers.pc = 0;
				ctx.memory.at(0) = 6;
				ctx.flags.c = 0;

				auto cycles = JR_NC_D8::execute(ctx);
				EXPECT(ctx.registers.pc == 7);
				EXPECT(cycles == 12);
			}
		}

	}
};


int main (int argc, char * argv[])
{
	return lest::run(specification, argc, argv);
}
