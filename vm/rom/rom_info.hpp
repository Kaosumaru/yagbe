#pragma once
#include <cstdint>
#include <string>

namespace yagbe
{
	struct alignas(4) rom_info
	{
		constexpr static uint16_t address() { return 0x100; }

		uint32_t entry_point;
		uint8_t logo[48];

		uint8_t title[11];
		uint8_t game_code[4];
		uint8_t cgb_support;

		uint8_t publisher[2];

		uint8_t sgc;
		uint8_t cartridge_type;

		uint8_t rom_size_flag;
		uint8_t ram_size;

		uint8_t destination;
		uint8_t publisher_old;
		uint8_t rom_version;

		uint8_t  header_checksum;
		uint16_t rom_checksum;

		int rom_size_bytes() const { return (32 * 1024) << rom_size_flag; }

		std::string game_title() const { return { (char*)title, 11 }; }
		bool supports_cgb() const { return cgb_support == 0x80 || requires_cgb(); }
		bool requires_cgb() const { return cgb_support == 0xC0; }
	};

	static_assert(sizeof(rom_info) == 80, "Wrong size, padding?");
}

