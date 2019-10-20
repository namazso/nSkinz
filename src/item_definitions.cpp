/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "item_definitions.hpp"
// These are std::vectors because else I'd have to write their size in the header or write my own container

// We need these for overriding viewmodels and icons
const game_data::weapon_info* game_data::get_weapon_info(int defindex)
{
	const static std::map<int, weapon_info> info =
	{
		{WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "knife_default_ct"}},
		{WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"}},
		{WEAPON_KNIFE_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
		{WEAPON_KNIFE_CSS, {"models/weapons/v_knife_css.mdl", "knife_css"}},
		{WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
		{WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
		{WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
		{WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
		{WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
		{WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
		{WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
		{WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
		{WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
		{WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
		{WEAPON_KNIFE_GYPSY_JACKKNIFE,{"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"}},
		{WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
		{WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
		{GLOVE_STUDDED_BLOODHOUND,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"}},
		{GLOVE_T_SIDE,{"models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl"}},
		{GLOVE_CT_SIDE,{"models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl"}},
		{GLOVE_SPORTY,{"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"}},
		{GLOVE_SLICK,{"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"}},
		{GLOVE_LEATHER_WRAP,{"models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"}},
		{GLOVE_MOTORCYCLE,{"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"}},
		{GLOVE_SPECIALIST,{"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}},
		{GLOVE_HYDRA,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"}}
	};

	const auto entry = info.find(defindex);
	return entry == end(info) ? nullptr : &entry->second;
}

const std::vector<game_data::weapon_name> game_data::knife_names =
{
	{0, "Default"},
	{WEAPON_KNIFE_BAYONET, "Bayonet"},
	{WEAPON_KNIFE_CSS, "Classic Knife"},
	{WEAPON_KNIFE_FLIP, "Flip Knife"},
	{WEAPON_KNIFE_GUT, "Gut Knife"},
	{WEAPON_KNIFE_KARAMBIT, "Karambit"},
	{WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet"},
	{WEAPON_KNIFE_TACTICAL, "Huntsman Knife"},
	{WEAPON_KNIFE_FALCHION, "Falchion Knife"},
	{WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife"},
	{WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife"},
	{WEAPON_KNIFE_PUSH, "Shadow Daggers"},
	{WEAPON_KNIFE_URSUS, "Ursus Knife"},
	{WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife"},
	{WEAPON_KNIFE_STILETTO, "Stiletto Knife"},
	{WEAPON_KNIFE_WIDOWMAKER, "Talon Knife"}
};

const std::vector<game_data::weapon_name> game_data::glove_names =
{
	{0, "Default"},
	{GLOVE_STUDDED_BLOODHOUND, "Bloodhound"},
	{GLOVE_T_SIDE, "Default (Terrorists)"},
	{GLOVE_CT_SIDE, "Default (Counter-Terrorists)"},
	{GLOVE_SPORTY, "Sporty"},
	{GLOVE_SLICK, "Slick"},
	{GLOVE_LEATHER_WRAP, "Handwrap"},
	{GLOVE_MOTORCYCLE, "Motorcycle"},
	{GLOVE_SPECIALIST, "Specialist"},
	{GLOVE_HYDRA, "Hydra"}
};

const std::vector<game_data::weapon_name> game_data::weapon_names =
{
	{WEAPON_KNIFE, "Knife"},
	{GLOVE_T_SIDE, "Glove"},
	{WEAPON_AK47, "AK-47"},
	{WEAPON_AUG, "AUG"},
	{WEAPON_AWP, "AWP"},
	{WEAPON_CZ75A, "CZ75 Auto"},
	{WEAPON_DEAGLE, "Desert Eagle"},
	{WEAPON_ELITE, "Dual Berettas"},
	{WEAPON_FAMAS, "FAMAS"},
	{WEAPON_FIVESEVEN, "Five-SeveN"},
	{WEAPON_G3SG1, "G3SG1"},
	{WEAPON_GALILAR, "Galil AR"},
	{WEAPON_GLOCK, "Glock-18"},
	{WEAPON_M249, "M249"},
	{WEAPON_M4A1_SILENCER, "M4A1-S"},
	{WEAPON_M4A1, "M4A4"},
	{WEAPON_MAC10, "MAC-10"},
	{WEAPON_MAG7, "MAG-7"},
	{WEAPON_MP5_SD, "MP5-SD"},
	{WEAPON_MP7, "MP7"},
	{WEAPON_MP9, "MP9"},
	{WEAPON_NEGEV, "Negev"},
	{WEAPON_NOVA, "Nova"},
	{WEAPON_HKP2000, "P2000"},
	{WEAPON_P250, "P250"},
	{WEAPON_P90, "P90"},
	{WEAPON_BIZON, "PP-Bizon"},
	{WEAPON_REVOLVER, "R8 Revolver"},
	{WEAPON_SAWEDOFF, "Sawed-Off"},
	{WEAPON_SCAR20, "SCAR-20"},
	{WEAPON_SSG08, "SSG 08"},
	{WEAPON_SG556, "SG 553"},
	{WEAPON_TEC9, "Tec-9"},
	{WEAPON_UMP45, "UMP-45"},
	{WEAPON_USP_SILENCER, "USP-S"},
	{WEAPON_XM1014, "XM1014"},
};

const std::vector<game_data::quality_name> game_data::quality_names =
{
	{0, "Default"},
	{1, "Genuine"},
	{2, "Vintage"},
	{3, "Unusual"},
	{5, "Community"},
	{6, "Developer"},
	{7, "Self-Made"},
	{8, "Customized"},
	{9, "Strange"},
	{10, "Completed"},
	{12, "Tournament"}
};
