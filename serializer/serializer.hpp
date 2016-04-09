#pragma once
#include <cstdint>
#include <array>
#include <sstream>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "vm/context.hpp"

#include <direct.h>

namespace yagbe
{

	class serializer
	{
	public:
		serializer(context &c) : _c(c) {}

	protected:
		template<typename Stream>
		void save(Stream& stream)
		{
			stream.seekp(0);
			stream.clear();

			cereal::BinaryOutputArchive oarchive(stream);
			oarchive(_c);
		}

		template<typename Stream>
		void load(Stream& stream)
		{
			stream.seekg(0);
			stream.clear();

			cereal::BinaryInputArchive iarchive(stream);
			iarchive(_c);
		}

		context &_c;
	};


	class quicksave_serializer : public serializer
	{
	public:
		using serializer::serializer;

		bool load_state(int n)
		{
			load(_saves[n]);
			return true;
		}

		bool save_state(int n)
		{
			save(_saves[n]);
			return true;
		}

	protected:
		std::stringstream _saves[10];
	};

	class filesave_serializer : public serializer
	{
	public:
		filesave_serializer(context &c, const std::string& base_path = "saves/") : serializer(c), _base_path(base_path)
		{
			_mkdir(base_path.c_str());
		}

		bool load_state(int n)
		{
			std::ifstream file{ filename_for_index(n), std::ios::binary };
			if (!file)
				return false;
			load(file);
			return true;
		}

		bool save_state(int n)
		{
			std::ofstream file{ filename_for_index(n), std::ios::binary };
			if (!file)
				return false;
			save(file);
			return !!file;
		}
	protected:
		std::string filename_for_index(int i)
		{
			std::stringstream ss;
			ss << _base_path;
			ss << _c.current_rom_info()->game_title() << "." << "sav" << i;
			return ss.str();
		}

		std::string _base_path;
	};
	

};