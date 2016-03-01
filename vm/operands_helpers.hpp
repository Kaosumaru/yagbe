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
	struct d8_pointer {};
	struct d16 {};
	struct d16_pointer {};

	struct HLI {};
	struct HLD {};

	struct SP {};





	template<typename Register>
	struct unwrap
	{

	};

	template<>
	struct unwrap<A>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.a; }
	};

	template<>
	struct unwrap<operands::F>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.f; }
	};



	template<>
	struct unwrap<operands::B>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.b; }
	};

	template<>
	struct unwrap<operands::C>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.c; }
	};



	template<>
	struct unwrap<operands::D>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.d; }
	};

	template<>
	struct unwrap<operands::E>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.e; }
	};



	template<>
	struct unwrap<operands::H>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.h; }
	};

	template<>
	struct unwrap<operands::L>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.registers.l; }
	};


	template<>
	struct unwrap<operands::AF>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.af; }
	};

	template<>
	struct unwrap<operands::BC>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.bc; }
	};

	template<>
	struct unwrap<operands::DE>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.de; }
	};

	template<>
	struct unwrap<operands::HL>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.hl; }
	};

	template<>
	struct unwrap<operands::SP>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto& get(context &c) { return c.registers.sp; }
	};

	template<>
	struct unwrap<operands::BC_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.bc); }
	};

	template<>
	struct unwrap<operands::DE_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.de); }
	};

	template<>
	struct unwrap<operands::HL_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl); }
	};

	template<>
	struct unwrap<operands::HLI>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl++); }
	};

	template<>
	struct unwrap<operands::HLD>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.registers.hl--); }
	};

	template<>
	struct unwrap<operands::C_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(0xFF00 + c.registers.c); }
	};


	template<>
	struct unwrap<operands::d8>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;

		static auto get(context &c) { return c.read_byte(); }
	};

	template<>
	struct unwrap<operands::d8_pointer>
	{
		constexpr static int complexity = 8;
		constexpr static int size_of = 1;

		static auto get(context &c) { return c.memory.at(0xFF00 + c.read_byte()); }
	};

	template<>
	struct unwrap<operands::d16>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;

		static auto get(context &c) { return c.read_word(); }
	};

	template<>
	struct unwrap<operands::d16_pointer>
	{
		constexpr static int complexity = 12;
		constexpr static int size_of = 1;

		static auto& get(context &c) { return c.memory.at(c.read_word()); }
	};

	//conditions
	namespace condition
	{
		struct NZ {};
		struct Z {};

		struct NC {};
		struct C {};

		struct _ {};
	}

	template<>
	struct unwrap<condition::NZ>
	{
		static bool is_true(context &c) { return !c.flags.z; }
	};

	template<>
	struct unwrap<condition::Z>
	{
		static bool is_true(context &c) { return (bool)c.flags.z; }
	};

	template<>
	struct unwrap<condition::NC>
	{
		static bool is_true(context &c) { return !c.flags.c; }
	};

	template<>
	struct unwrap<condition::C>
	{
		static bool is_true(context &c) { return (bool)c.flags.c; }
	};

	template<>
	struct unwrap<condition::_>
	{
		static bool is_true(context &c) { return true; }
	};


}
}