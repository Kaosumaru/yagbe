#pragma once
#include <cstdint>

namespace yagbe
{
	class memory
	{
	public:
		//TODO endianess
		union ByteWord
		{
			uint16_t word;
			uint8_t byte[2];
		};

		struct memory_address
		{
			memory_address(memory& m, uint16_t a) : _m(m), _a(a) 
			{
				_b = _m.read_at(_a);
			}

			~memory_address()
			{
				_m.write_byte_at(_a, _b);
			}

			memory_address& operator =(uint8_t b)
			{
				_b = b;
				return *this;
			}

			operator uint8_t() const
			{
				return _b;
			}


			memory_address& operator |=(uint8_t v)
			{
				_b |= v;
				return *this;
			}

			memory_address& operator &=(uint8_t v)
			{
				_b &= v;
				return *this;
			}

			memory_address& operator +=(uint8_t v)
			{
				_b += v;
				return *this;
			}

			memory_address& operator -=(uint8_t v)
			{
				_b -= v;
				return *this;
			}

			memory_address& operator >>=(uint8_t v)
			{
				_b >>= v;
				return *this;
			}

			memory_address& operator <<=(uint8_t v)
			{
				_b <<= v;
				return *this;
			}


		protected:
			memory& _m;
			uint16_t _a;
			uint8_t _b;
		};

		memory_address at(uint16_t address)
		{
			return memory_address{*this, address};
		}

		uint8_t read_at(uint16_t address)
		{
			return data[address];
		}

		void write_byte_at(uint16_t address, uint8_t byte)
		{
			data[address] = byte;
		}

		void write_word_at(uint16_t address, uint16_t word)
		{
			ByteWord w = { word };

			write_byte_at(address, w.byte[0]);
			write_byte_at(address + 1, w.byte[1]);
		}




		uint8_t data[0xFFFF];
	};
};