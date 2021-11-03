// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Crazy_Santa : ModuleRules
{
	public Crazy_Santa(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
