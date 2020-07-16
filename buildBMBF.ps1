# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
Compress-Archive -Path "./libs/arm64-v8a/lib{{ mod.out }}.so","./bmbfmod.json","./include/libs/libbeatsaber-hook_2019_2_1f1_0_2_0.so" -DestinationPath "./{{ mod.out }}_v0.1.0.zip" -Update
