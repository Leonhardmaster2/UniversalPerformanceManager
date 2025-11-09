// Copyright Universal Performance Manager. All Rights Reserved.

using UnrealBuildTool;

public class UniversalPerformanceManager : ModuleRules
{
    public UniversalPerformanceManager(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RHI",
            "RenderCore",
            "Json",
            "JsonUtilities",
            "UMG",
            "Slate",
            "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ApplicationCore"
        });

        // If you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
