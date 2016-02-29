#pragma once
#include "operands.hpp"
#include "utils.hpp"
#include <tuple>
#include <type_traits>

namespace yagbe
{
namespace operands
{
namespace automap
{



	using register_types = std::tuple<B, C, D, E, H, L, HL_pointer, A>;


	constexpr bool in_range(uint8_t op, uint8_t mi, uint8_t ma)
	{
		return op >= mi && op <= ma;
	}

	constexpr bool in_column_4(uint8_t op, uint8_t start)
	{
		return op == op == start ||
			op == (start + 0x10) ||
			op == (start + 0x20) ||
			op == (start + 0x30);
	}


	template<uint8_t op, typename = std::enable_if_t<true>>
	struct operand
	{

	};

	//0x0 : NOP
	template<>
	struct operand<0x0> : operands::NOP	{};

	//0x10 : NOP
	template<>
	struct operand<0x10> : operands::STOP {};

	

	//0x40-0x7F (not 0x76) : LD X,y
	template<uint8_t op>
	struct operand<op, std::enable_if_t< in_range(op, 0x40, 0x7F) && op != 0x76 > > :
		operands::LD<
			std::tuple_element_t<(op - 0x40) / 8, register_types>, 
			std::tuple_element_t<op % 8, register_types>
			>
	{

	};


	template<>
	struct operand<0x76> : operands::HALT{};


	//--- JUMPS 

	//0xCA : JP Z, a16
	template<>
	struct operand<0xCA> : operands::JP<condition::Z, d16> {};

	//0xDA : JP C, a16
	template<>
	struct operand<0xDA> : operands::JP<condition::C, d16> {};

	//0xC2 : JP NZ, a16
	template<>
	struct operand<0xC2> : operands::JP<condition::NZ, d16> {};

	//0xD2 : JP NC, a16
	template<>
	struct operand<0xD2> : operands::JP<condition::NC, d16> {};

	//0xC3 : JP a16
	template<>
	struct operand<0xC3> : operands::JP<condition::_, d16> {};

	//0xE9 : JP (HL)
	template<>
	struct operand<0xE9> : operands::JP<condition::_, HL> {};

	//0xCA : JR Z, r8
	template<>
	struct operand<0x28> : operands::JR<condition::Z, d8> {};

	//0xDA : JR C, r8
	template<>
	struct operand<0x38> : operands::JR<condition::C, d8> {};

	//0xC2 : JR NZ, r8
	template<>
	struct operand<0x20> : operands::JR<condition::NZ, d8> {};

	//0xD2 : JR NC, r8
	template<>
	struct operand<0x30> : operands::JR<condition::NC, d8> {};

	//0xC3 : JR r8
	template<>
	struct operand<0x18> : operands::JR<condition::_, d8> {};




}
}
}