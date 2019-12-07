#include "../include/main.hpp"

#include <android/log.h>
#include "../extern/beatsaber-hook/shared/inline-hook/inlineHook.h"
#include "../extern/beatsaber-hook/shared/utils/logging.h"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"

MAKE_HOOK(hello_world, 0x000000, void, void *self) {
    log(INFO, "Hello world!");
}

__attribute__((constructor)) void lib_main()
{
    INSTALL_HOOK(hello_world);
    log(INFO, "Installed {{ mod.name }} hooks!");
}

extern "C" void load() {
    log(INFO, "Hello from il2cpp_init!");
}