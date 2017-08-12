#pragma once
#include "serializer.hpp"
#include "em-appdata/DriveAppData.h"
#include <iostream>

namespace yagbe
{
#ifdef __EMSCRIPTEN__
	class drive_serializer : public serializer
	{
	public:
		using serializer::serializer;

		bool load_state(int n)
		{
			if (_working)
				return false;
			_working = true;
			if (!load_stream(key_for_index(n)))
				return false;
			return true;
		}

		bool save_state(int n)
		{
			if (_working)
				return false;
			_working = true;
			std::stringstream ss;
			save(ss);
			save_stream(ss, key_for_index(n));
			return true;
		}

	protected:
		std::string key_for_index(int i)
		{
			std::stringstream ss;
			ss << _c.current_rom_info()->game_title() << "." << "sav" << i;
			return ss.str();
		}

		void save_stream(std::stringstream &ss, const std::string& name)
		{
			int error;
			auto str = ss.str();

			MX::Drive::UploadFile(name, (void*)str.c_str(), str.size(), [this](MX::Drive::NetworkResponse result){
				if (result == MX::Drive::NetworkResponse::OK)
					save_ok();
				else
					on_error();
			});

		}

		bool load_stream(const std::string& name)
		{
			MX::Drive::DownloadFile(name, [this](MX::Drive::NetworkResponse result, std::unique_ptr<char[]>&& data, unsigned int size){
						if (result != MX::Drive::NetworkResponse::OK)
						{
							on_error();
							return;
						}
						load_ok(data.get(), size);
			});
			return true;
		}

		void save_ok()
		{
			std::cout << "Cloud: on_save_ok" << std::endl;
			_working = false;
		}

		void on_error()
		{
			std::cout << "Cloud: on_error" << std::endl;
			_working = false;
		}

		void load_ok(void* ptr, int num)
		{
			std::cout << "Cloud: on_load_ok" << std::endl;
			std::stringstream ss;
			ss.write((const char*)ptr, num);
			load(ss);

			_working = false;
		}


		bool _working = false;
	};
#endif
};
