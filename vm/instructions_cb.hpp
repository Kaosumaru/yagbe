#pragma once
#include "instructions.hpp"


namespace yagbe
{

namespace instructions
{

	//RLC
	template<typename Arg>
	struct RLC : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			uint8_t carry = (value & 0x80) >> 7;

			c.flags.c = (value & 0x80) != 0;

			value <<= 1;
			value += carry;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SRA::cycles();
		}
	};

	//RRC
	template<typename Arg>
	struct RRC : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			uint8_t carry = value & 0x01;
			value >>= 1;

			if (carry)
			{
				c.flags.c = true;
				value |= 0x80;
			}
			else
				c.flags.c = false;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SRA::cycles();
		}
	};


	//RL
	template<typename Arg>
	struct RL : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto carry = c.flags.c ? 1 : 0;

			auto &value = unwrap<Arg>::get(c);
			c.flags.c = (value & 0x80) != 0;

			value <<= 1;
			value += carry;
			unwrap<Arg>::get(c) = value;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return RL::cycles();
		}
	};

	//RR
	template<typename Arg>
	struct RR : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			value >>= 1;
			if (c.flags.c)
				value |= 0x80;

			
			c.flags.c = (value & 0x01) != 0;
			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return RR::cycles();
		}
	};

	//SLA
	template<typename Arg>
	struct SLA : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			c.flags.c = (value & 0x80) != 0;
			value <<= 1;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SLA::cycles();
		}
	};

	//SRA
	template<typename Arg>
	struct SRA : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			c.flags.c = value & 0x01;

			value = (value & 0x80) | (value >> 1);

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SRA::cycles();
		}
	};

	//SWAP
	template<typename Arg>
	struct SWAP : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			value = ((value & 0xf) << 4) | ((value & 0xf0) >> 4);

			c.flags.c = 0;
			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SWAP::cycles();
		}
	};

	//SRL
	template<typename Arg>
	struct SRL : default_unary_instruction<1, 8, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto &value = unwrap<Arg>::get(c);
			c.flags.c = (value & 0x01) != 0;
			value >>= 1;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return SRL::cycles();
		}
	};


}
}