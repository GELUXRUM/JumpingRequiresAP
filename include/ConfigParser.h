#pragma once
#include "SimpleIni.h"

class ModConfigs
{
public:
	void LoadConfigs(CSimpleIniA& a_ini)
	{
		a_ini.LoadFile("Data\\F4SE\\Plugins\\GLXRM_JumpingRequiresAP.ini");

		jumpStyle = std::stoi(a_ini.GetValue("Configs", "iJumpStyle", "2"));
		jumpCost = std::stof(a_ini.GetValue("Configs", "fJumpCost", "0.25"));
		multPerLegCrippled = std::stof(a_ini.GetValue("Configs", "fMultPerLegCrippled", "1.0"));
		jumpHeightMult = std::stof(a_ini.GetValue("Configs", "fJumpHeightMult", "0.67"));

		a_ini.Reset();

		return;
	}

public:
	int   jumpStyle;
	float jumpCost;
	float multPerLegCrippled;
	float jumpHeightMult;
};
