#pragma once
#include "operands.hpp"

namespace yagbe
{
namespace operands
{
namespace automap
{


	template<uint8_t operand, typename = std::enable_if_t<true>>
	struct type_from_op {	};

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 0 > > { using type = B; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 1 > > { using type = C; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 2 > > { using type = D; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 3 > > { using type = E; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 4 > > { using type = H; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 5 > > { using type = L; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 6 > > { using type = HL_pointer; };

	template<uint8_t op>
	struct type_from_op<op, std::enable_if_t< op % 8 == 7 > > { using type = A; };

	template<uint8_t op>
	using type_from_op_t = typename type_from_op<op>::type;

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

	//0x40-0x47 : LD B,X
	template<uint8_t op>
	struct operand<op, std::enable_if_t< in_range(op, 0x40, 0x47) > > : operands::LD<B, automap::type_from_op_t<op> >
	{

	};

}
}
}