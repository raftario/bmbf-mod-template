#include "../extern/beatsaber-hook/shared/utils/utils.h"

MAKE_HOOK(example_hook, 0x000000, void, void *self);

__attribute__((constructor)) void lib_main();