#pragma once
#include "context.hpp"
#include "operands_helpers.hpp"

namespace yagbe
{

namespace operands
{
	//operands
	template<uint8_t s, uint8_t c>
	struct default_operand
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c; }
	};

	template<uint8_t s, uint8_t c, typename Arg>
	struct default_unary_operand
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap<Arg>::complexity; }
	};

	template<uint8_t s, uint8_t c, typename Arg1, typename Arg2>
	struct default_binary_operand
	{
		static_assert(unwrap<Arg1>::size_of == unwrap<Arg2>::size_of, "Mismatching sizes");

		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap<Arg1>::complexity + unwrap<Arg2>::complexity; }
	};

	//--- MISC 1

	//NOP
	struct NOP : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			return cycles();
		}
	};

	//HALT
	struct HALT : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			throw std::runtime_error("NYI");
			return cycles();
		}
	};

	//STOP
	struct STOP : default_operand<2, 4>
	{
		static int execute(context &c)
		{
			c.read_byte();
			throw std::runtime_error("NYI");
			return cycles();
		}
	};


	//--- REGISTERS & FLAGS

	//RLCA
	struct RLCA : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			uint16_t carry = (c.registers.a & 0x80) >> 7;

			c.registers.a <<= 1;
			c.registers.a += carry;

			c.flags.z = 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = carry != 0;

			return cycles();
		}
	};

	//RLA
	struct RLA : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			uint8_t carry = c.flags.c ? 1 : 0;

			c.flags.z = 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = c.registers.a & 0x80;

			c.registers.a <<= 1;
			c.registers.a += carry;

			return cycles();
		}
	};

	//DAA
	struct DAA : default_operand<1, 4>
	{


		static int execute(context &c)
		{
			uint16_t s = c.registers.a;

			//TODO check this

			if (c.flags.n) {
				if (c.flags.h) s = (s - 0x06) & 0xFF;
				if (c.flags.c) s -= 0x60;
			}
			else {
				if (c.flags.h || (s & 0xF) > 9) s += 0x06;
				if (c.flags.c || s > 0x9F) s += 0x60;
			}

			c.registers.a = s;

			c.flags.z = c.registers.a == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			if (s > 0x100)
				c.flags.c = 1;

			return cycles();
		}
	};

	//SCF
	struct SCF : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = 1;

			return cycles();
		}
	};

	//RRCA
	struct RRCA : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			uint8_t carry = c.registers.a & 0x01;

			c.registers.a >>= 1;
			if (carry) c.registers.a |= 0x80;

			c.flags.z = 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = carry != 0;

			return cycles();
		}
	};

	//RRA
	struct RRA : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			uint8_t carry = (c.flags.c ? 1 : 0) << 7;

			c.flags.z = 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = c.registers.a & 0x01;

			c.registers.a >>= 1;
			c.registers.a += carry;

			return cycles();
		}
	};

	//CPL
	struct CPL : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			c.registers.a = ~c.registers.a;

			c.flags.n = 0;
			c.flags.h = 0;

			return cycles();
		}
	};

	//CCF
	struct CCF : default_operand<1, 4>
	{
		static int execute(context &c)
		{
			c.flags.c = !c.flags.c;

			c.flags.n = 0;
			c.flags.h = 0;

			return cycles();
		}
	};


