#pragma once
#include "instructions.hpp"
#include "instructions_cb.hpp"
#include "utils.hpp"
#include <tuple>
#include <type_traits>

namespace yagbe
{
namespace instructions
{
namespace automap
{



	using register_types = std::tuple<B, C, D, E, H, L, HL_pointer, A>;
	using register_BC_DE_HL_SP = std::tuple<BC, DE, HL, SP>;
	using register_BC_DE_HL_AF = std::tuple<BC, DE, HL, AF>;
	using pointers_BC_DE_HLI_HLD = std::tuple<BC_pointer, DE_pointer, HLI, HLD>;


	using register_B_D_H_HLP = std::tuple<B, D, H, HL_pointer>;
	using register_C_E_L_A = std::tuple<C, E, L, A>;


	constexpr bool in_range(uint8_t op, uint8_t mi, uint8_t ma)
	{
		return op >= mi && op <= ma;
	}

	constexpr uint8_t row_8(uint8_t op)
	{
		return op / 8;
	}

	constexpr uint8_t col_8(uint8_t op)
	{
		return op % 8;
	}

	constexpr uint8_t row_16(uint8_t op)
	{
		return op / 16;
	}

	constexpr uint8_t col_16(uint8_t op)
	{
		return op % 16;
	}

	template<uint8_t op, typename = std::enable_if_t<true>>
	struct instruction
	{

	};

	//--- first 4 rows


	//x0 column (from 0)
	template<> struct instruction<0x0> : instructions::NOP	{};
	template<> struct instruction<0x10> : instructions::STOP {};
	template<> struct instruction<0x20> : instructions::JR<condition::NZ, r8> {};
	template<> struct instruction<0x30> : instructions::JR<condition::NC, r8> {};

	//x1 column (from 0)
	template<uint8_t op>
	struct instruction<op, std::enable_if_t< 
			col_16(op) == 0x1 && row_16(op) < 4
		>> : instructions::LD<
			std::tuple_element_t<row_16(op), register_BC_DE_HL_SP>,
			d16
		>{};

	static_assert(instruction<0x1>::cycles() == 12, "wrong cycles");

	//x2 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x2 && row_16(op) < 4
		>> : instructions::LD<
			std::tuple_element_t<row_16(op), pointers_BC_DE_HLI_HLD>,
			A
		>{};

	static_assert(instruction<0x2>::cycles() == 8, "wrong cycles");

	//x3 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x3 && row_16(op) < 4
		>> : instructions::INC<
			std::tuple_element_t<row_16(op), register_BC_DE_HL_SP>
		>{};

	static_assert(instruction<0x3>::cycles() == 8, "wrong cycles");

	//x4 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x4 && row_16(op) < 4
		>> : instructions::INC<
		std::tuple_element_t<row_16(op), register_B_D_H_HLP>
		>{};

	static_assert(instruction<0x24>::cycles() == 4, "wrong cycles");
	static_assert(instruction<0x34>::cycles() == 12, "wrong cycles");

	//x5 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x5 && row_16(op) < 4
		>> : instructions::DEC<
		std::tuple_element_t<row_16(op), register_B_D_H_HLP>
		>{};

	static_assert(instruction<0x25>::cycles() == 4, "wrong cycles");
	static_assert(instruction<0x35>::cycles() == 12, "wrong cycles");

	//x6 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x6 && row_16(op) < 4
		>> : instructions::LD<
		std::tuple_element_t<row_16(op), register_B_D_H_HLP>,
		d8
		>{};

	static_assert(instruction<0x26>::cycles() == 8, "wrong cycles");
	static_assert(instruction<0x36>::cycles() == 12, "wrong cycles");

	//x7 column
	template<> struct instruction<0x07> : instructions::RLCA {};
	template<> struct instruction<0x17> : instructions::RLA {};
	template<> struct instruction<0x27> : instructions::DAA {};
	template<> struct instruction<0x37> : instructions::SCF {};

	//x8 column
	template<> struct instruction<0x08> : instructions::LD<d16_pointer, SP> {}; static_assert(LD<d16_pointer, SP>::cycles() == 20, "wrong cycles");
	template<> struct instruction<0x18> : instructions::JR<condition::_, r8> {};
	template<> struct instruction<0x28> : instructions::JR<condition::Z, r8> {};
	template<> struct instruction<0x38> : instructions::JR<condition::C, r8> {};

	//x9 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x9 && row_16(op) < 4
		>> : instructions::ADD<
		HL,
		std::tuple_element_t<row_16(op), register_BC_DE_HL_SP>
		>{};

