#include "Renderer.hpp"
#include "SDK.hpp"
#include "Utilities/Platform.hpp"

// Renderer for windows. Maybe sometime i'll make a linux one

#include <d3d9.h>

#include <imgui.h>
#include <imgui_impl_dx9.h>

// Implement this somewhere
extern void DrawGUI();

using EndScene_t = HRESULT(__stdcall *)(IDirect3DDevice9*);
using Reset_t = HRESULT(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Render
{
	static HWND s_hwnd;
	static WNDPROC s_original_wnd_proc = nullptr;
	static VMTHook* s_d3d_hook;

	static bool s_ready = false;
	static bool s_active = false;

	HRESULT __stdcall hkReset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* params)
	{
		static auto original_fn = s_d3d_hook->GetOriginalFunction<Reset_t>(16);

		if(!s_ready)
			return original_fn(thisptr, params);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto result = original_fn(thisptr, params);

		ImGui_ImplDX9_CreateDeviceObjects();

		return result;
	}

	HRESULT __stdcall hkEndScene(IDirect3DDevice9* thisptr)
	{
		static auto original_fn = s_d3d_hook->GetOriginalFunction<EndScene_t>(42);

		//fix drawing without cl_showfps
		thisptr->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

		static auto mouse_enabled = true;

		if(s_active)
		{
			if(mouse_enabled)
			{
				// We could do this with cvars, but too much work to implement a whole cvar interface just for this.
				g_engine->ClientCmd_Unrestricted("cl_mouseenable 0");
				mouse_enabled = false;
			}

			ImGui::GetIO().MouseDrawCursor = true;

			ImGui_ImplDX9_NewFrame();

			DrawGUI();

			ImGui::Render();
		}
		else
		{
			if(!mouse_enabled)
			{
				g_engine->ClientCmd_Unrestricted("cl_mouseenable 1");
				mouse_enabled = true;
			}
		}

		return original_fn(thisptr);
	}

	bool HandleInputMessage(UINT message_type, WPARAM w_param, LPARAM l_param)
	{
		if(!s_ready)
			return false;

		if(message_type == WM_KEYUP && w_param == VK_INSERT)
			s_active = !s_active;

		if(s_active)
			return ImGui_ImplDX9_WndProcHandler(s_hwnd, message_type, w_param, l_param) != 0;

		return false;
	}

	LRESULT __stdcall hkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(HandleInputMessage(message, wParam, lParam))
			return true;

		return CallWindowProc(s_original_wnd_proc, hWnd, message, wParam, lParam);
	};

	void Initialize()
	{
		if(s_ready)
			return;

		s_hwnd = FindWindowA("Valve001", nullptr);

		s_original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(s_hwnd, GWLP_WNDPROC, LONG_PTR(hkWndProc)));

		auto d3d_device = **reinterpret_cast<IDirect3DDevice9***>(platform::FindPattern("shaderapidx9.dll", "\xA1\x00\x00\x00\x00\x50\x8B\x08\xFF\x51\x0C", "x????xxxxxx") + 1);

		s_d3d_hook = new VMTHook(d3d_device);
		s_d3d_hook->HookFunction(reinterpret_cast<void*>(hkReset), 16);
		s_d3d_hook->HookFunction(reinterpret_cast<void*>(hkEndScene), 42);

		if(ImGui_ImplDX9_Init(s_hwnd, d3d_device))
			s_ready = true;

		{
			auto& style = ImGui::GetStyle();

			style.ChildWindowRounding = 3.f;
			style.GrabRounding = 0.f;
			style.WindowRounding = 0.f;
			style.ScrollbarRounding = 3.f;
			style.FrameRounding = 3.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

			style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
			style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

			auto& io = ImGui::GetIO();

			io.Fonts->AddFontDefault();
		}
	}

	void Uninitialize()
	{
		if(!s_ready)
			return;

		s_ready = false;

		SetWindowLongPtr(s_hwnd, GWLP_WNDPROC, LONG_PTR(s_original_wnd_proc));
		delete s_d3d_hook;

		// In case our hooks are still running
		Sleep(100);
	}
}