#pragma once
#include "Configuration.hpp"
#include "ItemDefinitions.hpp"
#include "SDK.hpp"
#include "Skins.hpp"
#include <imgui.h>

inline void DrawGUI()
{
	ImGui::SetNextWindowSize(ImVec2(600, 300));
	if(ImGui::Begin("nSkinz", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Columns(2, nullptr, false);

		auto& vItems = Config::Get()->GetItems();
		std::vector<char*> vpszItemNames;

		if(vItems.size() == 0)
			vItems.push_back(EconomyItem_t());

		for(auto& x : vItems)
			vpszItemNames.push_back(x.szName);

		static auto iSelected = 0;

		// If the user deleted the last element or loaded a config
		if(static_cast<size_t>(iSelected) >= vpszItemNames.size())
			iSelected = 0;

		ImGui::PushItemWidth(-1);
		ImGui::ListBox("", &iSelected, const_cast<const char**>(vpszItemNames.data()), vpszItemNames.size(), 13);
		ImGui::PopItemWidth();

		ImGui::NextColumn();

		auto& SelectedItem = vItems[iSelected];

		// Name
		ImGui::InputText("Name", SelectedItem.szName, 32);

		// Item to change skins for
		ImGui::Combo("Item", &SelectedItem.iDefinitionId, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_WeaponNames[idx].szName;
				return true;
			}, nullptr, k_WeaponNames.size(), 5);

		// Enabled
		ImGui::Checkbox("Enabled", &SelectedItem.bEnabled);

		// Paint kit ID
		//ImGui::InputInt("Paint Kit", &SelectedItem.iPaintKitIndex);

		// Pattern Seed
		ImGui::InputInt("Seed", &SelectedItem.iSeed);

		// Custom StatTrak number
		ImGui::InputInt("StatTrak", &SelectedItem.iStatTrak);

		// Wear Float
		ImGui::SliderFloat("Wear", &SelectedItem.flWear, FLT_MIN, 1.f, "%.10f", 5);

		// Paint kit
		ImGui::Combo("Paint Kit", &SelectedItem.iPaintKitId, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_Skins[idx].strName.c_str();
			return true;
		}, nullptr, k_Skins.size(), 10);

		// Quality
		ImGui::Combo("Quality", &SelectedItem.iEntityQualityId, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_QualityNames[idx].szName;
			return true;
		}, nullptr, k_QualityNames.size(), 5);

		// Yes we do it twice to decide knifes
		SelectedItem.UpdateValues();

		// Item defindex override
		if(IsKnife(SelectedItem.iDefinitionIndex))
		{
			ImGui::Combo("Knife", &SelectedItem.iDefinitionOverrideId, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_KniveNames[idx].szName;
					return true;
				}, nullptr, k_KniveNames.size(), 5);
		}
		else
		{
			static auto iThrowaway = 0;
			SelectedItem.iDefinitionOverrideId = 0;
			ImGui::Combo("Unavailable", &iThrowaway, "Only for knives\0");
		}

		SelectedItem.UpdateValues();

		// Custom Name tag
		ImGui::InputText("Name Tag", SelectedItem.szCustomName, 32);

		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::Columns(5, nullptr, false);
		ImGui::PushItemWidth(-1);

		auto vSize = ImVec2(ImGui::GetColumnWidth(), 20);

		if(ImGui::Button("Add", vSize))
		{
			vItems.push_back(EconomyItem_t());
			iSelected = vItems.size() - 1;
		}
		ImGui::NextColumn();

		if(ImGui::Button("Remove", vSize))
			vItems.erase(vItems.begin() + iSelected);
		ImGui::NextColumn();

		if(ImGui::Button("Update", vSize))
			(*g_ppClientState)->ForceFullUpdate();
		ImGui::NextColumn();

		if(ImGui::Button("Save", vSize))
			Config::Get()->Save();
		ImGui::NextColumn();

		if(ImGui::Button("Load", vSize))
			Config::Get()->Load();
		ImGui::NextColumn();

		ImGui::PopItemWidth();
		ImGui::Columns(1);

		ImGui::Text("nSkinz by namazso");
		ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("https://skinchanger.download").x - 17);
		ImGui::Text("https://skinchanger.download");

		ImGui::End();
	}
}