	static_assert(instruction<0x9>::cycles() == 8, "wrong cycles");


	//xA column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0xA && row_16(op) < 4
		>> : instructions::LD<
		A,
		std::tuple_element_t<row_16(op), pointers_BC_DE_HLI_HLD>
		>{};

	//xB column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0xB && row_16(op) < 4
		>> : instructions::DEC<
		std::tuple_element_t<row_16(op), register_BC_DE_HL_SP>
		>{};

	//xC column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0xC && row_16(op) < 4
		>> : instructions::INC<
		std::tuple_element_t<row_16(op), register_C_E_L_A>
		>{};

	//xD column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0xD && row_16(op) < 4
		>> : instructions::DEC<
		std::tuple_element_t<row_16(op), register_C_E_L_A>
		>{};

	//xE column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0xE && row_16(op) < 4
		>> : instructions::LD<
		std::tuple_element_t<row_16(op), register_C_E_L_A>,
		d8
		>{};

	static_assert(instruction<0x0E>::cycles() == 8, "wrong cycles");

	//xF column
	template<> struct instruction<0x0F> : instructions::RRCA {};
	template<> struct instruction<0x1F> : instructions::RRA {};
	template<> struct instruction<0x2F> : instructions::CPL {};
	template<> struct instruction<0x3F> : instructions::CCF {};


	//--- second 4 rows

	template<> struct instruction<0x76> : instructions::HALT {};

	//0x40-0x7F (not 0x76) : LD X,y
	template<uint8_t op>
	struct instruction<op, std::enable_if_t< in_range(op, 0x40, 0x7F) && op != 0x76 > > :
		instructions::LD<
			std::tuple_element_t<row_8(op - 0x40), register_types>,
			std::tuple_element_t<col_8(op), register_types>
			>
	{

	};



	//--- third 4 rows
	template<typename Arg> struct ADD_A : ADD<A, Arg> {};

	template<typename Arg>
	using arithmetic_instructions_unary = std::tuple< ADD_A<Arg>, ADC<Arg>, SUB<Arg>, SBC<Arg>, AND<Arg>, XOR<Arg>, OR<Arg>, CP<Arg>>;


	template<uint8_t op>
	struct instruction<op, std::enable_if_t< in_range(op, 0x80, 0xBF) >> :
		std::tuple_element_t<row_8(op - 0x80), arithmetic_instructions_unary< std::tuple_element_t<col_8(op), register_types>  > >
	{

	};


	//--- fourth 4 rows

	//x0 column
	template<> struct instruction<0xC0> : instructions::RET<condition::NZ> {}; static_assert(RET<condition::NZ>::cycles() == 20, "wrong cycles");
	template<> struct instruction<0xD0> : instructions::RET<condition::NC> {};
	template<> struct instruction<0xE0> : instructions::LD<d8_pointer, A> {}; static_assert(LD<d8_pointer, A>::cycles() == 12, "wrong cycles");
	template<> struct instruction<0xF0> : instructions::LD<A, d8_pointer> {};


	//x1 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x1 && row_16(op) >= 0xC
		>> : instructions::POP<
			std::tuple_element_t<row_16(op - 0xC0), register_BC_DE_HL_AF>
		>{};

	//x2 column
	template<> struct instruction<0xC2> : instructions::JP<condition::NZ, d16> {};
	template<> struct instruction<0xD2> : instructions::JP<condition::NC, d16> {};
	template<> struct instruction<0xE2> : instructions::LD<C_pointer, A> {}; static_assert(LD<C_pointer, A>::cycles() == 8, "wrong cycles");
	template<> struct instruction<0xF2> : instructions::LD<A, C_pointer> {};

	//x3 column
	template<> struct instruction<0xC3> : instructions::JP<condition::_, d16> {};
	template<> struct instruction<0xD3> : instructions::UNDEFINED {};
	template<> struct instruction<0xE3> : instructions::UNDEFINED {};
	template<> struct instruction<0xF3> : instructions::DI {};

	//x4 column
	template<> struct instruction<0xC4> : instructions::CALL<condition::NZ, d16> {};
	template<> struct instruction<0xD4> : instructions::CALL<condition::NC, d16> {};
	template<> struct instruction<0xE4> : instructions::UNDEFINED {};
	template<> struct instruction<0xF4> : instructions::UNDEFINED {};

	//x5 column
	template<uint8_t op>
	struct instruction<op, std::enable_if_t<
		col_16(op) == 0x5 && row_16(op) >= 0xC
		>> : instructions::PUSH<
		std::tuple_element_t<row_16(op - 0xC0), register_BC_DE_HL_AF>
		>{};

