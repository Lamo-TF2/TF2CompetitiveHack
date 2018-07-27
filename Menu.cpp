#include "Menu.h"

/*
char* szHealthModes[] = {
	"OFF", "Text", "Bar", "Both",
};

char* szBoneModes[] = {
	"OFF", "White", "Health", "Team",
};

char* szHitboxes[] =
{
	"Auto", "Head", "Pelvis", "Lower Torse", "Lower Mid. Torse", "Upper Mid. Torse", "Upper Torse",
	"Left Upper Arm", "Left Lower Arm", "Left Hand", "Right Upper Arm",
	"Right Lower Arm", "Right Hand", "Left Hip", "Left Knee", "Left Foot",
	"Right Hip", "Right Knee", "Right Foot",
};

*/

void Menu::Init()
{
	m_isVisible = false;

	CWindow Main(100, 100, 703, 600);

	// checkbox: 12 pixels

#pragma region Aimbot
	auto AimbotTab = new CChild(0, 0, 220, L"Aimbot");

	AimbotTab->AddControl(new CCheckBox(L"Enabled", &gCvars.aimbot_active));
	AimbotTab->AddControl(new CSlider<int>(L"Field of View", 0, 30, &gCvars.aimbot_fov));
	AimbotTab->AddControl(new CCheckBox(L"Hitscan", &gCvars.aimbot_hitscan));
	AimbotTab->AddControl(new CCombo(&gCvars.aimbot_key, { L"Always", L"Mouse 1", L"Mouse 2", L"Mouse 3", L"Mouse 4", L"Mouse 5", L"Shift", L"Alt", L"F" }));
	AimbotTab->AddControl(new CSlider<int>(L"Smooth", 0, 180, &gCvars.aimbot_smooth));
	AimbotTab->AddControl(new CCheckBox(L"Ignore Disguise", &gCvars.aimbot_ignore_disguise));
	AimbotTab->AddControl(new CCheckBox(L"Ignore Taunt", &gCvars.aimbot_ignore_taunt));
	AimbotTab->AddControl(new CCheckBox(L"Zoomed Only", &gCvars.aimbot_zoomedonly));

	Main.AddControl(AimbotTab);
#pragma endregion
#pragma region Triggerbot
	auto TriggerbotTab = new CChild(230, 0, 220, L"Triggerbot");

	TriggerbotTab->AddControl(new CCheckBox(L"Enabled", &gCvars.triggerbot_active));
	TriggerbotTab->AddControl(new CCombo(&gCvars.triggerbot_key, { L"Always", L"Mouse 1", L"Mouse 2", L"Mouse 3", L"Mouse 4", L"Mouse 5", L"Shift", L"Alt", L"F" })); // NEEDS LIST
	TriggerbotTab->AddControl(new CCheckBox(L"Head Only", &gCvars.triggerbot_headonly));
	TriggerbotTab->AddControl(new CCheckBox(L"Auto Airblast", &gCvars.triggerbot_autoairblast));
	TriggerbotTab->AddControl(new CCombo(&gCvars.triggerbot_autoairblast_key, { L"Always", L"Mouse 1", L"Mouse 2", L"Mouse 3", L"Mouse 4", L"Mouse 5", L"Shift", L"Alt", L"F" })); // NEEDS LIST


	Main.AddControl(TriggerbotTab);
#pragma endregion
#pragma region ESP
	auto ESPTab = new CChild(0, 150, 220, L"ESP");

	ESPTab->AddControl(new CCheckBox(L"Enabled", &gCvars.esp_active));
	ESPTab->AddControl(new CCombo(&gCvars.esp_key, { L"Always", L"Mouse 1", L"Mouse 2", L"Mouse 3", L"Mouse 4", L"Mouse 5", L"Shift", L"Alt", L"F" })); // NEEDS LIST
	ESPTab->AddControl(new CCheckBox(L"Enemies Only", &gCvars.esp_enemyonly));
	ESPTab->AddControl(new CCheckBox(L"Box", &gCvars.esp_box));
	ESPTab->AddControl(new CCombo(&gCvars.esp_box_selection, { L"Outlined", L"Box"})); 
	ESPTab->AddControl(new CCheckBox(L"Name", &gCvars.esp_name));
	ESPTab->AddControl(new CCheckBox(L"Class", &gCvars.esp_class));
	ESPTab->AddControl(new CCheckBox(L"Weapon", &gCvars.esp_weapons));
	ESPTab->AddControl(new CCheckBox(L"Cond", &gCvars.esp_playerCond_enabled));
	ESPTab->AddControl(new CCombo(&gCvars.esp_playerCond, { L"Dumpster Fire", L"NullCore" })); // NEEDS LIST
	ESPTab->AddControl(new CCheckBox(L"Remove Cloak", &gCvars.esp_removeCloak));
	ESPTab->AddControl(new CCheckBox(L"Remove Disguise", &gCvars.esp_removeDisguise));
	ESPTab->AddControl(new CCheckBox(L"Remove Taunt", &gCvars.esp_removeTaunt));
	ESPTab->AddControl(new CCheckBox(L"Health", &gCvars.esp_health_enabled));
	ESPTab->AddControl(new CCombo(&gCvars.esp_health, { L"Text", L"Bar", L"Both" })); // NEEDS LIST
	ESPTab->AddControl(new CCheckBox(L"Sentry", &gCvars.esp_buildings));
	ESPTab->AddControl(new CCheckBox(L"Box", &gCvars.esp_box_buildings));
	ESPTab->AddControl(new CCombo(&gCvars.esp_health_buildings, { L"Text", L"Bar", L"Both" }));

	Main.AddControl(ESPTab);
#pragma endregion

#pragma region Misc
	auto MiscTab = new CChild(230, 100, 220, L"Misc");
	MiscTab->AddControl(new CCheckBox(L"Clean Screenshot", &gCvars.misc_cleanScreenshot));
	MiscTab->AddControl(new CCheckBox(L"AutoBackstab", &gCvars.misc_autobackstab));

	Main.AddControl(MiscTab);
#pragma endregion

	MenuForm.AddWindow(Main);
}

void Menu::Kill()
{}

void Menu::PaintTraverse()
{
	if (m_isVisible)
		MenuForm.Paint();
}

void Menu::Click()
{
	if (m_isVisible)
		MenuForm.Click();
}

void Menu::HandleInput(WPARAM vk)
{
	if (m_isVisible)
		MenuForm.HandleInput(vk);
}

void Menu::Toggle()
{
	m_isVisible = !m_isVisible;
}
