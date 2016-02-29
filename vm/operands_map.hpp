#pragma once
#include "operands.hpp"

namespace yagbe
{
namespace operands
{
namespace automap
{


	template<uint8_t number>
	struct type_from_number {	};

	template<>
	struct type_from_number<0> { using type = B; };

	template<uint8_t op>
	struct type_from_number<1> { using type = C; };

	template<uint8_t op>
	struct type_from_number<2> { using type = D; };

	template<uint8_t op>
	struct type_from_number<3> { using type = E; };

	template<uint8_t op>
	struct type_from_number<4> { using type = H; };

	template<uint8_t op>
	struct type_from_number<5> { using type = L; };

	template<uint8_t op>
	struct type_from_number<6> { using type = HL_pointer; };

	template<uint8_t op>
	struct type_from_number<7> { using type = A; };

	template<uint8_t op>
	using type_from_number_t = typename type_from_number<op>::type;

	constexpr bool in_range(uint8_t op, uint8_t mi, uint8_t ma)
	{
		return op >= mi && op <= ma;
	}

	template<uint8_t op, typename = std::enable_if_t<true>>
	struct operand
	{

	};

	//0x0 : NOP
	template<>
	struct operand<0> : operands::NOP
	{

	};




	//0x40-0x7F (not 0x76) : LD X,y
	template<uint8_t op>
	struct operand<op, std::enable_if_t< in_range(op, 0x40, 0x7F) && op != 0x76 > > : operands::LD<automap::type_from_number_t<(op - 0xd0)/8>, automap::type_from_number_t<op % 8> >
	{

	};


	template<>
	struct operand<0x76> : operands::HALT
	{

	};

}
}
}