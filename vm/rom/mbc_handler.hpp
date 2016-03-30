#pragma once

#include <memory>
#include <vector>
#include "rom_info.hpp"
#include "vm/memory.hpp"

namespace yagbe
{

	class mbc_handler
	{
	public:
		mbc_handler(memory& m, std::vector<uint8_t>&& data) : _m(m), _rom_data(std::move(data)) {}
		using pointer = std::unique_ptr<mbc_handler>;



		virtual uint8_t handle_read(uint16_t address) = 0;
		virtual void handle_write(uint16_t address, uint8_t byte) = 0;

		virtual uint8_t handle_ram_read(uint16_t address)
		{
			return _m.raw_at(address);
		}
		virtual void handle_ram_write(uint16_t address, uint8_t byte)
		{
			_m.raw_at(address) = byte;
		}

		static pointer create_for(rom_info *info, memory& m, std::vector<uint8_t>&& data);

		template <typename Archive>
		void operator & (Archive &archive)
		{

		}
	protected:
		std::vector<uint8_t> _rom_data;
		memory& _m;
	};


}