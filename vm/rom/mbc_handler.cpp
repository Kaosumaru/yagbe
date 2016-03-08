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


class generic_mbc_handler : public mbc_handler
{
	using mbc_handler::mbc_handler;

	uint8_t handle_read(uint16_t address) override
	{
		if (address < rom_bank_size())
			return _m.raw_at(address);

		auto offset = (int)address - rom_bank_size();
		return *(address_of_current_bank() + offset);
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

	uint8_t _selected_rom_bank = 1;
};

class mbc1_handler : public generic_mbc_handler
{
	using generic_mbc_handler::generic_mbc_handler;

	void handle_write(uint16_t address, uint8_t byte) override
	{
		if (address < 0x2000) //Enable external RAM
		{
			_ram_enabled = byte == 0x0A;
		}
		else if (address < 0x4000) 	//ROM bank (low 5 bits)
		{
			update_byte_with_mask<uint8_t>(_rom_bank_temp, byte, 0b00011111);
			select_rom_bank(_rom_bank_temp);
		}
		else if (address < 0x6000)	//ROM bank (high 2 bits) set OR RAM bank
		{
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
	bool _rom_mode = true;
	
	uint8_t _rom_bank_temp = 0;

	bool _ram_enabled = false;
	int _ram_bank = 0;
};



class mbc3_handler : public generic_mbc_handler
{
	using generic_mbc_handler::generic_mbc_handler;

	void handle_write(uint16_t address, uint8_t byte) override
	{
		if (address < 0x2000) //Enable external RAM & Timer
		{
			_ram_enabled = byte == 0x0A;
		}
		else if (address < 0x4000) 	//ROM bank
		{
			select_rom_bank(byte);
		}
		else if (address < 0x6000)	//RAM Bank Number - or -RTC Register Select(Write Only)
		{
			//NYI
		}
		else //Latch Clock Data (Write Only)
		{
			//NYI
		}
	}

protected:

	uint8_t _rom_bank_temp = 0;

	bool _ram_enabled = false;
	int _ram_bank = 0;
};

mbc_handler::pointer mbc_handler::create_for(rom_info *info, memory& m, std::vector<uint8_t>&& data)
{
	auto mbc_type = info->cartridge_type;
	

	switch (mbc_type)
	{
	case 0x00: //rom only
		return std::make_unique<mbc_handler_0>(m, std::move(data));

	case 0x01: //MBC1
	case 0x02: //MBC1+RAM
	case 0x03: //MBC1+RAM+BATT
		return std::make_unique<mbc1_handler>(m, std::move(data));

	case 0x05: //MBC2+RAM
	case 0x06: //MBC2+RAM+BATT
		break;

	case 0x08: //ROM+RAM
	case 0x09: //ROM+RAM+BATT
		return std::make_unique<mbc_handler_0>(m, std::move(data));

	case 0x12: //MBC3+RAM
	case 0x13: //MBC3+RAM+BATT
		return std::make_unique<mbc3_handler>(m, std::move(data));
		break;

	case 0x19: //MBC5
	case 0x1A: //MBC5+RAM
	case 0x1B: //MBC5+RAM+BATT
	case 0x1C: //MBC5+RUMBLE
	case 0x1D: //MBC5+RUMBLE+SRAM
	case 0x1E: //MBC5+RUMBLE+SRAM+BATT
		break;

	}

	throw std::runtime_error("Not implemented this MBC");
	return nullptr;
}