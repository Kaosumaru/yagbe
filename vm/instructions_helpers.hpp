#pragma once
#include "context.hpp"
namespace yagbe
{

namespace instructions
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
	struct d8_as_word {};
	struct d8_pointer {};
	struct d16 {};
	struct d16_pointer {};

	struct r8 {};
	struct r8_as_word {};

	struct HLI {};
	struct HLD {};

	struct SP {};
	struct SP_p_r8 {};




	template<typename Register>
	struct unwrap
	{

	};

	template<>
	struct unwrap<A>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.a; }
	};

	template<>
	struct unwrap<instructions::F>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.f; }
	};



	template<>
	struct unwrap<instructions::B>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.b; }
	};

	template<>
	struct unwrap<instructions::C>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.c; }
	};



	template<>
	struct unwrap<instructions::D>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.d; }
	};

	template<>
	struct unwrap<instructions::E>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.e; }
	};



	template<>
	struct unwrap<instructions::H>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.h; }
	};

	template<>
	struct unwrap<instructions::L>
	{
		constexpr static int complexity = 0;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.l; }
	};


	template<>
	struct unwrap<instructions::AF>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.af; }
	};

	template<>
	struct unwrap<instructions::BC>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.bc; }
	};

	template<>
	struct unwrap<instructions::DE>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.de; }
	};

	template<>
	struct unwrap<instructions::HL>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.hl; }
	};

	template<>
	struct unwrap<instructions::SP>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto& get(context &c) { return c.registers.sp; }
	};

	template<>
	struct unwrap<instructions::BC_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.registers.bc); }
	};

	template<>
	struct unwrap<instructions::DE_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.registers.de); }
	};

	template<>
	struct unwrap<instructions::HL_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.registers.hl); }
	};

	template<>
	struct unwrap<instructions::HLI>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.registers.hl++); }
	};

	template<>
	struct unwrap<instructions::HLD>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.registers.hl--); }
	};

	template<>
	struct unwrap<instructions::C_pointer>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(0xFF00 + c.registers.c); }
	};


	template<>
	struct unwrap<instructions::r8>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_signed = 1;

		static int8_t get(context &c) { return (int8_t)c.read_byte(); }
	};

	template<>
	struct unwrap<instructions::r8_as_word>
	{
		constexpr static int complexity = 8;
		constexpr static int size_of = 2;
		constexpr static int is_signed = 1;

		static int16_t get(context &c) { return (int8_t)c.read_byte(); }
	};

	template<>
	struct unwrap<instructions::d8>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 0;

		static auto get(context &c) { return c.read_byte(); }
	};

	template<>
	struct unwrap<instructions::d8_as_word>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static uint16_t get(context &c) { return c.read_byte(); }
	};

	template<>
	struct unwrap<instructions::d8_pointer>
	{
		constexpr static int complexity = 8;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) 
		{ 
			auto address = 0xFF00 + c.read_byte();
			return c.memory.at(address);
		}
	};

	template<>
	struct unwrap<instructions::d16>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

		static auto get(context &c) { return c.read_word(); }
	};

	template<>
	struct unwrap<instructions::d16_pointer>
	{
		constexpr static int complexity = 12;
		constexpr static int size_of = 1;
		constexpr static int is_pointer = 1;

		static auto get(context &c) { return c.memory.at(c.read_word()); }
	};

	template<>
	struct unwrap<instructions::SP_p_r8>
	{
		constexpr static int complexity = 4;
		constexpr static int size_of = 2;
		constexpr static int is_pointer = 0;

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