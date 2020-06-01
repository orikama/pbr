#pragma once

#include "core.hpp"

#include <map>
#include <functional>
#include <mutex>


// TODO: Try string_view as a key to std::map (Heterogeneous Lookup)
// TODO: Will unordered_map be better ?
// TODO: Why book implementation has wierd .cpp/.h separation ? Move all functions to .cpp ?


namespace pbr {

// ******************************************************************************
// ----------------------------------- STATS ------------------------------------
// ******************************************************************************
#if PBR_ENABLE_STATS_COUNT == 1

class StatsAccumulator
{
public:
    void ReportCounter(const std::string &name, i64 value)
    {
        m_counters[name] += value;
    }

private:
    std::map<std::string, i64> m_counters;
};


class StatsRegisterer
{
public:
    StatsRegisterer(std::function<void(StatsAccumulator &)> callback)
    {
        static std::mutex m;
        std::lock_guard lock(m);

// TODO: Is this check necessary ?
        if(m_callbacks == nullptr)
            m_callbacks = new std::vector<std::function<void(StatsAccumulator &)>>;

        m_callbacks->push_back(callback);
    }

    static void CallCallbacks(StatsAccumulator &accumulator);

private:
// TODO: unique_ptr ?
// TODO: Delete pointer ?
    static std::vector<std::function<void(StatsAccumulator &)>> *m_callbacks;
};


// TODO: Why variable=0 is necessary ?
#define PBR_STATS_COUNTER(title, variable)                                          \
    static thread_local i64 variable;                                               \
    static void _PBR_STATS_FUNC_##variable(StatsAccumulator &accumulator)           \
    {                                                                               \
        accumulator.ReportCounter(title, variable);                                 \
        variable = 0;                                                               \
    }                                                                               \
    static StatsRegisterer _PBR_STATS_REG_##variable(_PBR_STATS_FUNC_##variable);   \

#endif // PBR_ENABLE_STATS_COUNT

} // namespace pbr
