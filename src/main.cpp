#include "../include/main.hpp"

// Hook a void instance method: FireworksController.OnEnable()
MAKE_HOOK_OFFSETLESS(FireworksController_OnEnable, void, Il2CppObject* self) {
    // Get an instance field
    float _minSpawnInterval;
    if (!il2cpp_utils::GetFieldValue(&_minSpawnInterval, self, "_minSpawnInterval")) {
        // Failed to get field!
        log(CRITICAL, "Failed to get field: _minSpawnInterval on FireworksController!");
    }
    log(INFO, "FireworksController._minSpawnInterval: %f", _minSpawnInterval);
    // An unsafe way of getting a field value and casting it accordingly
    auto _spawnSize = il2cpp_utils::GetFieldValueUnsafe<Vector3>(self, "_spawnSize");
    log(INFO, "FireworksController._spawnSize: (%f, %f, %f)", _spawnSize.x, _spawnSize.y, _spawnSize.z);
    // Set a field
    _minSpawnInterval = 0;
    if (!il2cpp_utils::SetFieldValue(self, "_minSpawnInterval", &_minSpawnInterval)) {
        // Failed to set field!
        log(CRITICAL, "Failed to set field: _minSpawnInterval on FireworksController!");
    }
    // Call original method, if desired
    FireworksController_OnEnable(self);
}

// Hook a void instance method: HealthWarningFlowCoordinator.DidActivate(bool firstActivation, FlowCoordinator.ActivationType activationType)
// Because FlowCoordinator.ActivationType is an enum that inherits an int, we can simply express this as an int.
MAKE_HOOK_OFFSETLESS(HealthWarningFlowCoordinator_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    // We can ignore one of the parameters by calling the original function with one of the parameters constant:
    log(INFO, "Calling original HealthWarningFlowCoordinator_DidActive with 'false' firstActivation!");
    HealthWarningFlowCoordinator_DidActivate(self, false, activationType);
}

// Hook a bool instance method: BeatmapLevelsModel.IsBeatmapLevelLoaded(string levelId)
// All C# strings should be interpreted as Il2CppString*
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded, bool, Il2CppObject* self, Il2CppString* levelId) {
    // We can convert an Il2CppString* into a string_view by casting it to a u16string_view, then casting it to utf8
    auto legibleLevelId = to_utf8(csstrtostr(levelId));
    log(INFO, "Attempting to check if %s is already loaded...", legibleLevelId.data());
    // We can call the original function whenever we want
    auto actualIsLoadedValue = BeatmapLevelsModel_IsBeatmapLevelLoaded(self, levelId);
    log(INFO, "The beatmap %s", actualIsLoadedValue ? "is loaded" : "is not loaded");
    return actualIsLoadedValue;
}

// Hook an instance property getter: BeatmapLevelsModel.get_customLevelPackCollection()
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection, Il2CppObject*, Il2CppObject* self) {
    // We can call other methods whenever we want:
    Il2CppObject* allLoadedBeatmapLevelPackCollection;
    if (!il2cpp_utils::RunMethod(&allLoadedBeatmapLevelPackCollection, self, "get_allLoadedBeatmapLevelPackCollection")) {
        // Failed to run method!
        log(CRITICAL, "Failed to run method: get_allLoadedBeatmapLevelPackCollection on BeatmapLevelsModel!");
    }
    log(INFO, "allLoadedBeatmapLevelPackCollection pointer: %p", allLoadedBeatmapLevelPackCollection);
    // Since we know that allLoadedBeatmapLevelPackCollection is actually of type: IBeatmapLevelPackCollection, we can use it like one
    Il2CppArray* beatmapLevelPacks;
    if (!il2cpp_utils::RunMethod(&beatmapLevelPacks, allLoadedBeatmapLevelPackCollection, "get_beatmapLevelPacks")) {
        // Failed to run method!
        log(CRITICAL, "Failed to run method: get_beatmapLevelPacks of allLoadedBeatmapLevelPackCollection!");
    }
    log(INFO, "There are a total of: %ui level packs", il2cpp_functions::array_length(beatmapLevelPacks));
    return BeatmapLevelsModel_get_customLevelPackCollection(self);
}

// Hook a void static method: MonoBehaviour.print(object message)
MAKE_HOOK_OFFSETLESS(MonoBehaviour_print, void, Il2CppObject* message) {
    // We can access other static things too, just as usual.
    // For now, however, we will choose to simply perform nothing.
    // We do not have to call the original function if we do not want to, although it may cause code to crash in strange ways
    return;
}

// This function is called when the mod is loaded for the first time
__attribute__((constructor)) void lib_main()
{
    log(INFO, "Hello from the first time this mod is loaded!");
}

// This function is called once il2cpp_init has been called, so il2cpp_utils and il2cpp_functions can be used safely here.
// This is where OFFSETLESS hooks must be installed.
extern "C" void load() {
    log(INFO, "Hello from il2cpp_init!");
    log(INFO, "Installing hooks...");
    INSTALL_HOOK_OFFSETLESS(FireworksController_OnEnable, il2cpp_utils::GetMethod("", "FireworksController", "OnEnable", 0));
    INSTALL_HOOK_OFFSETLESS(HealthWarningFlowCoordinator_DidActivate, il2cpp_utils::GetMethod("", "HealthWarningFlowCoordinator", "DidActivate", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded, il2cpp_utils::GetMethod("", "BeatmapLevelsModel", "IsBeatmapLevelLoaded", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection, il2cpp_utils::GetMethod("", "BeatmapLevelsModel", "get_customLevelPackCollection", 0));
    INSTALL_HOOK_OFFSETLESS(MonoBehaviour_print, il2cpp_utils::GetMethod("UnityEngine", "MonoBehaviour", "print", 1));
    log(INFO, "Installed all hooks!");
}