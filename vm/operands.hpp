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

	template<typename Register>
	struct unwrap_register
	{

	};

	template<>
	struct unwrap_register<A>
	{
		static auto& get(context &c) { return c.registers.a; }
	};

	template<>
	struct unwrap_register<operands::F>
	{
		static auto& get(context &c) { return c.registers.f; }
	};



	template<>
	struct unwrap_register<operands::B>
	{
		static auto& get(context &c) { return c.registers.b; }
	};

	template<>
	struct unwrap_register<operands::C>
	{
		static auto& get(context &c) { return c.registers.c; }
	};



	template<>
	struct unwrap_register<operands::D>
	{
		static auto& get(context &c) { return c.registers.d; }
	};

	template<>
	struct unwrap_register<operands::E>
	{
		static auto& get(context &c) { return c.registers.e; }
	};



	template<>
	struct unwrap_register<operands::H>
	{
		static auto& get(context &c) { return c.registers.h; }
	};

	template<>
	struct unwrap_register<operands::L>
	{
		static auto& get(context &c) { return c.registers.l; }
	};


	template<>
	struct unwrap_register<operands::HL>
	{
		static auto& get(context &c) { return c.registers.hl; }
	};


	//operands

	//NOP
	struct NOP
	{
		static void execute(context &c)
		{

		}
	};

	//this allows to type LD<B,A>::execute, or LD<A,B>::execute, etc
	template<typename Arg1, typename Arg2>
	struct LD
	{
		static void execute(context &c)
		{
			unwrap_register<Arg1>::get(c) = unwrap_register<Arg2>::get(c);
		}
	};

}
}