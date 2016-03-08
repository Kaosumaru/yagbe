#include "mbc_handler.hpp"
#include "vm/utils.hpp"
#include <set>

using namespace yagbe;


class mbc_handler_0 : public mbc_handler
{
	using mbc_handler::mbc_handler;

	uint8_t handle_read(uint16_t address) override
	{
		return _m.raw_at(address);
	}

	void handle_write(uint16_t address, uint8_t byte) override
	{
		
	}
};


class mbc1_handler : public mbc_handler
{
	using mbc_handler::mbc_handler;

	uint8_t handle_read(uint16_t address) override
	{
		if (address < rom_bank_size())
			return _m.raw_at(address);
		
		auto offset = (int)address - rom_bank_size();
		return *(address_of_current_bank() + offset);
	}

	void handle_write(uint16_t address, uint8_t byte) override
	{
		if (address < 0x2000)
		{
			//Enable external RAM
			_ram_enabled = byte == 0x0A;
		}
		else if (address < 0x4000)
		{
			//ROM bank (low 5 bits)
			update_byte_with_mask<uint8_t>(_rom_bank_temp, byte, 0b00011111);
			select_rom_bank(_rom_bank_temp);
		}
		else if (address < 0x6000)
		{
			//ROM bank (high 2 bits) set, RAM bank
			if (_rom_mode)
			{
				update_byte_with_mask<uint8_t>(_rom_bank_temp, byte << 5, 0b01100000);
				select_rom_bank(_rom_bank_temp);
			}
			else
			{
				//NYI
			}
		}
		else
		{
			_rom_mode = byte == 0;
		}
	}

protected:
	void select_rom_bank(int bank)
	{
		std::set<int> bugged_banks = { 0x00, 0x20, 0x40, 0x60 };
		if (bugged_banks.find(bank) != bugged_banks.end())
			bank++;

		_selected_rom_bank = bank;
	}

	int rom_bank_size()
	{
		return 0x4000;
	}

	uint8_t* address_of_current_bank()
	{
		return _rom_data.data() + rom_bank_size() * _selected_rom_bank;
	}

	bool _rom_mode = true;

	uint8_t _selected_rom_bank = 1;
	uint8_t _rom_bank_temp = 0;

	bool _ram_enabled = false;
	int _ram_bank = 0;

};


mbc_handler::pointer mbc_handler::create_for(rom_info *info, memory& m, std::vector<uint8_t>&& data)
{
	auto mbc_type = info->cartridge_type;
	

	switch (mbc_type)
	{
	case 0: //rom only
		return std::make_unique<mbc_handler_0>(m, std::move(data));
	case 1: //MBC1
		return std::make_unique<mbc1_handler>(m, std::move(data));
	case 2: //MBC1+RAM
		return std::make_unique<mbc1_handler>(m, std::move(data));
	case 3: //MBC1+RAM+BATT
		return std::make_unique<mbc1_handler>(m, std::move(data));

	}

	throw std::runtime_error("Not implemented this MBC");
	return nullptr;
}