	//x6 column
	template<> struct instruction<0xC6> : instructions::ADD<A, d8> {};
	template<> struct instruction<0xD6> : instructions::SUB<d8> {};
	template<> struct instruction<0xE6> : instructions::AND<d8> {};
	template<> struct instruction<0xF6> : instructions::OR <d8> {};

	//x7 column	
	template<> struct instruction<0xC7> : instructions::RST<0x00> {};
	template<> struct instruction<0xD7> : instructions::RST<0x10> {};
	template<> struct instruction<0xE7> : instructions::RST<0x20> {};
	template<> struct instruction<0xF7> : instructions::RST<0x30> {};

	//x8 column
	template<> struct instruction<0xC8> : instructions::RET<condition::Z> {};
	template<> struct instruction<0xD8> : instructions::RET<condition::C> {};
	template<> struct instruction<0xE8> : instructions::ADD<SP, r8_as_word> {}; static_assert(ADD<SP, r8_as_word>::cycles() == 16, "wrong cycles");
	template<> struct instruction<0xF8> : instructions::LD<HL, SP_p_r8> {}; static_assert(LD<HL, SP_p_r8>::cycles() == 12, "wrong cycles"); 

	//x9 column
	template<> struct instruction<0xC9> : instructions::RET<> {};
	template<> struct instruction<0xD9> : instructions::RETI {};
	template<> struct instruction<0xE9> : instructions::JP<condition::_, HL> {};
	template<> struct instruction<0xF9> : instructions::LD<SP, HL> {}; static_assert(instructions::LD<SP, HL>::cycles() == 8, "wrong cycles");

	//xA column
	template<> struct instruction<0xCA> : instructions::JP<condition::Z, d16> {};
	template<> struct instruction<0xDA> : instructions::JP<condition::C, d16> {};
	template<> struct instruction<0xEA> : instructions::LD<d16_pointer, A> {}; static_assert(LD<d16_pointer, A>::cycles() == 16, "wrong cycles");
	template<> struct instruction<0xFA> : instructions::LD<A, d16_pointer> {}; static_assert(LD<A, d16_pointer>::cycles() == 16, "wrong cycles");

	//xB column
	template<> struct instruction<0xCB> : instructions::CB {};
	template<> struct instruction<0xDB> : instructions::UNDEFINED {};
	template<> struct instruction<0xEB> : instructions::UNDEFINED {};
	template<> struct instruction<0xFB> : instructions::EI {};

	//xC column
	template<> struct instruction<0xCC> : instructions::CALL<condition::Z, d16> {};
	template<> struct instruction<0xDC> : instructions::CALL<condition::C, d16> {};
	template<> struct instruction<0xEC> : instructions::UNDEFINED {};
	template<> struct instruction<0xFC> : instructions::UNDEFINED {};

	//xD column
	template<> struct instruction<0xCD> : instructions::CALL<condition::_, d16> {};
	template<> struct instruction<0xDD> : instructions::UNDEFINED {};
	template<> struct instruction<0xED> : instructions::UNDEFINED {};
	template<> struct instruction<0xFD> : instructions::UNDEFINED {};

	//xE column
	template<> struct instruction<0xCE> : instructions::ADC<d8> {}; static_assert(instructions::ADC<d8>::cycles() == 8, "wrong cycles");
	template<> struct instruction<0xDE> : instructions::SBC<d8> {};
	template<> struct instruction<0xEE> : instructions::XOR<d8> {};
	template<> struct instruction<0xFE> : instructions::CP <d8> {};

	//xF column	
	template<> struct instruction<0xCF> : instructions::RST<0x08> {};
	template<> struct instruction<0xDF> : instructions::RST<0x18> {};
	template<> struct instruction<0xEF> : instructions::RST<0x28> {};
	template<> struct instruction<0xFF> : instructions::RST<0x38> {};

	





	//--- CBMAP ------------------------------------

	template<uint8_t op, typename = std::enable_if_t<true>>
	struct instruction_cb
	{

	};

	//first 4 rows

	template<typename Arg>
	using cb_first_instructions = std::tuple< RLC<Arg>, RRC<Arg>, RL<Arg>, RR<Arg>, SLA<Arg>, SRA<Arg>, SWAP<Arg>, SRL<Arg>>;


	template<uint8_t op>
	struct instruction_cb<op, std::enable_if_t< in_range(op, 0x0, 0x3F) >> :
		std::tuple_element_t<row_8(op), cb_first_instructions< std::tuple_element_t<col_8(op), register_types>  > >
	{

	};

