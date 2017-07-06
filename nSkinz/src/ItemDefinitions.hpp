#pragma once
#include <map>
#include <vector>

inline bool IsKnife(int i) { return i >= 500 || i == 59 || i == 42; }

// Stupid MSVC requires separate constexpr constructors for any initialization

struct Item_t
{
	constexpr Item_t(const char* display_name, const char* entity_name, const char* model, const char* icon = nullptr) :
		//display_name(display_name),
		//entity_name(entity_name),
		model(model),
		icon(icon)
	{}

	//const char* display_name = nullptr;
	//const char* entity_name = nullptr;
	const char* model;
	const char* icon;
};

struct WeaponName_t
{
	constexpr WeaponName_t(int definition_index, const char* name) :
		definition_index(definition_index),
		name(name)
	{}

	int definition_index = 0;
	const char* name = nullptr;
};

struct QualityName_t
{
	constexpr QualityName_t(int index, const char* name) :
		index(index),
		name(name)
	{}

	int index = 0;
	const char* name = nullptr;
};

extern const std::map<size_t, Item_t> k_weapon_info;
extern const std::vector<WeaponName_t> k_knife_names;
extern const std::vector<WeaponName_t> k_weapon_names;
extern const std::vector<QualityName_t> k_quality_names;