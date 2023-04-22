// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectFPSTarget : TargetRules
{
	public ProjectFPSTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUsesSteam = true;

        DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ProjectFPS" } );
	}
}