	template<uint8_t op>
	struct instruction_cb<op, std::enable_if_t< in_range(op, 0x40, 0x7F) >> :
		BIT<row_8(op-0x40), std::tuple_element_t<col_8(op), register_types> >
	{

	};

	template<uint8_t op>
	struct instruction_cb<op, std::enable_if_t< in_range(op, 0x80, 0xBF) >> :
		RES<row_8(op - 0x80), std::tuple_element_t<col_8(op), register_types> >
	{

	};

	template<uint8_t op>
	struct instruction_cb<op, std::enable_if_t< in_range(op, 0xC0, 0xFF) >> :
		SET<row_8(op - 0xC0), std::tuple_element_t<col_8(op), register_types> >
	{

	};


	//fillers


	namespace impl
	{
		//I yearn for constexpr for
		template<int n = 255>
		struct fill_instructions
		{
			static void fill(instructions_array &arr)
			{
				arr[n] = &(instruction<n>::execute);
				arr[n-1] = &(instruction<n-1>::execute);
				arr[n-2] = &(instruction<n-2>::execute);
				arr[n-3] = &(instruction<n-3>::execute);

				fill_instructions<n - 4>::fill(arr);
			}
		};

		template<>
		struct fill_instructions<3>
		{
			static void fill(instructions_array &arr)
			{
				arr[3] = &(instruction<3>::execute);
				arr[2] = &(instruction<2>::execute);
				arr[1] = &(instruction<1>::execute);
				arr[0] = &(instruction<0>::execute);
			}
		};

		template<int n = 255>
		struct fill_instructions_cb
		{
			static void fill(instructions_array &arr)
			{
				arr[n] = &(instruction_cb<n>::execute);
				arr[n-1] = &(instruction_cb<n-1>::execute);
				arr[n-2] = &(instruction_cb<n-2>::execute);
				arr[n-3] = &(instruction_cb<n-3>::execute);

				fill_instructions_cb<n - 4>::fill(arr);
			}
		};

		template<>
		struct fill_instructions_cb<3>
		{
			static void fill(instructions_array &arr)
			{
				arr[3] = &(instruction_cb<3>::execute);
				arr[2] = &(instruction_cb<2>::execute);
				arr[1] = &(instruction_cb<1>::execute);
				arr[0] = &(instruction_cb<0>::execute);
			}
		};
	}






	//CHECKS
	static_assert(&(instruction<0x47>::execute) == &(LD<B, A>::execute), "Wrong mapping");
	static_assert(&(instruction<0x7E>::execute) == &(LD<A, HL_pointer>::execute), "Wrong mapping");

	static_assert(&(instruction<0x80>::execute) == &(ADD<A, B>::execute), "Wrong mapping");
	static_assert(&(instruction<0xBF>::execute) == &(CP<A>::execute), "Wrong mapping");

	static_assert(&(instruction<0x95>::execute) == &(SUB<L>::execute), "Wrong mapping");

	static_assert(&(instruction<0xE1>::execute) == &(POP<HL>::execute), "Wrong mapping");
	static_assert(&(instruction<0x36>::execute) == &(LD<HL_pointer,d8>::execute), "Wrong mapping");

	//CB CHECKS
	static_assert(&(instruction_cb<0x0>::execute) == &(RLC<B>::execute), "Wrong mapping");
	static_assert(&(instruction_cb<0x3F>::execute) == &(SRL<A>::execute), "Wrong mapping");

	static_assert(&(instruction_cb<0x15>::execute) == &(RL<L>::execute), "Wrong mapping");

	static_assert(&(instruction_cb<0x40>::execute) == &(BIT<0,B>::execute), "Wrong mapping");
	static_assert(&(instruction_cb<0x7F>::execute) == &(BIT<7, A>::execute), "Wrong mapping");

	static_assert(&(instruction_cb<0x80>::execute) == &(RES<0, B>::execute), "Wrong mapping");
	static_assert(&(instruction_cb<0xBF>::execute) == &(RES<7, A>::execute), "Wrong mapping");

	static_assert(&(instruction_cb<0xC0>::execute) == &(SET<0, B>::execute), "Wrong mapping");
	static_assert(&(instruction_cb<0xFF>::execute) == &(SET<7, A>::execute), "Wrong mapping");


	static_assert(instruction_cb<0x05>::cycles() == 8, "wrong cycles");
	static_assert(instruction_cb<0x06>::cycles() == 16, "wrong cycles");

}
}
}