// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Voxta : ModuleRules
{
	public Voxta(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Voxta" });

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}