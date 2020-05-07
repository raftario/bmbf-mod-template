#include "../include/mod_interface.hpp"

#include <unordered_set>  // specific types that you might want here

#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"  // if you use il2cpp_utils:: methods
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"  // if you use il2cpp_functions:: methods
#include "../extern/beatsaber-hook/shared/utils/utils.h"  // always import last to avoid dumb math.h log errors!

// Hook a void instance method: FireworksController.OnEnable()
// name for the original v     method's return type v    v arguments (start with Il2CppObject* self only for instance methods!)
MAKE_HOOK_OFFSETLESS(FireworksController_OnEnable, void, Il2CppObject* self) {
    // Get an instance field, crashing the game if it fails
    float _minSpawnInterval = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_minSpawnInterval"));
    log(INFO, "FireworksController._minSpawnInterval: %f", _minSpawnInterval);
    // For a softer touch, you can instead return on failure
    auto _spawnSize = RET_V_UNLESS(il2cpp_utils::GetFieldValue<Vector3>(self, "_spawnSize"));
    log(INFO, "FireworksController._spawnSize: (%f, %f, %f)", _spawnSize.x, _spawnSize.y, _spawnSize.z);
    // Set a field
    CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_minSpawnInterval", 0));
    // Call original method, if desired
    FireworksController_OnEnable(self);
}

// Hook a void instance method: HealthWarningFlowCoordinator.DidActivate(bool firstActivation, FlowCoordinator.ActivationType activationType)
// Because FlowCoordinator.ActivationType is an enum that inherits an int, we can simply express it as an int.
MAKE_HOOK_OFFSETLESS(HealthWarningFlowCoordinator_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    // We can ignore one of the parameters by calling the original function with one of the parameters constant:
    log(INFO, "Calling original HealthWarningFlowCoordinator_DidActive with 'false' firstActivation!");
    HealthWarningFlowCoordinator_DidActivate(self, false, activationType);
}

// Hook a bool instance method: BeatmapLevelsModel.IsBeatmapLevelLoaded(string levelId)
// All C# strings should be handled as Il2CppString*
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded, bool, Il2CppObject* self, Il2CppString* levelId) {
    // We can convert an Il2CppString* into a normal string by casting it to a u16string_view, then casting it to utf8
    auto legibleLevelId = to_utf8(csstrtostr(levelId));
    log(INFO, "Attempting to check if %s is already loaded...", legibleLevelId.data());
    // We can call the original function whenever we want
    auto actualIsLoadedValue = BeatmapLevelsModel_IsBeatmapLevelLoaded(self, levelId);
    log(INFO, "The beatmap %s", actualIsLoadedValue ? "is loaded" : "is not loaded");
    return actualIsLoadedValue;  // be sure to actually return the original function's result if you want the caller to have it!
}

// Hook an instance property getter: BeatmapLevelsModel.get_customLevelPackCollection()
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection, Il2CppObject*, Il2CppObject* self) {
    // We can call other methods whenever we want:
    auto* allLoadedBeatmapLevelPackCollection = CRASH_UNLESS(il2cpp_utils::GetPropertyValue(self, "allLoadedBeatmapLevelPackCollection"));
    log(INFO, "allLoadedBeatmapLevelPackCollection pointer: %p", allLoadedBeatmapLevelPackCollection);
    // Since we know that allLoadedBeatmapLevelPackCollection is actually of type: IBeatmapLevelPackCollection, we can use it like one
    auto* beatmapLevelPacks = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppArray*>(
        allLoadedBeatmapLevelPackCollection, "beatmapLevelPacks"));
    log(INFO, "There are a total of: %ui level packs", il2cpp_functions::array_length(beatmapLevelPacks));
    return BeatmapLevelsModel_get_customLevelPackCollection(self);
}

// Hook a void static method: MonoBehaviour.print(object message)
MAKE_HOOK_OFFSETLESS(MonoBehaviour_print, void, Il2CppObject* message) {
    // We can access other static things too, just as usual.
    // For now, however, we will choose to simply perform nothing.
    // We do not have to call the original function if we do not want to, although it may cause code to crash in strange ways.
    return;
}

