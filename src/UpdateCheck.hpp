#pragma once
#include <string>
#include <vector>
extern void CheckUpdate();

struct Commit_t
{
	std::string author;
	std::string date;
	std::string message;
};

extern bool g_update_needed;
extern std::vector<Commit_t> g_commits_since_compile;