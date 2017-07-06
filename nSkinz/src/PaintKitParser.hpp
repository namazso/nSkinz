#pragma once
#include <vector>

struct PaintKit_t
{
	int id;
	std::string name;

	bool operator < (const PaintKit_t& other) const { return (name < other.name); }
};

extern std::vector<PaintKit_t> k_skins;
extern std::vector<PaintKit_t> k_gloves;

extern void GetPaintKits();