// This attribute means the function will be called when the mod is loaded for the first time.
// Generally only useful for file-based operations like configs.
__attribute__((constructor)) void lib_main() {
    log(INFO, "Hello from the first time this mod is loaded!");
}

// This function is called once il2cpp_init has been called, so this is the first time in the execution that il2cpp_utils and
// il2cpp_functions can be safely used, and OFFSETLESS hooks installed.
extern "C" void load() {
    log(INFO, "Hello from il2cpp_init!");
    log(INFO, "Installing hooks...");
    // FindMethod expects either an Il2CppClass*/Il2CppObject* OR 2 strings (one for namespace, then one for class name)
    // followed by a method name and then the argument types (if any) as Il2CppType*s.
    auto* strType = il2cpp_functions::class_get_type(il2cpp_functions::defaults->string_class);
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded,
        il2cpp_utils::FindMethod("", "BeatmapLevelsModel", "IsBeatmapLevelLoaded", strType));
    // Just use that without anything for the arguments for methods with no arguments!
    INSTALL_HOOK_OFFSETLESS(FireworksController_OnEnable, il2cpp_utils::FindMethod("", "FireworksController", "OnEnable"));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection,
        il2cpp_utils::FindMethod("", "BeatmapLevelsModel", "get_customLevelPackCollection"));
    // Since grabbing the correct Il2CppType*s is a lot of work, you can use the Unsafe variant to specify only the number of
    // arguments. But beware! It will select the first matching method it finds with no complaints, even if multiple matches exist.
    INSTALL_HOOK_OFFSETLESS(HealthWarningFlowCoordinator_DidActivate, il2cpp_utils::FindMethodUnsafe(
        "", "HealthWarningFlowCoordinator", "DidActivate", 2));
    INSTALL_HOOK_OFFSETLESS(MonoBehaviour_print, il2cpp_utils::FindMethodUnsafe("UnityEngine", "MonoBehaviour", "print", 1));
    // Because of that, you should only use FindMethod when you really need the MethodInfo*. RunMethod can be called in the same
    // way as FindMethod, but you pass actual values instead of the argument types and their types will be automatically extracted!
    
    // To avoid the risk of multiple matches, try to keep the types of your arguments as specific as possible!
    // For example, you *can* grab an enum value as an int:
    static auto* con = CRASH_UNLESS(il2cpp_utils::GetClassFromName("", "OVRInput/Controller"));
    //                                                                   vvvvv using this
    static const int leftTouch = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(con, "LTouch"));
    // But grabbing it as the default Il2CppObject* will allow it to remember its true type!
    static auto* a1D = CRASH_UNLESS(il2cpp_utils::GetClassFromName("", "OVRInput/Axis1D"));
    //                                                                              v no <blah> means default (Il2CppObject*)!
    auto* primaryHandTrigger = CRASH_UNLESS(il2cpp_utils::GetFieldValue(a1D, "PrimaryHandTrigger"));
    // This method (OVRInput.Get) has a lot of 2-argument overloads, but only one with an Axis1D in the first spot!
    float inputValue = CRASH_UNLESS(il2cpp_utils::RunMethod<float>("", "OVRInput", "Get", primaryHandTrigger, leftTouch));
    // And you can still grab the integer from that enum Il2CppObject* using object_unbox:
    int value = *reinterpret_cast<int*>(il2cpp_functions::object_unbox(primaryHandTrigger));
    
    log(INFO, "Installed all hooks!");
}

// UnityEngine.Space
enum class Space {
    World,
    Self
};
// But the type extractor isn't magic. If you copy an enum or struct definition and want to pass it to RunMethod, just use this!
DEFINE_IL2CPP_ARG_TYPE(Space, "UnityEngine", "Space");
// now this RunMethod will treat the enum value as its true enum type:
// CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "Rotate", Vector3_Right, Space::World));
