// THIS FILE IS MEANT AS AN EXAMPLE OF API!
// THIS SHOULD NOT BE ASSUMED TO WORK OUT OF THE BOX, INSTEAD USE THIS AS A STARTING POINT TO UNDERSTAND THE API OF beatsaber-hook!
// CREATED BY Sc2ad and zoller27osu

#include "../include/mod_interface.hpp"

// Any specific types you would want here

#include <unordered_set>  // specific types that you might want here

// Any beatsaber-hook specific includes here

#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
// For displaying modloader information (ex: Modloader.getInfo().name)
#include "../extern/beatsaber-hook/include/modloader.hpp"
// For using il2cpp_utils:: methods
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
// For using il2cpp_functions:: methods
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
// For using commonly used types (such as Vector2, Vector3, Color, Scene, etc.)
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
// For using configuration
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"

// Holds the ModInfo object for this mod. This object is set in void setup()
static ModInfo modInfo;

// Returns the configuration object for this mod. Good as a static local, since you should avoid creating muliple within one mod.
static Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns the logger object for this mod. Good as a static local, since you should avoid creating multiple within one mod.
static const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}

// For either of these above functions, in order to use them from other files:
// create a header file and place method declarations within that header.
// Ex: const Logger& getLogger();
// Then, they can be defined within a file that is compiled and included and used elsewhere.

// TODO: make fireworks appear during Health warning?
// Hook a void instance method: FireworksController.OnEnable()
// name for the original v     method's return type v    v arguments (start with Il2CppObject* self only for instance methods!)
MAKE_HOOK_OFFSETLESS(FireworksController_OnEnable, void, Il2CppObject* self) {
    // Get an instance field, crashing the game if it fails for faster debugging/bug reporting
    float _minSpawnInterval = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_minSpawnInterval"));
    getLogger().info("FireworksController._minSpawnInterval: %f", _minSpawnInterval);
    // For a softer touch, you can instead return on failure
    auto _spawnSize = RET_V_UNLESS(il2cpp_utils::GetFieldValue<Vector3>(self, "_spawnSize"));
    getLogger().info("FireworksController._spawnSize: (%f, %f, %f)", _spawnSize.x, _spawnSize.y, _spawnSize.z);
    // Set a field
    CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_minSpawnInterval", 0));
    // this should give the fireworks much more room to be closer or farther from you - some might even appear behind you!
    _spawnSize.z *= 20;
    CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_spawnSize", _spawnSize));
    // Call original method, if desired
    FireworksController_OnEnable(self);
}

// Hook a void instance method: HealthWarningFlowCoordinator.DidActivate(bool firstActivation, FlowCoordinator.ActivationType activationType)
// Because FlowCoordinator.ActivationType is an enum that inherits an int, we can simply express it as an int.
MAKE_HOOK_OFFSETLESS(HealthWarningFlowCoordinator_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    // We can ignore one of the parameters by calling the original function with one of the parameters constant:
    getLogger().info("Calling original HealthWarningFlowCoordinator_DidActive with 'true' firstActivation!");
    HealthWarningFlowCoordinator_DidActivate(self, true, activationType);
}

