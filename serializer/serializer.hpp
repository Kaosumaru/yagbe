#pragma once
#include <cstdint>
#include <array>
#include <sstream>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "vm/context.hpp"



#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#if 0
#include <direct.h>
#endif
#endif

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

#if 0
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
#endif	

#ifdef __EMSCRIPTEN__
	class emscripten_serializer : public serializer
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
			emscripten_idb_async_store("saves", name.c_str(), (void*)str.c_str(), str.size(), (void*)this, 
				[](void* arg) { ((emscripten_serializer*)arg)->save_ok(); }, 
				[](void* arg) { ((emscripten_serializer*)arg)->on_error(); });
		}

		bool load_stream(const std::string& name)
		{
			emscripten_idb_async_load("saves", name.c_str(), (void*)this, 
				[](void* arg, void* ptr, int num) { ((emscripten_serializer*)arg)->load_ok(ptr, num); },
				[](void* arg) { ((emscripten_serializer*)arg)->on_error(); });
			return true;
		}

		void save_ok()
		{
			_working = false;
		}

		void on_error()
		{
			_working = false;
		}

		void load_ok(void* ptr, int num)
		{
			std::stringstream ss;
			ss.write((const char*)ptr, num);
			load(ss);

			_working = false;
		}


		bool _working = false;
	};
#endif

};