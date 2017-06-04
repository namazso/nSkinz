#pragma once
#include <memory>

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool IsReady() const { return m_bReady; }
	bool& GetActive() const { return m_bActive; }
private:
	bool m_bReady = false;
	mutable bool m_bActive = false;
};

extern Renderer* g_pRenderer;