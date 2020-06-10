#include "stats.h"


namespace pbr {

#if PBR_ENABLE_PROFILING == 1

thread_local ui64 g_ProfilerState;

#endif // PBR_ENABLE_PROFILING

} // namespace pbr
