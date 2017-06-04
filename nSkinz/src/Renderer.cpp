#include "Renderer.hpp"
#include "SDK.hpp"
#include "Utilities/Platform.hpp"

// Renderer for windows. Maybe sometime i'll make a linux one

#include <d3d9.h>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include "Interface.hpp"

using EndScene_t = HRESULT(__stdcall *)(IDirect3DDevice9*);
using Reset_t = HRESULT(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

static HWND l_hwnd;
static WNDPROC l_WndProcOriginal = nullptr;
static VMTHook* l_D3D9Hook;

HRESULT __stdcall hkReset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* params)
{
	static auto fnReset = l_D3D9Hook->GetOriginalFunction<Reset_t>(16);

	if(!g_pRenderer || !g_pRenderer->IsReady())
		return fnReset(thisptr, params);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto result = fnReset(thisptr, params);

	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

HRESULT __stdcall hkEndScene(IDirect3DDevice9* thisptr)
{
	//fix drawing without cl_showfps
	thisptr->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	static auto fnEndScene = l_D3D9Hook->GetOriginalFunction<EndScene_t>(42);
	static auto bMouseEnabled = true;

	if(g_pRenderer && g_pRenderer->GetActive())
	{
		if(bMouseEnabled)
		{
			// We could do this with cvars, but too much work to implement a whole cvar interface just for this.
			g_pEngine->ClientCmd_Unrestricted("cl_mouseenable 0");
			bMouseEnabled = false;
		}

		ImGui::GetIO().MouseDrawCursor = true;

		ImGui_ImplDX9_NewFrame();

		DrawGUI();

		ImGui::Render();
	}
	else
	{
		if(!bMouseEnabled)
		{
			g_pEngine->ClientCmd_Unrestricted("cl_mouseenable 1");
			bMouseEnabled = true;
		}
	}

	return fnEndScene(thisptr);
}

bool HandleInputMessage(UINT uMessageType, WPARAM wParam, LPARAM lParam)
{
	if(!g_pRenderer)
		return false;

	if(uMessageType == WM_KEYUP && wParam == VK_INSERT)
		g_pRenderer->GetActive() ^= true;

	if(g_pRenderer->GetActive())
		return ImGui_ImplDX9_WndProcHandler(l_hwnd, uMessageType, wParam, lParam) != 0;

	return false;
}

LRESULT __stdcall hkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(HandleInputMessage(message, wParam, lParam))
		return true;

	return CallWindowProc(l_WndProcOriginal, hWnd, message, wParam, lParam);
};

Renderer::~Renderer()
{
	SetWindowLongPtr(l_hwnd, GWLP_WNDPROC, LONG_PTR(l_WndProcOriginal));
	delete l_D3D9Hook;

	// In case our hooks are still running
	Sleep(100);
}

Renderer::Renderer()
{
	l_hwnd = FindWindowA("Valve001", nullptr);

	l_WndProcOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(l_hwnd, GWLP_WNDPROC, LONG_PTR(hkWndProc)));

	auto pD3D9Device = **reinterpret_cast<IDirect3DDevice9***>(platform::FindPattern("shaderapidx9.dll", "\xA1\x00\x00\x00\x00\x50\x8B\x08\xFF\x51\x0C", "x????xxxxxx") + 1);

	l_D3D9Hook = new VMTHook(pD3D9Device);
	l_D3D9Hook->HookFunction(reinterpret_cast<void*>(hkReset), 16);
	l_D3D9Hook->HookFunction(reinterpret_cast<void*>(hkEndScene), 42);

	if(ImGui_ImplDX9_Init(l_hwnd, pD3D9Device))
		m_bReady = true;

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