// Hook a bool instance method: BeatmapLevelsModel.IsBeatmapLevelLoaded(string levelId)
// All C# strings should be handled as Il2CppString*
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded, bool, Il2CppObject* self, Il2CppString* levelId) {
    // We can convert an Il2CppString* into a normal string by casting it to a u16string_view, then casting it to utf8
    auto legibleLevelId = to_utf8(csstrtostr(levelId));
    getLogger().info("Attempting to check if %s is already loaded...", legibleLevelId.data());
    // We can call the original function whenever we want
    auto actualIsLoadedValue = BeatmapLevelsModel_IsBeatmapLevelLoaded(self, levelId);
    getLogger().info("The beatmap %s", actualIsLoadedValue ? "is loaded" : "is not loaded");
    if (actualIsLoadedValue) {
        // We can call other methods whenever we want (this could also be expressed as GetPropertyValue if you remove the get_):
        auto* allLoadedBeatmapLevelPackCollection = CRASH_UNLESS(il2cpp_utils::RunMethod(self, "get_allLoadedBeatmapLevelPackCollection"));
        getLogger().info("allLoadedBeatmapLevelPackCollection pointer: %p", allLoadedBeatmapLevelPackCollection);
        // Since we know that allLoadedBeatmapLevelPackCollection is actually of type: IBeatmapLevelPackCollection, we can use it like one
        auto* beatmapLevelPacks = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppArray*>(
            allLoadedBeatmapLevelPackCollection, "beatmapLevelPacks"));
        getLogger().info("There are a total of: %u level packs", il2cpp_functions::array_length(beatmapLevelPacks));
    }
    return actualIsLoadedValue;  // be sure to actually return the original function's result if you want the caller to have it!
}

// Generally /avoid/ hooking instance property getters like BeatmapLevelsModel.get_customLevelPackCollection()
// A hook needs 3-5 instructions, so hooking methods with too few instructions can result in the next method being overwritten!
MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection, Il2CppObject*, Il2CppObject* self) {
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
    getLogger().info("Hello from the first time this mod is loaded!");
}

// This function is called immediately after the mod is loaded and is used to determine information about the mod.
// This function is useful for setting up other things, such as the logger and configuration objects.
extern "C" void setup(ModInfo& info) {
    // Set the ID and version of this mod
    info.id = "{{ mod.id }}";
    info.version = "0.1.0";
    modInfo = info;
    // Log some information
    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
    // Create and load config here, config can be modified using rapidjson and getConfig().config for the rapidjson::Document
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// This function is called once il2cpp_init has been called, so this is the first time in the execution that il2cpp_utils and
// il2cpp_functions can be safely used, and OFFSETLESS hooks installed.
extern "C" void load() {
    getLogger().debug("Hello from il2cpp_init!");
    getLogger().debug("Installing hooks...");
    // FindMethod expects either an Il2CppClass*/Il2CppObject* OR 2 strings (one for namespace, then one for class name)
    // followed by a method name and then the argument types (if any) as Il2CppType*s.
    // must be called before using any method or field of il2cpp_functions; many il2cpp_utils methods will call it for you:
    il2cpp_functions::Init();
    // il2cpp_functions::defaults exposes the Il2CppDefaults*, which contains pointers to many basic C#/Unity Il2CppClass*s
    auto* strType = il2cpp_functions::class_get_type(il2cpp_functions::defaults->string_class);
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_IsBeatmapLevelLoaded,
        il2cpp_utils::FindMethod("", "BeatmapLevelsModel", "IsBeatmapLevelLoaded", strType));
    // Just use that without anything for the arguments for methods with no arguments!
    INSTALL_HOOK_OFFSETLESS(FireworksController_OnEnable, il2cpp_utils::FindMethod("", "FireworksController", "OnEnable"));
    // This hook may result in ILL_OPC due to the next instruction in memory being corrupted and later called by Unity!
    // INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_get_customLevelPackCollection, il2cpp_utils::FindMethod("", "BeatmapLevelsModel", "get_customLevelPackCollection"));
    
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
    CRASH_UNLESS((intptr_t)primaryHandTrigger > 64);
    // This method (OVRInput.Get) has a lot of 2-argument overloads, but only one with an Axis1D in the first spot!
    float inputValue = CRASH_UNLESS(il2cpp_utils::RunMethod<float>("", "OVRInput", "Get", primaryHandTrigger, leftTouch));
    // And you can still grab the integer from that enum Il2CppObject* using object_unbox:
    int value = *reinterpret_cast<int*>(il2cpp_functions::object_unbox(primaryHandTrigger));
    
    getLogger().debug("Installed all hooks!");
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
