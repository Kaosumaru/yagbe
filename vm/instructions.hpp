#pragma once
#include <type_traits>
#include "context.hpp"
#include "instructions_helpers.hpp"
#include "utils.hpp"


namespace yagbe
{

namespace instructions
{


	//instructions
	template<uint8_t s, uint8_t c>
	struct default_instruction
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c; }
	};

	template<uint8_t s, uint8_t c, typename Arg>
	struct default_unary_instruction
	{
		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap<Arg>::complexity + unwrap<Arg>::is_pointer * 4; }
	};

	template<uint8_t s, uint8_t c, typename Arg1, typename Arg2>
	struct default_binary_instruction
	{
		static_assert(unwrap<Arg1>::size_of == unwrap<Arg2>::size_of, "Mismatching sizes");

		constexpr static int size() { return s; }
		constexpr static int cycles() { return c + unwrap<Arg1>::complexity + unwrap<Arg2>::complexity; }
	};

	//--- MISC 1

	//UNDEFINED
	struct UNDEFINED : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			return UNDEFINED::cycles();
		}
	};

	//NOP
	struct NOP : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			return NOP::cycles();
		}
	};

	//HALT
	struct HALT : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			if ((c.memory.io_register.IE & c.memory.io_register.IF & 0x1F) > 0)
				return HALT::cycles();

			c.halted = true;
			return HALT::cycles();
		}
	};

	//STOP
	struct STOP : default_instruction<2, 4>
	{
		static int execute(context &c)
		{
			c.read_byte();
#ifdef _DEBUG
			throw std::runtime_error("NYI");
#endif
			return STOP::cycles();
		}
	};


	//--- REGISTERS & FLAGS

	//RLCA
	struct RLCA : default_instruction<1, 4>
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

			return RLCA::cycles();
		}
	};

	//RLA
	struct RLA : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			uint8_t carry = c.flags.c ? 1 : 0;

			c.flags.z = 0;
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = (c.registers.a & 0x80) != 0;

			c.registers.a <<= 1;
			c.registers.a += carry;

			return RLA::cycles();
		}
	};

	//DAA
	struct DAA : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			if (!c.flags.n) {
				if (c.flags.c || c.registers.a > 0x99) {
					c.registers.a = (c.registers.a + 0x60);
					c.flags.c = true;
				}
				if (c.flags.h || (c.registers.a & 0xF) > 0x9) {
					c.registers.a = (c.registers.a + 0x06);
					c.flags.h = false;
				}
			}
			else if (c.flags.c && c.flags.h) {
				c.registers.a = (c.registers.a + 0x9A);
				c.flags.h = false;
			}
			else if (c.flags.c) {
				c.registers.a = (c.registers.a + 0xA0);
			}
			else if (c.flags.h) {
				c.registers.a = (c.registers.a + 0xFA);
				c.flags.h = false;
			}
			c.flags.z = (c.registers.a == 0);

			return DAA::cycles();
		}
	};

	//SCF
	struct SCF : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			c.flags.n = 0;
			c.flags.h = 0;
			c.flags.c = 1;

			return SCF::cycles();
		}
	};

	//RRCA
	struct RRCA : default_instruction<1, 4>
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

			return RRCA::cycles();
		}
	};

	//RRA
	struct RRA : default_instruction<1, 4>
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

			return RRA::cycles();
		}
	};

	//CPL
	struct CPL : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			c.registers.a = ~c.registers.a;

			c.flags.n = 1;
			c.flags.h = 1;

			return CPL::cycles();
		}
	};

	//CCF
	struct CCF : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			c.flags.c = !c.flags.c;

			c.flags.n = 0;
			c.flags.h = 0;

			return CCF::cycles();
		}
	};

	//--- LOADS

	//this allows to type LD<B,A>::execute, or LD<A,B>::execute, etc
	template<typename Arg1, typename Arg2>
	struct LD : default_binary_instruction<1, 4, Arg1, Arg2>
	{
		static int execute(context &c)
		{
			unwrap<Arg1>::get(c) = unwrap<Arg2>::get(c);
			return LD::cycles();
		}
	};

	//specialized LD<SP, HL> to change cycles to 8
	template<>
	struct LD<SP, HL> : default_instruction<1, 8>
	{
		static int execute(context &c)
		{
			unwrap<SP>::get(c) = unwrap<HL>::get(c);
			return LD::cycles();
		}
	};


	//specialized LD<d16_pointer, SP> to allow writing word at address
	template<>
	struct LD<d16_pointer, SP> : default_instruction<3, 20>
	{
		static int execute(context &c)
		{
			auto address = c.read_word();
			c.memory.write_word_at(address, c.registers.sp);
			return LD::cycles();
		}
	};

	//PUSH
	template<typename Arg>
	struct PUSH : default_instruction<1, 16>
	{
		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			c.push(unwrap<Arg>::get(c));
			return PUSH::cycles();
		}
	};

	//POP
	template<typename Arg>
	struct POP : default_instruction<1, 12>
	{
		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			unwrap<Arg>::get(c) = c.pop();
			return POP::cycles();
		}
	};

	template<>
	struct POP<AF> : default_instruction<1, 12>
	{
		static_assert(unwrap<AF>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			unwrap<AF>::get(c) = c.pop();
			c.registers.f &= 0xF0;
			return POP::cycles();
		}
	};


	//--- ARITHMETIC (8 & 16 bit)

	//INC
	template<typename Arg>
	struct INC : default_unary_instruction<1, 4, Arg>
	{
		static int execute(context &c)
		{
			bool byte = unwrap<Arg>::size_of == 1;
			auto &&value = unwrap<Arg>::get(c);

			if (byte)
			{
				c.flags.h = ((value & 0x0f) == 0x0f);
			}

			value += 1;

			if (byte)
			{
				c.flags.z = (value == 0);
				c.flags.n = 0;
			}

			return INC::cycles();
		}
	};

	//DEC
	template<typename Arg>
	struct DEC : default_unary_instruction<1, 4, Arg>
	{
		static int execute(context &c)
		{
			bool byte = unwrap<Arg>::size_of == 1;
			auto &&value = unwrap<Arg>::get(c);

			value-=1;

			if (byte)
			{
				c.flags.h = (value & 0x0f) == 0x0f; //TODO is this correct?
				c.flags.z = (value == 0);
				c.flags.n = 1;
			}

			return DEC::cycles();
		}
	};


	//ADD (8bit, 16 bit)
	template<typename Arg1, typename Arg2>
	struct ADD : default_instruction<1, 4>
	{
		static_assert(unwrap<Arg1>::size_of == unwrap<Arg2>::size_of, "Mismatching sizes");

		constexpr static int size() { return 1; }
		constexpr static int cycles() { return unwrap<Arg1>::size_of == 2 && unwrap<Arg2>::size_of == 2 ? 8 : 4 + unwrap<Arg1>::complexity + unwrap<Arg2>::complexity; }

		static int execute(context &c)
		{
			constexpr bool single_byte = unwrap<Arg1>::size_of == 1;
			using overflow_result_type = std::conditional_t<single_byte, uint16_t, uint32_t>;
			using result_type = std::conditional_t<single_byte, uint8_t, uint16_t>;


			auto &&arg1 = unwrap<Arg1>::get(c);
			auto &&arg2 = unwrap<Arg2>::get(c);
			

			overflow_result_type result = (overflow_result_type)arg1 + (overflow_result_type)arg2;
			

			c.flags.n = 0;

			if (single_byte)
				c.flags.h = ((result & 0xf) < (arg1 & 0xf));
			else
				c.flags.h = (arg1 & 0xFFF) > (result & 0xFFF);

			
			arg1 = (result_type)(result);

			if constexpr (single_byte)
			{
				c.flags.c = (result > 0xFF);
			}
			else
			{
				overflow_result_type cmask = (overflow_result_type)0xffff0000;
				c.flags.c = (result & cmask) != 0;
			}


			if (single_byte)
				c.flags.z = (arg1 == 0);


			return ADD::cycles();
		}
	};


	template<>
	struct ADD<SP, r8_as_word> : default_instruction<1, 4>
	{
		using Arg1 = SP;
		using Arg2 = r8_as_word;

		static_assert(unwrap<Arg1>::size_of == unwrap<Arg2>::size_of, "Mismatching sizes");

		constexpr static int size() { return 1; }
		constexpr static int cycles() { return 16; }

		static int execute(context &c)
		{
			constexpr bool single_byte = unwrap<Arg1>::size_of == 1;
			using overflow_result_type = std::conditional_t<single_byte, uint16_t, uint32_t>;
			using result_type = std::conditional_t<single_byte, uint8_t, uint16_t>;


			auto &&arg1 = unwrap<Arg1>::get(c);
			auto &&arg2 = unwrap<Arg2>::get(c);

			overflow_result_type result = (overflow_result_type)arg1 + (overflow_result_type)arg2;

			auto temp_value2 = arg1 ^ arg2 ^ result;
			arg1 = result;

			c.flags.c = ((temp_value2 & 0x100) == 0x100);
			c.flags.h = ((temp_value2 & 0x10) == 0x10);
			c.flags.z = 0;
			c.flags.n = 0;
			return ADD::cycles();
		}
	};


	//ADC
	template<typename Arg>
	struct ADC : default_unary_instruction<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			int32_t result = (int32_t)c.registers.a + (int32_t)value + (c.flags.c ? 1 : 0);


			c.flags.h = ((int32_t)(c.registers.a & 0xF) + (int32_t)(value & 0xF) + (c.flags.c ? 1 : 0)) > 0xF;
			c.flags.c = (result > 0xFF);
			c.registers.a = (uint8_t)result;
			c.flags.z = 0 == c.registers.a;
			c.flags.n = false;


			return ADC::cycles();
		}
	};

	//SBC
	template<typename Arg>
	struct SBC : default_unary_instruction<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);
			int32_t result = (int32_t)c.registers.a - (int32_t)value - (c.flags.c ? 1 : 0);

			c.flags.h = ((int32_t)(c.registers.a & 0xF) - (int32_t)(value & 0xF) - (c.flags.c ? 1 : 0) < 0);
			c.flags.c = result < 0;
			c.registers.a = (uint8_t)result;
			c.flags.z = 0 == c.registers.a;
			c.flags.n = true;
			
			return SBC::cycles();
		}
	};

	//SUB
	template<typename Arg>
	struct SUB : default_unary_instruction<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);

			c.flags.z = value == c.registers.a;
			c.flags.n = true;
			c.flags.h = (value & 0x0f) > (c.registers.a & 0x0f);
			c.flags.c = value > c.registers.a;
			
			c.registers.a -= value;

			return SUB::cycles();
		}
	};

	//AND
	template<typename Arg>
	struct AND : default_unary_instruction<1, 4, Arg>
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

			return AND::cycles();
		}
	};

	//XOR
	template<typename Arg>
	struct XOR : default_unary_instruction<1, 4, Arg>
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

			return XOR::cycles();
		}
	};

	//OR
	template<typename Arg>
	struct OR : default_unary_instruction<1, 4, Arg>
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

			return OR::cycles();
		}
	};

	//CP
	template<typename Arg>
	struct CP : default_unary_instruction<1, 4, Arg>
	{
		static_assert(unwrap<Arg>::size_of == 1, "Assuming byte");

		static int execute(context &c)
		{
			auto value = unwrap<Arg>::get(c);

			c.flags.z = value == c.registers.a;
			c.flags.n = true;
			c.flags.h = (value & 0x0f) > (c.registers.a & 0x0f);
			c.flags.c = value > c.registers.a;

			return CP::cycles();

		}
	};

	//--- JUMPS
	template<typename Cond, typename Arg>
	struct JP
	{
		constexpr static int size() { return std::is_same<Arg, instructions::d16>::value ? 3 : 1; }
		constexpr static int cycles() { return std::is_same<Arg, instructions::d16>::value ? 16 : 4; }

		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			auto pointer = unwrap<Arg>::get(c);

			if (!unwrap<Cond>::is_true(c))
				return 12;

			c.registers.pc = pointer;
			return JP::cycles();
		}
	};

	template<typename Cond, typename Arg>
	struct JR
	{
		constexpr static int size() { return 2; }
		constexpr static int cycles() { return 12; }

		static_assert(unwrap<Arg>::is_signed == 1, "Assuming signed data");

		static int execute(context &c)
		{
			auto pointer = unwrap<Arg>::get(c);

			if (!unwrap<Cond>::is_true(c))
				return 8;

			c.registers.pc += pointer;
			return JR::cycles();
		}
	};

	template<typename Cond = condition::_>
	struct RET
	{
		constexpr static int size() { return 1; }
		constexpr static int cycles() { return 20; }

		static int execute(context &c)
		{
			if (!unwrap<Cond>::is_true(c))
				return 8;

			c.registers.pc = c.pop();
			return RET::cycles();
		}
	};

	template<>
	struct RET<condition::_>
	{
		constexpr static int size() { return 1; }
		constexpr static int cycles() { return 16; }

		static int execute(context &c)
		{
			c.registers.pc = c.pop();
			return RET::cycles();
		}
	};


	template<typename Cond, typename Arg>
	struct CALL
	{
		constexpr static int size() { return 1; }
		constexpr static int cycles() { return 24; }

		static_assert(unwrap<Arg>::size_of == 2, "Assuming word");

		static int execute(context &c)
		{
			auto pointer = unwrap<Arg>::get(c);

			if (!unwrap<Cond>::is_true(c))
				return 12;

			c.push(c.registers.pc);
			c.registers.pc = pointer;
			return CALL::cycles();
		}
	};


	template<uint16_t address>
	struct RST : default_instruction<1, 16>
	{
		static int execute(context &c)
		{
			c.push(c.registers.pc);
			c.registers.pc = address;
			return RST::cycles();
		}
	};


	//--- INTERRUPTS
	struct EI : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			int delay = 2;
			if (c.peek_byte() == 0x76)
				delay = 1;
			c.interrupt.delay_enable(delay);
			return EI::cycles();
		}
	};

	struct DI : default_instruction<1, 4>
	{
		static int execute(context &c)
		{
			//parentObj.IME = false;
			//parentObj.IRQEnableDelay = 0;
			c.interrupt.enabled = 0;
			return EI::cycles();
		}
	};

	struct RETI
	{
		constexpr static int size() { return 1; }
		constexpr static int cycles() { return 16; }

		static int execute(context &c)
		{
			c.interrupt.enabled = 1;
			c.registers.pc = c.pop();
			return RETI::cycles();
		}
	};


	//--- CB
	struct CB
	{
		static int execute(context &c)
		{
			auto opcode = c.read_byte();
			auto instruction = c.cb_instructions()[opcode];
			return instruction(c);
		}
	};

}
}