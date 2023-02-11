#pragma once
class IPanel
{
public:
	const char* GetName(VPANEL vguiPanel);
};

enum VGuiPanel_t {
	PANEL_ROOT = 0, // buy menu problems
	PANEL_GAMEUIDLL, // only called in main menu
	PANEL_CLIENTDLL, // only called ingame
	PANEL_TOOLS, // perfect
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS
};

class IEngineVGui {
public:
	virtual	~IEngineVGui(void) {}
	virtual uint32_t GetPanel(VGuiPanel_t type) = 0;
	virtual bool IsGameUIVisible() = 0;
};