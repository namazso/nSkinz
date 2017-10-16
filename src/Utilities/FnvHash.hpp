#pragma once
#include <cstring>

using fnv_t = unsigned;

class FnvHash
{
	static const fnv_t FNV_PRIME = 16777619u;
	static const fnv_t OFFSET_BASIS = 2166136261u;

	template <unsigned int N>
	static constexpr fnv_t fnvHashConst(const char(&str)[N], unsigned int I = N)
	{
		return static_cast<fnv_t>(1ULL * (I == 1 ? (OFFSET_BASIS ^ str[0]) : (fnvHashConst(str, I - 1) ^ str[I - 1])) * FNV_PRIME);
	}

	static fnv_t fnvHash(const char* str)
	{
		const auto length = strlen(str) + 1;
		auto hash = OFFSET_BASIS;
		for (size_t i = 0; i < length; ++i)
		{
			hash ^= *str++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	struct Wrapper
	{
		Wrapper(const char* str) : str(str) { }
		const char* str;
	};

	fnv_t hash_value;
public:
	FnvHash(Wrapper wrapper) : hash_value(fnvHash(wrapper.str)) { }

	template <unsigned int N>
	constexpr FnvHash(const char(&str)[N]) : hash_value(fnvHashConst(str)) { }

	constexpr operator fnv_t() const { return this->hash_value; }
};
