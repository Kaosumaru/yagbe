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
	using register_BC_DE_HL_SP = std::tuple<BC, DE, HL, SP>;
	using pointers_BC_DE_HLI_HLD = std::tuple<BC_pointer, DE_pointer, HLI, HLD>;


	using register_B_D_H_HLP = std::tuple<B, D, H, HL_pointer>;
	using register_C_E_L_A = std::tuple<C, E, L, A>;


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

	constexpr uint8_t row(uint8_t op)
	{
		return op / 8;
	}

	constexpr uint8_t col(uint8_t op)
	{
		return op % 8;
	}

	template<uint8_t op, typename = std::enable_if_t<true>>
	struct operand
	{

	};

	//--- first 4 rows


	//x0 column (from 0)
	template<> struct operand<0x0> : operands::NOP	{};
	template<> struct operand<0x10> : operands::STOP {};
	template<> struct operand<0x20> : operands::JR<condition::NZ, d8> {};
	template<> struct operand<0x30> : operands::JR<condition::NC, d8> {};

	//x1 column (from 0)
	template<uint8_t op>
	struct operand<op, std::enable_if_t< 
			col(op) == 0x1 && row(op) < 4
		>> : operands::LD<
			std::tuple_element_t<row(op), register_BC_DE_HL_SP>,
			d16
		>{};

	//x2 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x2 && row(op) < 4
		>> : operands::LD<
			std::tuple_element_t<row(op), pointers_BC_DE_HLI_HLD>,
			A
		>{};

	//x3 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x3 && row(op) < 4
		>> : operands::INC<
			std::tuple_element_t<row(op), register_BC_DE_HL_SP>
		>{};

	//x4 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x4 && row(op) < 4
		>> : operands::INC<
		std::tuple_element_t<row(op), register_B_D_H_HLP>
		>{};

	//x5 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x5 && row(op) < 4
		>> : operands::DEC<
		std::tuple_element_t<row(op), register_B_D_H_HLP>
		>{};

	//x6 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x6 && row(op) < 4
		>> : operands::LD<
		std::tuple_element_t<row(op), register_B_D_H_HLP>,
		d8
		>{};

	//x7 column
	template<> struct operand<0x07> : operands::RLCA {};
	template<> struct operand<0x17> : operands::RLA {};
	template<> struct operand<0x27> : operands::DAA {};
	template<> struct operand<0x37> : operands::SCF {};

	//x8 column
	template<> struct operand<0x08> : operands::LD<d16_pointer, SP> {}; static_assert(LD<d16_pointer, SP>::cycles() == 20, "wrong cycles");
	template<> struct operand<0x18> : operands::JR<condition::_, d8> {};
	template<> struct operand<0x28> : operands::JR<condition::Z, d8> {};
	template<> struct operand<0x38> : operands::JR<condition::C, d8> {};

	//x9 column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0x9 && row(op) < 4
		>> : operands::ADD<
		HL,
		std::tuple_element_t<row(op), register_BC_DE_HL_SP>
		>{};

	//xA column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0xA && row(op) < 4
		>> : operands::LD<
		A,
		std::tuple_element_t<row(op), pointers_BC_DE_HLI_HLD>
		>{};

	//xB column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0xB && row(op) < 4
		>> : operands::DEC<
		std::tuple_element_t<row(op), register_BC_DE_HL_SP>
		>{};

	//xC column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0xC && row(op) < 4
		>> : operands::INC<
		std::tuple_element_t<row(op), register_C_E_L_A>
		>{};

	//xD column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0xD && row(op) < 4
		>> : operands::DEC<
		std::tuple_element_t<row(op), register_C_E_L_A>
		>{};

	//xE column
	template<uint8_t op>
	struct operand<op, std::enable_if_t<
		col(op) == 0xE && row(op) < 4
		>> : operands::LD<
		std::tuple_element_t<row(op), register_C_E_L_A>,
		d8
		>{};

	//xF column
	template<> struct operand<0x0F> : operands::RRCA {};
	template<> struct operand<0x1F> : operands::RRA {};
	template<> struct operand<0x2F> : operands::CPL {};
	template<> struct operand<0x3F> : operands::CCF {};


	//--- second 4 rows

	//0x40-0x7F (not 0x76) : LD X,y
	template<uint8_t op>
	struct operand<op, std::enable_if_t< in_range(op, 0x40, 0x7F) && op != 0x76 > > :
		operands::LD<
			std::tuple_element_t<row(op - 0x40), register_types>,
			std::tuple_element_t<col(op), register_types>
			>
	{

	};

	//--- third 4 rows
	template<typename Arg> struct ADD_A : ADD<A, Arg> {};

	template<typename Arg>
	using arithmetic_operands_unary = std::tuple< ADD_A<Arg>, ADC<Arg>, SUB<Arg>, SBC<Arg>, AND<Arg>, XOR<Arg>, OR<Arg>, CP<Arg>>;


	template<uint8_t op>
	struct operand<op, std::enable_if_t< in_range(op, 0x80, 0xBF) >> :
		std::tuple_element_t<col(op), arithmetic_operands_unary< std::tuple_element_t<row(op - 0x80), register_types>  > >
	{

	};


	//--- fourth 4 rows
	template<> struct operand<0x76> : operands::HALT{};
	//--- JUMPS 

	//0xCA : JP Z, a16
	template<> struct operand<0xCA> : operands::JP<condition::Z, d16> {};

	//0xDA : JP C, a16
	template<> struct operand<0xDA> : operands::JP<condition::C, d16> {};

	//0xC2 : JP NZ, a16
	template<> struct operand<0xC2> : operands::JP<condition::NZ, d16> {};

	//0xD2 : JP NC, a16
	template<> struct operand<0xD2> : operands::JP<condition::NC, d16> {};

	//0xC3 : JP a16
	template<> struct operand<0xC3> : operands::JP<condition::_, d16> {};

	//0xE9 : JP (HL)
	template<> struct operand<0xE9> : operands::JP<condition::_, HL> {};





}
}
}