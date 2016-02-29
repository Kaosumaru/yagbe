#pragma once
#include "context.hpp"
namespace yagbe
{

namespace operands
{
	struct A {};
	struct F {};

	struct AF {};

	struct B {};
	struct C {};

	struct C_pointer {};

	struct BC {};
	struct BC_pointer {};

	struct D {};
	struct E {};

	struct DE {};
	struct DE_pointer {};

	struct H {};
	struct L {};

	struct HL {};
	struct HL_pointer {};

	struct d8 {};
	struct d16 {};

	struct HLI {};
	struct HLD {};

	struct SP {};




	template<typename Register>
	struct unwrap_register
	{

	};

	template<>
	struct unwrap_register<A>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.a; }
	};

	template<>
	struct unwrap_register<operands::F>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.f; }
	};



	template<>
	struct unwrap_register<operands::B>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.b; }
	};

	template<>
	struct unwrap_register<operands::C>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.c; }
	};



	template<>
	struct unwrap_register<operands::D>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.d; }
	};

	template<>
	struct unwrap_register<operands::E>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.e; }
	};



	template<>
	struct unwrap_register<operands::H>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.h; }
	};

	template<>
	struct unwrap_register<operands::L>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.l; }
	};


	template<>
	struct unwrap_register<operands::AF>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.af; }
	};

	template<>
	struct unwrap_register<operands::BC>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.bc; }
	};

	template<>
	struct unwrap_register<operands::DE>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.de; }
	};

	template<>
	struct unwrap_register<operands::HL>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.hl; }
	};

	template<>
	struct unwrap_register<operands::SP>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.sp; }
	};

	template<>
	struct unwrap_register<operands::BC_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.bc); }
	};

	template<>
	struct unwrap_register<operands::DE_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.de); }
	};

	template<>
	struct unwrap_register<operands::HL_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl); }
	};

	template<>
	struct unwrap_register<operands::HLI>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl++); }
	};

	template<>
	struct unwrap_register<operands::HLD>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl--); }
	};

	template<>
	struct unwrap_register<operands::C_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(0xFF00 + c.registers.c); }
	};


	template<>
	struct unwrap_register<operands::d8>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto get(context &c) { return c.read_byte(); }
	};

	template<>
	struct unwrap_register<operands::d16>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto get(context &c) { return c.read_word(); }
	};


	//operands

	template<uint8_t s, uint8_t c>
	struct default_operand
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c; }
	};

	template<uint8_t s, uint8_t c, typename Arg>
	struct default_unary_operand
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap_register<Arg>::complexity; }
	};

	template<uint8_t s, uint8_t c, typename Arg1, typename Arg2>
	struct default_binary_operand
	{
		static_assert(unwrap_register<Arg1>::size_of == unwrap_register<Arg2>::size_of, "Mismatching sizes");

		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap_register<Arg1>::complexity + unwrap_register<Arg2>::complexity; }
	};

	//NOP
	struct NOP : default_operand<1, 4>
	{
		static void execute(context &c)
		{

		}
	};

	//HALT
	struct HALT : default_operand<1, 4>
	{
		static void execute(context &c)
		{
			throw std::runtime_error("NYI");
		}
	};

	//STOP
	struct STOP : default_operand<2, 4>
	{
		static void execute(context &c)
		{
			c.read_byte();
			throw std::runtime_error("NYI");
		}
	};

	//this allows to type LD<B,A>::execute, or LD<A,B>::execute, etc
	template<typename Arg1, typename Arg2>
	struct LD : default_binary_operand<1, 4, Arg1, Arg2>
	{
		static void execute(context &c)
		{
			unwrap_register<Arg1>::get(c) = unwrap_register<Arg2>::get(c);
		}
	};


	//PUSH
	template<typename Arg>
	struct PUSH : default_operand<1, 16>
	{
		static_assert(unwrap_register<Arg>::size_of == 2, "Assuming word");

		static void execute(context &c)
		{
			union
			{
				uint8_t bytes[2];
				uint16_t word;
			};

			word = unwrap_register<Arg>::get(c);

			//TODO endianess
			c.memory.at(c.registers.sp--) = bytes[0];
			c.memory.at(c.registers.sp--) = bytes[1];
		}
	};

	//POP
	template<typename Arg>
	struct POP : default_operand<1, 16>
	{
		static_assert(unwrap_register<Arg>::size_of == 2, "Assuming word");

		static void execute(context &c)
		{
			union
			{
				uint8_t bytes[2];
				uint16_t word;
			};

			//TODO endianess
			bytes[1] = c.memory.at(c.registers.sp++);
			bytes[0] = c.memory.at(c.registers.sp++);

			unwrap_register<Arg>::get(c) = word;
		}
	};


	//ARITHMETIC

	//INC
	template<typename Arg>
	struct INC : default_unary_operand<1, 4, Arg>
	{
		static void execute(context &c)
		{
			bool byte = unwrap_register<Arg>::size_of == 1;
			auto value = unwrap_register<Arg>::get(c);

			if (byte)
			{
				c.flags.h = ((value & 0x0f) == 0x0f);
			}

			value++;
			unwrap_register<Arg>::get(c) = value;

			if (byte)
			{
				c.flags.z = (value == 0);
				c.flags.n = 0;
			}
		}
	};

	//DEC
	template<typename Arg>
	struct DEC : default_unary_operand<1, 4, Arg>
	{
		static void execute(context &c)
		{
			bool byte = unwrap_register<Arg>::size_of == 1;
			auto value = unwrap_register<Arg>::get(c);

			if (byte)
			{
				c.flags.h = !(value & 0x0f); //TODO is this correct?
			}

			value--;
			unwrap_register<Arg>::get(c) = value;

			if (byte)
			{
				c.flags.z = (value == 0);
				c.flags.n = 1;
			}
		}
	};


}
}