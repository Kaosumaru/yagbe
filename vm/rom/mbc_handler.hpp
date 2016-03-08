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

		static pointer create_for(rom_info *info, memory& m, std::vector<uint8_t>&& data);
	protected:
		std::vector<uint8_t> _rom_data;
		memory& _m;
	};


}