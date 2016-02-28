#pragma once
#include "context.hpp"
namespace yagbe
{

namespace operands
{
	struct A {};
	struct F {};

	struct B {};
	struct C {};

	struct D {};
	struct E {};

	struct H {};
	struct L {};

	struct HL {};

	struct HL_pointer {};

	template<typename Register>
	struct unwrap_register
	{

	};

	template<>
	struct unwrap_register<A>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.a; }
	};

	template<>
	struct unwrap_register<operands::F>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.f; }
	};



	template<>
	struct unwrap_register<operands::B>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.b; }
	};

	template<>
	struct unwrap_register<operands::C>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.c; }
	};



	template<>
	struct unwrap_register<operands::D>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.d; }
	};

	template<>
	struct unwrap_register<operands::E>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.e; }
	};



	template<>
	struct unwrap_register<operands::H>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.h; }
	};

	template<>
	struct unwrap_register<operands::L>
	{
		constexpr static int complexity = 1;

		static auto& get(context &c) { return c.registers.l; }
	};


	template<>
	struct unwrap_register<operands::HL>
	{
		constexpr static int complexity = 2;

		static auto& get(context &c) { return c.registers.hl; }
	};

	template<>
	struct unwrap_register<operands::HL_pointer>
	{
		constexpr static int complexity = 2;

		static auto& get(context &c) { return c.memory.get(c.registers.hl); }
	};


	//operands

	template<uint8_t s, uint8_t c>
	struct default_operand
	{
		constexpr static int size = s;
		constexpr static int cycles = c;
	};

	template<uint8_t s, uint8_t c, typename Arg1, typename Arg2>
	struct default_binary_operand
	{
		constexpr static int size = s;
		constexpr static int cycles = c * unwrap_register<Arg1>::complexity * unwrap_register<Arg2>::complexity;
	};

	//NOP
	struct NOP : default_operand<1, 4>
	{
		static void execute(context &c)
		{

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

}
}