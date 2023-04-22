// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectFPSServerTarget : TargetRules
{
	public ProjectFPSServerTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ProjectFPS" } );

		bUsesSteam = true;
        GlobalDefinitions.Add("UE_PROJECT_STEAMPRODUCTNAME=\"Highrise Guardian\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDESC=\"Highrise Guardian\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"Highrise Guardian\"");
        GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");
	}
}
