// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectFPSEditorTarget : TargetRules
{
	public ProjectFPSEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        bUsesSteam = true;

        DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ProjectFPS" } );
	}
}
