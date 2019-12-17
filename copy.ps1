$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
& adb push libs/arm64-v8a/lib{{ mod.out }}.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/lib{{ mod.out }}.so
& adb shell am force-stop com.beatgames.beatsaber
