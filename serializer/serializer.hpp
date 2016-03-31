#pragma once
#include <cstdint>
#include <array>
#include <sstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "vm/context.hpp"

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
			stream.seekg(0);
			stream.seekp(0);
			stream.clear();

			cereal::BinaryOutputArchive oarchive(stream);
			oarchive(_c);
		}

		template<typename Stream>
		void load(Stream& stream)
		{
			stream.seekg(0);
			stream.seekp(0);
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

		void load_state(int n)
		{
			load(_saves[n]);
		}

		void save_state(int n)
		{
			save(_saves[n]);
		}

	protected:
		std::stringstream _saves[10];
	};


	

};