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
#include "hooks.hpp"
#include "../config.hpp"
#include "../nSkinz.hpp"

auto __fastcall hooks::FireEventClientSide::hooked(sdk::IGameEventManager2* thisptr, void*, sdk::IGameEvent* event) -> bool
{
	// Filter to only the events we're interested in.
	if(!strcmp(event->GetName(), "player_death")
		&& g_engine->GetPlayerForUserID(event->GetInt("attacker")) == g_engine->GetLocalPlayer())
		if(const auto icon_override = g_config.get_icon_override(event->GetString("weapon")))
			event->SetString("weapon", icon_override);

	return m_original(thisptr, nullptr, event);
}

hooks::FireEventClientSide::Fn* hooks::FireEventClientSide::m_original;