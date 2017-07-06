#pragma once
#include "Configuration.hpp"
#include "ItemDefinitions.hpp"
#include "SDK.hpp"
#include "PaintKitParser.hpp"
#include <imgui.h>

inline void DrawGUI()
{
	ImGui::SetNextWindowSize(ImVec2(600, 300));
	if (ImGui::Begin("nSkinz", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Columns(2, nullptr, false);

		auto& entries = Config::Get()->GetItems();
		std::vector<const char*> entry_names;

		// If the user deleted the only config let's add one
		if (entries.size() == 0)
			entries.push_back(EconomyItem_t());

		for (auto& x : entries)
			entry_names.push_back(x.szName);

		static auto selected_id = 0;

		// If the user deleted the last element or loaded a config
		if (static_cast<size_t>(selected_id) >= entry_names.size())
			selected_id = 0;

		ImGui::PushItemWidth(-1);
		ImGui::ListBox("", &selected_id, const_cast<const char**>(entry_names.data()), entry_names.size(), 13);
		ImGui::PopItemWidth();

		ImGui::NextColumn();

		auto& selected_entry = entries[selected_id];

		// Name
		ImGui::InputText("Name", selected_entry.szName, 32);

		// Item to change skins for
		ImGui::Combo("Item", &selected_entry.iDefinitionId, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_weapon_names[idx].name;
			return true;
		}, nullptr, k_weapon_names.size(), 5);

		// Enabled
		ImGui::Checkbox("Enabled", &selected_entry.bEnabled);

		// Paint kit ID
		//ImGui::InputInt("Paint Kit", &SelectedItem.iPaintKitIndex);

		// Pattern Seed
		ImGui::InputInt("Seed", &selected_entry.iSeed);

		// Custom StatTrak number
		ImGui::InputInt("StatTrak", &selected_entry.iStatTrak);

		// Wear Float
		ImGui::SliderFloat("Wear", &selected_entry.flWear, FLT_MIN, 1.f, "%.10f", 5);

		// Paint kit
		ImGui::Combo("Paint Kit", &selected_entry.iPaintKitId, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_skins[idx].name.c_str();
			return true;
		}, nullptr, k_skins.size(), 10);

		// Quality
		ImGui::Combo("Quality", &selected_entry.iEntityQualityId, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_quality_names[idx].name;
			return true;
		}, nullptr, k_quality_names.size(), 5);

		// Yes we do it twice to decide knifes
		selected_entry.UpdateValues();

		// Item defindex override
		if (IsKnife(selected_entry.iDefinitionIndex))
		{
			ImGui::Combo("Knife", &selected_entry.iDefinitionOverrideId, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_knife_names[idx].name;
				return true;
			}, nullptr, k_knife_names.size(), 5);
		}
		else
		{
			// We don't want to override non-knives
			static auto unused_value = 0;
			selected_entry.iDefinitionOverrideId = 0;
			ImGui::Combo("Unavailable", &unused_value, "Only for knives\0");
		}

		selected_entry.UpdateValues();

		// Custom Name tag
		ImGui::InputText("Name Tag", selected_entry.szCustomName, 32);

		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::Columns(5, nullptr, false);
		ImGui::PushItemWidth(-1);

		// Lower buttons for modifying items and saving
		{
			auto button_size = ImVec2(ImGui::GetColumnWidth(), 20);

			if(ImGui::Button("Add", button_size))
			{
				entries.push_back(EconomyItem_t());
				selected_id = entries.size() - 1;
			}
			ImGui::NextColumn();

			if(ImGui::Button("Remove", button_size))
				entries.erase(entries.begin() + selected_id);
			ImGui::NextColumn();

			if(ImGui::Button("Update", button_size))
				(*g_client_state)->ForceFullUpdate();
			ImGui::NextColumn();

			if(ImGui::Button("Save", button_size))
				Config::Get()->Save();
			ImGui::NextColumn();

			if(ImGui::Button("Load", button_size))
				Config::Get()->Load();
			ImGui::NextColumn();
		}

		ImGui::PopItemWidth();
		ImGui::Columns(1);

		ImGui::Text("nSkinz by namazso");
		ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("https://skinchanger.download").x - 17);
		ImGui::Text("https://skinchanger.download");

		ImGui::End();
	}
}
