#pragma once
#include "Configuration.hpp"
#include "ItemDefinitions.hpp"
#include "SDK.hpp"
#include "PaintKitParser.hpp"
#include <imgui.h>
#include "UpdateCheck.hpp"

inline void DrawGUI()
{
	char title[128];
	
	if(!g_update_needed)
		strcpy_s(title, "nSkinz");
	else
		sprintf_s(title, "nSkinz - OUTDATED - Latest commit: %s", g_last_commit_date);

	ImGui::SetNextWindowSize(ImVec2(600, 300));
	if(ImGui::Begin(title, nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Columns(2, nullptr, false);

		auto& entries = Config::Get()->GetItems();

		// If the user deleted the only config let's add one
		if(entries.size() == 0)
			entries.push_back(EconomyItem_t());

		static auto selected_id = 0;

		// If the user deleted the last element or loaded a config
		//if(size_t(selected_id) >= entries.size())
		//	selected_id = 0;
		// ImGui clamps this anyways

		ImGui::PushItemWidth(-1);
		ImGui::ListBox("", &selected_id, [](void* data, int idx, const char** out_text)
		{
			auto& entries = *reinterpret_cast<std::vector<EconomyItem_t>*>(data);
			*out_text = entries.at(idx).name;
			return true;
		}, &entries, entries.size(), 13);
		ImGui::PopItemWidth();

		ImGui::NextColumn();

		auto& selected_entry = entries[selected_id];

		// Name
		ImGui::InputText("Name", selected_entry.name, 32);

		// Item to change skins for
		ImGui::Combo("Item", &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_weapon_names[idx].name;
			return true;
		}, nullptr, k_weapon_names.size(), 5);

		// Enabled
		ImGui::Checkbox("Enabled", &selected_entry.enabled);

		// Paint kit ID
		//ImGui::InputInt("Paint Kit", &SelectedItem.iPaintKitIndex);

		// Pattern Seed
		ImGui::InputInt("Seed", &selected_entry.seed);

		// Custom StatTrak number
		ImGui::InputInt("StatTrak", &selected_entry.stat_trak);

		// Wear Float
		ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

		// Paint kit
		if(selected_entry.definition_index != GLOVE_T_SIDE)
		{
			ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_skins[idx].name.c_str();
				return true;
			}, nullptr, k_skins.size(), 10);
		}
		else
		{
			ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_gloves[idx].name.c_str();
				return true;
			}, nullptr, k_gloves.size(), 10);
		}

		// Quality
		ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
		{
			*out_text = k_quality_names[idx].name;
			return true;
		}, nullptr, k_quality_names.size(), 5);

		// Yes we do it twice to decide knifes
		selected_entry.UpdateValues();

		// Item defindex override
		if(selected_entry.definition_index == WEAPON_KNIFE)
		{
			ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_knife_names.at(idx).name;
				return true;
			}, nullptr, k_knife_names.size(), 5);
		}
		else if(selected_entry.definition_index == GLOVE_T_SIDE)
		{
			ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_glove_names.at(idx).name;
				return true;
			}, nullptr, k_glove_names.size(), 5);
		}
		else
		{
			// We don't want to override weapons other than knives or gloves
			static auto unused_value = 0;
			selected_entry.definition_override_vector_index = 0;
			ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
		}

		selected_entry.UpdateValues();

		// Custom Name tag
		ImGui::InputText("Name Tag", selected_entry.custom_name, 32);

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
