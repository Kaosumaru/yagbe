#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include "io_registers.hpp"

namespace yagbe
{
	class context;

	class memory
	{
	public:
		struct interceptor
		{
			interceptor() {}
			//using ReadCallback = std::function<uint8_t(memory &m, uint16_t)>;
			//using WriteCallback = std::function<void(memory &m, uint16_t, uint8_t)>;

			using ReadCallback = uint8_t(*)(memory &m, uint16_t);
			using WriteCallback = void(*)(memory &m, uint16_t, uint8_t);

			ReadCallback  onRead = nullptr;
			WriteCallback onWrite = nullptr;
		};

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
				if (_dirty)
					_m.write_byte_at(_a, _b);
			}

			memory_address& operator =(uint8_t b)
			{
				_b = b;
				_dirty = true;
				return *this;
			}

			operator uint8_t() const
			{
				return _b;
			}


			memory_address& operator |=(uint8_t v)
			{
				_dirty = true;
				_b |= v;
				return *this;
			}

			memory_address& operator &=(uint8_t v)
			{
				_dirty = true;
				_b &= v;
				return *this;
			}

			memory_address& operator +=(uint8_t v)
			{
				_dirty = true;
				_b += v;
				return *this;
			}

			memory_address& operator -=(uint8_t v)
			{
				_dirty = true;
				_b -= v;
				return *this;
			}

			memory_address& operator >>=(uint8_t v)
			{
				_dirty = true;
				_b >>= v;
				return *this;
			}

			memory_address& operator <<=(uint8_t v)
			{
				_dirty = true;
				_b <<= v;
				return *this;
			}


		protected:
			bool _dirty = false;
			memory& _m;
			uint16_t _a;
			uint8_t _b;
		};

		memory(context &c) : _c(c)
		{

		}

		io_registers io_register{ data };

		memory_address at(uint16_t address)
		{
			return memory_address{*this, address};
		}

		//raw byte at address, without interceptors
		uint8_t& raw_at(uint16_t address)
		{
			return data[address];
		}

		uint8_t* raw_pointer_at(uint16_t address)
		{
			return data + address;
		}

		uint8_t read_at(uint16_t address)
		{
			auto& inter = interceptor_at(address).onRead;
			if (!inter)
				return data[address];
			return inter(*this, address);
		}

		void write_byte_at(uint16_t address, uint8_t byte)
		{
			auto& inter = interceptor_at(address).onWrite;
			if (!inter)
				data[address] = byte;
			else
				inter(*this, address, byte);
		}

		void write_word_at(uint16_t address, uint16_t word)
		{
			ByteWord w = { word };

			write_byte_at(address, w.byte[0]);
			write_byte_at(address + 1, w.byte[1]);
		}

		void map_interceptors(uint16_t start, uint16_t end, const interceptor::ReadCallback& rc, const interceptor::WriteCallback& wc)
		{
			int end_range = end;
			end_range++;
			if (start % interceptor_range != 0)
				throw std::out_of_range("Wrong start");
			if (end_range % interceptor_range != 0)
				throw std::out_of_range("Wrong end");

			for (int i = start; i < end_range; i += interceptor_range)
			{
				auto& inter = interceptor_at(i);
				inter.onWrite = wc;
				inter.onRead = rc;
			}
		}

		context &c() { return _c; }
protected:
		interceptor& interceptor_at(uint16_t address)
		{
			auto index = address >> 8;
			return _interceptors[index];
		}

		constexpr static int interceptor_range = 0x100;
		std::array<interceptor, interceptor_range> _interceptors;
		uint8_t data[0xFFFF];
		
		context &_c;

	};
};