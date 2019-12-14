#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"

__attribute__((constructor)) void lib_main();

extern "C" void load();
