#include "Config.h"
#include <string>
#include "SDK.h"
#include <sstream>

GConfig gConfig;

void GConfig::GetVal(Json::Value &config, int* setting)
{
	if (config.isNull())
		return;

	*setting = config.asInt();
}

void GConfig::GetVal(Json::Value &config, bool* setting)
{
	if (config.isNull())
		return;

	*setting = config.asBool();
}

void GConfig::GetVal(Json::Value &config, float* setting)
{
	if (config.isNull())
		return;

	*setting = config.asFloat();
}
void GConfig::GetVal(Json::Value &config, char** setting)
{
	if (config.isNull())
		return;

	*setting = strdup(config.asCString());
}

void GConfig::GetVal(Json::Value &config, char* setting)
{
	if (config.isNull())
		return;

	strcpy(setting, config.asCString());
}

void GConfig::GetVal(Json::Value &config, ImVec4* setting, bool loadAlpha)
{
	if (config.isNull())
		return;

	GetVal(config["r"], &setting->x);
	GetVal(config["g"], &setting->y);
	GetVal(config["b"], &setting->z);
	if (loadAlpha)
		GetVal(config["a"], &setting->w);
}

template <typename Ord, Ord(*lookupFunction)(std::string)>
void GetOrdinal(Json::Value& config, Ord* setting)
{
	if (config.isNull())
		return;

	Ord value;
	if (config.isString())
		value = lookupFunction(config.asString());
	else
		value = (Ord)config.asInt();

	*setting = value;
}
inline bool GConfig::exists_test3(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool GConfig::LoadSettings(const char* path)
{
	if (!exists_test3(path))
		return false;
	Json::Value settings;
	std::ifstream configDoc(path, std::ifstream::binary);
	configDoc >> settings;

	auto section = "Aimbot";
	GetVal(settings[section]["Enabled"], &gCvars.aimbot_active);
	GetVal(settings[section]["Silent"], &gCvars.aimbot_silent);
	GetVal(settings[section]["Autoshoot"], &gCvars.aimbot_autoshoot);
	GetVal(settings[section]["WaitForChargeAmby"], &gCvars.aimbot_waitforcharge);
	GetVal(settings[section]["Hitscan"], &gCvars.aimbot_hitscan);
	GetVal(settings[section]["Aimkey"], &gCvars.aimbot_key);
	GetVal(settings[section]["Hitbox"], &gCvars.aimbot_hitbox);
	GetVal(settings[section]["Smooth"], &gCvars.aimbot_smooth);
	GetVal(settings[section]["SmoothAMT"], &gCvars.aimbot_smooth_amt);
	GetVal(settings[section]["FovAMT"], &gCvars.aimbot_fov);
	GetVal(settings[section]["IgnoreCloak"], &gCvars.aimbot_ignore_cloak);
	GetVal(settings[section]["ZoomedOnly"], &gCvars.aimbot_zoomedonly);

	section = "Triggerbot";
	GetVal(settings[section]["Enabled"], &gCvars.triggerbot_active);
	GetVal(settings[section]["TriggerKey"], &gCvars.triggerbot_key);
	GetVal(settings[section]["HeadOnly"], &gCvars.triggerbot_headonly);
	GetVal(settings[section]["AutoAirblast"], &gCvars.triggerbot_autoairblast);
	GetVal(settings[section]["AirblastKey"], &gCvars.triggerbot_autoairblast_key);
	GetVal(settings[section]["AirblastLegit"], &gCvars.triggerbot_autoairblast_legit);
	GetVal(settings[section]["AirblastSilent"], &gCvars.triggerbot_autoairblast_silent);

	section = "AntiAim";
	GetVal(settings[section]["Enabled"], &gCvars.aa_enabled);
	GetVal(settings[section]["Pitch"], &gCvars.aa_pitch_enabled);
	GetVal(settings[section]["PitchAA"], &gCvars.aa_pitch);
	GetVal(settings[section]["Yaw"], &gCvars.aa_yaw_enabled);
	GetVal(settings[section]["YawAA"], &gCvars.aa_yaw);
	GetVal(settings[section]["Spinspeed"], &gCvars.aa_spinspeed);
	GetVal(settings[section]["Resolver"], &gCvars.aa_resolver);

	section = "ESP";
	GetVal(settings[section]["Enabled"], &gCvars.esp_active);
	GetVal(settings[section]["Local"], &gCvars.esp_local);
	GetVal(settings[section]["Weapon"], &gCvars.esp_weapons);
	GetVal(settings[section]["EnemiesOnly"], &gCvars.esp_enemyonly);
	GetVal(settings[section]["Box"], &gCvars.esp_box);
	GetVal(settings[section]["BoxBuildings"], &gCvars.esp_box_buildings);
	GetVal(settings[section]["BoxSelection"], &gCvars.esp_box_selection);
	GetVal(settings[section]["Name"], &gCvars.esp_name);
	GetVal(settings[section]["Class"], &gCvars.esp_class);
	GetVal(settings[section]["CondEnabled"], &gCvars.esp_playerCond_enabled);
	GetVal(settings[section]["CondSelection"], &gCvars.esp_playerCond);
	GetVal(settings[section]["AntiCloak"], &gCvars.esp_removeCloak);
	GetVal(settings[section]["AntiDisguise"], &gCvars.esp_removeDisguise);
	GetVal(settings[section]["AntiTaunt"], &gCvars.esp_removeTaunt);
	GetVal(settings[section]["HealthEnabled"], &gCvars.esp_health_enabled);
	GetVal(settings[section]["HealthSelection"], &gCvars.esp_health);
	GetVal(settings[section]["BonesEnabled"], &gCvars.esp_bones_enabled);
	GetVal(settings[section]["BonesSelection"], &gCvars.esp_bones);
	GetVal(settings[section]["HappyFace"], &gCvars.esp_face);
	GetVal(settings[section]["Skychanger"], &gCvars.sky_changer_value);
	GetVal(settings[section]["Crosshair"], &gCvars.esp_crosshair);
	GetVal(settings[section]["Objects"], &gCvars.esp_objects);
	GetVal(settings[section]["HealthBuildings"], &gCvars.esp_health_buildings);

	/*section = "RemoveCond";
	GetVal(settings[section]["Enabled"], &gCvars.removecond_enabled);
	GetVal(settings[section]["Key"], &gCvars.removecond_key);
	GetVal(settings[section]["Value"], &gCvars.removecond_value);
	GetVal(settings[section]["AutoMedkit"], &gCvars.removecond_automedkit);
	GetVal(settings[section]["AutoCap"], &gCvars.removecond_autoinstantcap);
	GetVal(settings[section]["InstaWeapSwitch"], &gCvars.removecond_instantweaponswitch);
	GetVal(settings[section]["Stickyspam"], &gCvars.removecond_stickyspam);*/

	section = "Misc";
	GetVal(settings[section]["BunnyHop"], &gCvars.misc_bunnyhop);
	GetVal(settings[section]["AutoStrafe"], &gCvars.misc_autostrafe);
	GetVal(settings[section]["Thirdperson"], &gCvars.misc_thirdPerson);
	GetVal(settings[section]["AutoBackstab"], &gCvars.misc_autobackstab);
	GetVal(settings[section]["Noisemaker Spam"], &gCvars.misc_noisemaker_spam);
	GetVal(settings[section]["NoScope"], &gCvars.misc_noscope);
	GetVal(settings[section]["NoZoom"], &gCvars.misc_nozoom);
	GetVal(settings[section]["NoPush"], &gCvars.misc_no_push);
	GetVal(settings[section]["ChatspamEnabled"], &gCvars.misc_chatspam_enabled);
	GetVal(settings[section]["ChatspamSelection"], &gCvars.misc_chatspam_selection);
	GetVal(settings[section]["Delay"], &gCvars.misc_chatspam_delay);
	GetVal(settings[section]["Newlines"], &gCvars.misc_chatspam_newline);
	GetVal(settings[section]["KillsayEnabled"], &gCvars.misc_killsay_enabled);
	GetVal(settings[section]["KillsaySelection"], &gCvars.misc_killsay_selection);
	GetVal(settings[section]["Voicespam"], &gCvars.misc_voice);
	GetVal(settings[section]["RollSpeedhack"], &gCvars.misc_roll_speedhack);
	GetVal(settings[section]["WOWSweet"], &gCvars.misc_wowsweet);
	GetVal(settings[section]["BigHead"], &gCvars.misc_bighead);
	GetVal(settings[section]["BigHeadSize"], &gCvars.misc_bigheadsize);
	GetVal(settings[section]["BigTorsoSize"], &gCvars.misc_torsosize);
	GetVal(settings[section]["BigHandSize"], &gCvars.misc_handsize);
	GetVal(settings[section]["Backtrack"], &gCvars.backtrack);
	//GetVal(settings[section]["ServerLagger"], &gCvars.misc_lagger);
	GetVal(settings[section]["ServerLaggerAMT"], &gCvars.misc_laggeramt);
	GetVal(settings[section]["LaggerKey"], &gCvars.misc_lagkey);
	GetVal(settings[section]["CleanSS"], &gCvars.misc_cleanScreenshot);
	GetVal(settings[section]["FOV"], &gCvars.misc_fov);
	GetVal(settings[section]["VFOV"], &gCvars.misc_viewmodel_fov);





	return true;
}

bool GConfig::SaveSettings(const char* path)
{
	try
	{
		Json::Value settings;
		Json::StyledWriter styledWriter;
		
		auto section = "Aimbot";
		settings[section]["WaitForChargeAmby"] = gCvars.aimbot_waitforcharge;
		settings[section]["Enabled"] = gCvars.aimbot_active;
		settings[section]["Silent"] = gCvars.aimbot_silent;
		settings[section]["Autoshoot"] = gCvars.aimbot_autoshoot;
		settings[section]["Hitscan"] = gCvars.aimbot_hitscan;
		settings[section]["Aimkey"] = gCvars.aimbot_key;
		settings[section]["Hitbox"] = gCvars.aimbot_hitbox;
		settings[section]["Smooth"] = gCvars.aimbot_smooth;
		settings[section]["SmoothAMT"] = gCvars.aimbot_smooth_amt;
		settings[section]["FovAMT"] = gCvars.aimbot_fov;
		settings[section]["IgnoreCloak"] = gCvars.aimbot_ignore_cloak;
		settings[section]["ZoomedOnly"] = gCvars.aimbot_zoomedonly;


		section = "Triggerbot";
		settings[section]["Enabled"] = gCvars.triggerbot_active;
		settings[section]["TriggerKey"] = gCvars.triggerbot_key;
		settings[section]["HeadOnly"] = gCvars.triggerbot_headonly;
		settings[section]["AutoAirblast"] = gCvars.triggerbot_autoairblast;
		settings[section]["AirblastKey"] = gCvars.triggerbot_autoairblast_key;
		settings[section]["AirblastLegit"] = gCvars.triggerbot_autoairblast_legit;
		settings[section]["AirblastSilent"] = gCvars.triggerbot_autoairblast_silent;

		section = "AntiAim";
		settings[section]["Enabled"] = gCvars.aa_enabled;
		settings[section]["Pitch"] = gCvars.aa_pitch_enabled;
		settings[section]["PitchAA"] = gCvars.aa_pitch;
		settings[section]["Yaw"] = gCvars.aa_yaw_enabled;
		settings[section]["YawAA"] = gCvars.aa_yaw;
		settings[section]["Spinspeed"] = gCvars.aa_spinspeed;
		settings[section]["Resolver"] = gCvars.aa_resolver;

		section = "ESP";
		settings[section]["Enabled"] = gCvars.esp_active;
		settings[section]["Local"] = gCvars.esp_local;
		settings[section]["Weapon"] = gCvars.esp_weapons;
		settings[section]["EnemiesOnly"] = gCvars.esp_enemyonly;
		settings[section]["Box"] = gCvars.esp_box;
		settings[section]["BoxBuildings"] = gCvars.esp_box_buildings;
		settings[section]["BoxSelection"] = gCvars.esp_box_selection;
		settings[section]["Name"] = gCvars.esp_name;
		settings[section]["Class"] = gCvars.esp_class;
		settings[section]["CondEnabled"] = gCvars.esp_playerCond_enabled;
		settings[section]["CondSelection"] = gCvars.esp_playerCond;
		settings[section]["AntiCloak"] = gCvars.esp_removeCloak;
		settings[section]["AntiDisguise"] = gCvars.esp_removeDisguise;
		settings[section]["AntiTaunt"] = gCvars.esp_removeTaunt;
		settings[section]["HealthEnabled"] = gCvars.esp_health_enabled;
		settings[section]["HealthSelection"] = gCvars.esp_health;
		settings[section]["BonesEnabled"] = gCvars.esp_bones_enabled;
		settings[section]["BonesSelection"] = gCvars.esp_bones;
		settings[section]["HappyFace"] = gCvars.esp_face;
		settings[section]["Skychanger"] = gCvars.sky_changer_value;
		settings[section]["Crosshair"] = gCvars.esp_crosshair;
		settings[section]["Objects"] = gCvars.esp_objects;
		settings[section]["HealthBuildings"] = gCvars.esp_health_buildings;

		/*section = "RemoveCond";
		settings[section]["Enabled"] = gCvars.removecond_enabled;
		settings[section]["Key"] = gCvars.removecond_key;
		settings[section]["Value"] = gCvars.removecond_value;
		settings[section]["AutoMedkit"] = gCvars.removecond_automedkit;
		settings[section]["AutoCap"] = gCvars.removecond_autoinstantcap;
		settings[section]["InstaWeapSwitch"] = gCvars.removecond_instantweaponswitch;
		settings[section]["Stickyspam"] = gCvars.removecond_stickyspam;*/

		section = "Misc";
		settings[section]["BunnyHop"] = gCvars.misc_bunnyhop;
		settings[section]["AutoStrafe"] = gCvars.misc_autostrafe;
		settings[section]["Thirdperson"] = gCvars.misc_thirdPerson;
		settings[section]["AutoBackstab"] = gCvars.misc_autobackstab;
		settings[section]["Noisemaker Spam"] = gCvars.misc_noisemaker_spam;
		settings[section]["NoScope"] = gCvars.misc_noscope;
		settings[section]["NoZoom"] = gCvars.misc_nozoom;
		settings[section]["NoPush"] = gCvars.misc_no_push;
		settings[section]["ChatspamEnabled"] = gCvars.misc_chatspam_enabled;
		settings[section]["ChatspamSelection"] = gCvars.misc_chatspam_selection;
		settings[section]["Delay"] = gCvars.misc_chatspam_delay;
		settings[section]["Newlines"] = gCvars.misc_chatspam_newline;
		settings[section]["KillsayEnabled"] = gCvars.misc_killsay_enabled;
		settings[section]["KillsaySelection"] = gCvars.misc_killsay_selection;
		settings[section]["Voicespam"] = gCvars.misc_voice;
		settings[section]["RollSpeedhack"] = gCvars.misc_roll_speedhack;
		settings[section]["WOWSweet"] = gCvars.misc_wowsweet;
		settings[section]["BigHead"] = gCvars.misc_bighead;
		settings[section]["BigHeadSize"] = gCvars.misc_bigheadsize;
		settings[section]["BigTorsoSize"] = gCvars.misc_torsosize;
		settings[section]["BigHandSize"] = gCvars.misc_handsize;
		settings[section]["Backtrack"] = gCvars.backtrack;
	//	settings[section]["ServerLagger"] = gCvars.misc_lagger;
		settings[section]["ServerLaggerAMT"] = gCvars.misc_laggeramt;
		settings[section]["LaggerKey"] = gCvars.misc_lagkey;
		settings[section]["CleanSS"] = gCvars.misc_cleanScreenshot;
		settings[section]["FOV"] = gCvars.misc_fov;
		settings[section]["VFOV"] = gCvars.misc_viewmodel_fov;




		std::ofstream(path) << styledWriter.write(settings);
		return true;
	}
	catch (...)
	{
		return false;
	}
}