/*
	//SRA
	template<typename Arg>
	struct SRA : default_unary_operand<1, 4, Arg>
	{
		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			c.flags.c = value & 0x01;

			value = (value & 0x80) | (value >> 1);
			unwrap<Arg>::get(c) = value;

			c.flags.z = value == 0;
			c.flags.n = 0;
			c.flags.h = 0;

			return cycles();
		}
	};
*/

	//--- LOADS

	//this allows to type LD<B,A>::execute, or LD<A,B>::execute, etc
	template<typename Arg1, typename Arg2>
	struct LD : default_binary_operand<1, 4, Arg1, Arg2>
	{
		static int execute(context &c)
		{
			unwrap<Arg1>::get(c) = unwrap<Arg2>::get(c);
			return cycles();
		}
	};


	//PUSH
	template<typename Arg>
	struct PUSH : default_operand<1, 16>
	{
		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			union
			{
				uint8_t bytes[2];
				uint16_t word;
			};

			word = unwrap<Arg>::get(c);

			//TODO endianess
			c.memory.at(c.registers.sp--) = bytes[0];
			c.memory.at(c.registers.sp--) = bytes[1];

			return cycles();
		}
	};

	//POP
	template<typename Arg>
	struct POP : default_operand<1, 16>
	{
		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			union
			{
				uint8_t bytes[2];
				uint16_t word;
			};

			//TODO endianess
			bytes[1] = c.memory.at(c.registers.sp++);
			bytes[0] = c.memory.at(c.registers.sp++);

			unwrap<Arg>::get(c) = word;

			return cycles();
		}
	};


	//--- ARITHMETIC (8 & 16 bit)

	//INC
	template<typename Arg>
	struct INC : default_unary_operand<1, 4, Arg>
	{
		static int execute(context &c)
		{
			bool byte = unwrap<Arg>::size_of == 1;
			auto value = unwrap<Arg>::get(c);

			if (byte)
			{
				c.flags.h = ((value & 0x0f) == 0x0f);
			}

			value++;
			unwrap<Arg>::get(c) = value;

			if (byte)
			{
				c.flags.z = (value == 0);
				c.flags.n = 0;
			}

			return cycles();
		}
	};

	//DEC
	template<typename Arg>
	struct DEC : default_unary_operand<1, 4, Arg>
	{
		static int execute(context &c)
		{
			bool byte = unwrap<Arg>::size_of == 1;
			auto value = unwrap<Arg>::get(c);

			if (byte)
			{
				c.flags.h = !(value & 0x0f); //TODO is this correct?
			}

			value--;
			unwrap<Arg>::get(c) = value;

			if (byte)
			{
				c.flags.z = (value == 0);
				c.flags.n = 1;
			}

			return cycles();
		}
	};


	//ADD (8bit, 16 bit)
	template<typename Arg1, typename Arg2>
	struct ADD : default_binary_operand<1, 4, Arg1, Arg2>
	{
		static int execute(context &c)
		{
			constexpr bool single_byte = unwrap<Arg1>::size_of == 1;
			using overflow_result_type = std::conditional_t<single_byte, uint16_t, uint32_t>;
			using result_type = std::conditional_t<single_byte, uint8_t, uint16_t>;


			auto arg1 = unwrap<Arg1>::get(c);
			auto arg2 = unwrap<Arg2>::get(c);
			overflow_result_type result = arg1 + arg2;


			unwrap<Arg1>::get(c) = (result_type)(result);

			c.flags.z = (result == 0);
			c.flags.n = 0;
			c.flags.h = ((result)+(arg2 & 0x0f)) > 0x0f;

			overflow_result_type cmask = single_byte ? 0xff00 : 0xffff0000;
			c.flags.c = result & cmask;

			return cycles();
		}
	};

	//ADC
	template<typename Arg>
	struct ADC : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			value += c.flags.c ? 1 : 0;

			uint16_t result = (uint16_t)c.registers.a + (uint16_t)value;

			c.flags.z = value == c.registers.a;
			c.flags.n = false;
			c.flags.h = ((value & 0x0f) + (registers.a & 0x0f)) > 0x0f;
			c.flags.c = result & 0xff00;

			registers.a = (uint8_t)result;

			return cycles();
		}
	};

	//SBC
	template<typename Arg>
	struct SBC : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			value += c.flags.c ? 1 : 0;

			
			c.flags.z = value == c.registers.a;
			c.flags.n = true;
			c.flags.h = (value & 0x0f) > (c.registers.a & 0x0f);
			c.flags.c = value > c.registers.a;

			registers.a -= value;

			return cycles();
		}
	};

	//SUB
	template<typename Arg>
	struct SUB : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);

			c.flags.z = value == c.registers.a;
			c.flags.n = true;
			c.flags.h = (value & 0x0f) > (c.registers.a & 0x0f);
			c.flags.c = value > c.registers.a;
			
			registers.a -= value;

			return cycles();
		}
	};

	//AND
	template<typename Arg>
	struct AND : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			c.registers.a &= value;

			c.flags.z = c.registers.a == 0;
			c.flags.n = 0;
			c.flags.h = 1;
			c.flags.c = 0;

			return cycles();
		}
	};

	//XOR
	template<typename Arg>
	struct XOR : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			c.registers.a ^= value;

			c.flags.z = c.registers.a == 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = 0;

			return cycles();
		}
	};

	//OR
	template<typename Arg>
	struct OR : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			c.registers.a |= value;

			c.flags.z = c.registers.a == 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = 0;

			return cycles();
		}
	};

	//CP
	template<typename Arg>
	struct CP : default_unary_operand<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);

			c.flags.z = value == c.registers.a;
			c.flags.n = true;
			c.flags.h = (value & 0x0f) > (c.registers.a & 0x0f);
			c.flags.c = value > c.registers.a;

			return cycles();

		}
	};

	//--- JUMPS
	template<typename Cond, typename Arg>
	struct JP
	{
		constexpr static int size() { return std::is_same<Arg, operands::d16>::value ? 3 : 1; }
		constexpr static int cycles() { return std::is_same<Arg, operands::d16>::value ? 16 : 4; }

		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			auto pointer = unwrap<Arg>::get(c);

			if (!unwrap<Cond>::is_true(c))
				return 12;

			c.registers.pc = pointer;
			return cycles();
		}
	};

	template<typename Cond, typename Arg>
	struct JR
	{
		constexpr static int size() { return 2; }
		constexpr static int cycles() { return 12; }

		static_assert(std::is_same<Arg, operands::d8>::value, "Assuming d8");

		static int execute(context &c)
		{
			auto pointer = unwrap<Arg>::get(c);

			if (!unwrap<Cond>::is_true(c))
				return 8;

			c.registers.pc += (int8_t)pointer;
			return cycles();
		}
	};



}
}