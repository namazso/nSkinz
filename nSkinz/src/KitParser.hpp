#pragma once
#include <vector>

struct Kit_t
{
	int id;
	std::string name;

	bool operator < (const Kit_t& other) const { return (name < other.name); }
};

extern std::vector<Kit_t> k_skins;
extern std::vector<Kit_t> k_gloves;
extern std::vector<Kit_t> k_stickers;

extern void InitializeKits();
