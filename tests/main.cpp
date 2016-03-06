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



bool test_opus5()
{
	std::string path = YAGBE_ROMS;
	path += "opus5.gb";

	context c;
	if (!c.load_rom(path))
		return false;

	int steps = 100;

	for (int i = 0; i < steps; i++)
	{
		c.cpu_step();

		//endian...
		if (c.registers.pc == 0x017E)
			break;
	}

	if (c.registers.pc != 0x017E)
		return false;

	bool s = true;

	auto m = [&](uint16_t a) { return c.memory.raw_at(a); };

	s &= m(0xFFFF) == 0x01;

	s &= m(0xFF41) == 0x00;
	s &= m(0xFF40) == 0x00;

	s &= m(0xFF43) == 0x10;
	s &= m(0xC1C9) == 0x10; s &= m(0xE1C9) == 0x10; //shadow RAM
	s &= m(0xC1CC) == 0x10; s &= m(0xE1CC) == 0x10;

	s &= m(0xFF42) == 0x08;
	s &= m(0xC1CB) == 0x08; s &= m(0xE1CB) == 0x08;
	s &= m(0xC1CD) == 0x08; s &= m(0xE1CD) == 0x08;

	s &= m(0xC1C8) == 0x00; s &= m(0xE1C8) == 0x00;
	s &= m(0xC1CA) == 0x00; s &= m(0xE1CA) == 0x00;
	s &= m(0xC0A0) == 0x00; s &= m(0xE0A0) == 0x00;

	//endian...
	s &= c.registers.bc == 0x4000;


	return true;
}

const lest::test specification[] =
{
	CASE("01-special-5")
	{
		context ctx;
		ctx.registers.bc = 0x1200;

		do
		{
			PUSH<BC>::execute(ctx); 	static_assert(&(instruction<0xC5>::execute) == &(PUSH<BC>::execute), "Wrong mapping");
			POP<AF>::execute(ctx);      static_assert(&(instruction<0xF1>::execute) == &(POP<AF>::execute), "Wrong mapping");
			PUSH<AF>::execute(ctx);     static_assert(&(instruction<0xF5>::execute) == &(PUSH<AF>::execute), "Wrong mapping");
			POP<DE>::execute(ctx);      static_assert(&(instruction<0xD1>::execute) == &(POP<DE>::execute), "Wrong mapping");

			LD<A, C>::execute(ctx);      static_assert(&(instruction<0x79>::execute) == &(LD<A, C>::execute), "Wrong mapping");

			ctx.registers.a &= 0xF0;

			CP<E>::execute(ctx);        static_assert(&(instruction<0xBB>::execute) == &(CP<E>::execute), "Wrong mapping");

			bool condition = (bool)ctx.flags.z == true;
			EXPECT(condition);

			INC<B>::execute(ctx);
			INC<C>::execute(ctx);
		} while (!ctx.flags.z);



	},


	CASE("ROM")
	{
		context ctx;
		ctx.memory.raw_at(0) = 1;
		ctx.memory.at(0) = 2;
		EXPECT(ctx.memory.at(0) == 1);
	},

	CASE("SHADOW RAM")
	{
		context ctx;

		for (uint16_t i = 0xC000; i <= 0xDFFF; i++)
			ctx.memory.at(i) = i % 256;

		for (uint16_t i = 0xE000; i <= 0xFDFF; i++)
			EXPECT(ctx.memory.read_at(i) == ctx.memory.read_at(i - 0x2000));

	},


	CASE("OPUS5")
	{
		EXPECT(test_opus5());
	},

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
			ctx.memory.raw_at(0) = 6;
			ctx.memory.raw_at(1) = 0;

			ld_bc_d16::execute(ctx);

			EXPECT(ctx.registers.bc == 6);
			EXPECT(ld_bc_d16::cycles() == 12);
		}
		
		//LD (HLI)/(HLD),A 
		{
			using ld_hli_a = LD<HLI, A>;
			using ld_hld_a = LD<HLD, A>;

			ctx.memory.raw_at(0xC000) = 6;
			ctx.memory.raw_at(0xC001) = 6;

			ctx.registers.a = 5;
			ctx.registers.hl = 0xC000;
			
			ld_hli_a::execute(ctx);

			EXPECT(ctx.registers.hl == 0xC001);
			EXPECT(ctx.memory.at(0xC000) == 5);
			EXPECT(ctx.memory.at(0xE000) == 5); //shadow

			ctx.registers.a = 1;
			ld_hld_a::execute(ctx);

			EXPECT(ctx.registers.hl == 0xC000);
			EXPECT(ctx.memory.at(0xC001) == 1);
			EXPECT(ctx.memory.at(0xE001) == 1); //shadow

			EXPECT(ld_hli_a::cycles() == 8);
			EXPECT(ld_hld_a::cycles() == 8);
		}

		//LD<HL, SP_p_r8> //TODO seems like this sets h & c - good test case


	},
	CASE("JP")
	{
		context ctx;

		//JP d16
		{
			using jp_d16 = JP<condition::_, d16>;

			ctx.registers.pc = 0;
			ctx.memory.raw_at(0) = 6;
			ctx.memory.raw_at(1) = 0;

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
				ctx.memory.raw_at(0) = 6;
				ctx.memory.raw_at(1) = 0;
				ctx.flags.c = 1;

				auto cycles = JP_NC_HL::execute(ctx);
				EXPECT(ctx.registers.pc == 2);
				EXPECT(cycles == 12);
			}

			{
				ctx.registers.pc = 0;
				ctx.memory.raw_at(0) = 6;
				ctx.memory.raw_at(1) = 0;
				ctx.flags.c = 0;

				auto cycles = JP_NC_HL::execute(ctx);
				EXPECT(ctx.registers.pc == 6);
				EXPECT(cycles == 16);
			}
		}

		//JP NZ,d16
		{
			using JP_NZ_HL = instructions::JP<condition::NZ, d16>;

			{
				ctx.registers.pc = 0;
				ctx.memory.raw_at(0) = 6;
				ctx.memory.raw_at(1) = 0;
				ctx.flags.z = 1;

				auto cycles = JP_NZ_HL::execute(ctx);
				EXPECT(ctx.registers.pc == 2);
				EXPECT(cycles == 12);
			}

			{
				ctx.registers.pc = 0;
				ctx.memory.raw_at(0) = 6;
				ctx.memory.raw_at(1) = 0;
				ctx.flags.z = 0;

				auto cycles = JP_NZ_HL::execute(ctx);
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
			ctx.memory.raw_at(0) = 6;

			auto cycles = JR_D8::execute(ctx);

			EXPECT(ctx.registers.pc == 7);
			EXPECT(cycles == 12);
		}

		//JR NC,r8
		{
			using JR_NC_D8 = instructions::JR<condition::NC, r8>;

			{
				ctx.registers.pc = 0;
				ctx.memory.raw_at(0) = 6;
				ctx.flags.c = 1;

				auto cycles = JR_NC_D8::execute(ctx);
				EXPECT(ctx.registers.pc == 1);
				EXPECT(cycles == 8);
			}

			{
				ctx.registers.pc = 0;
				ctx.memory.raw_at(0) = 6;
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
