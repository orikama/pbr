#pragma once

#include "core.hpp"

#include <map>
#include <utility>
#include <functional>
#include <mutex>


// TODO: Try string_view as a key to std::map (Heterogeneous Lookup)
// TODO: Will unordered_map be better ?
// TODO: Why book implementation has wierd .cpp/.h separation ? Move all functions to .cpp ?


PBR_NAMESPACE_BEGIN

// ******************************************************************************
// ------------------------------- STATS COUNTER --------------------------------
// ******************************************************************************
#if PBR_ENABLE_STATS_COUNT == 1

class StatsAccumulator
{
public:
    void ReportCounter(const std::string &name, i64 value)
    {
        m_counters[name] += value;
    }
    void ReportMemoryCounter(const std::string &name, i64 value)
    {
        m_memoryCounters[name] += value;
    }
    void ReportPercentage(const std::string &name, i64 value, i64 denominator)
    {
        m_percentages[name].first += value;
        m_percentages[name].second += denominator;
    }
    void ReportRatio(const std::string &name, i64 value, i64 denominator)
    {
        m_ratios[name].first += value;
        m_ratios[name].second += denominator;
    }

private:
    std::map<std::string, i64> m_counters;
    std::map<std::string, i64> m_memoryCounters;
    std::map<std::string, std::pair<i64, i64>> m_percentages;
    std::map<std::string, std::pair<i64, i64>> m_ratios;
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


#define PBR_STATS_VARIABLE_INCREMENT(variable) ++(variable);
#define PBR_STATS_VARIABLE_ADD(variable, value) (variable) += (value);

// TODO: Why variable=0 is necessary ?
#define PBR_STATS_COUNTER(title, variable)                                        \
    static thread_local i64 variable;                                             \
    static void _PBR_STATS_FUNC_##variable(StatsAccumulator &accumulator)         \
    {                                                                             \
        accumulator.ReportCounter(title, variable);                               \
        variable = 0;                                                             \
    }                                                                             \
    static StatsRegisterer _PBR_STATS_REG_##variable(_PBR_STATS_FUNC_##variable); \

#define PBR_STATS_MEMORY_COUNTER(title, variable)                                 \
    static thread_local i64 variable;                                             \
    static void _PBR_STATS_FUNC_##variable(StatsAccumulator &accumulator)         \
    {                                                                             \
        accumulator.ReportMemoryCounter(title, variable);                         \
        variable = 0;                                                             \
    }                                                                             \
    static StatsRegisterer _PBR_STATS_REG_##variable(_PBR_STATS_FUNC_##variable); \

#define PBR_STATS_PERCENT(title, variable, denominator)                           \
    static thread_local i64 variable, denominator;                                \
    static void _PBR_STATS_FUNC_##variable(StatsAccumulator &accumulator)         \
    {                                                                             \
        accumulator.ReportPercentage(title, variable, denominator);               \
        denominator = variable = 0;                                               \
    }                                                                             \
    static StatsRegisterer _PBR_STATS_REG_##variable(_PBR_STATS_FUNC_##variable); \

#define PBR_STATS_RATIO(title, variable, denominator)                             \
    static thread_local i64 variable, denominator;                                \
    static void _PBR_STATS_FUNC_##variable(StatsAccumulator &accumulator)         \
    {                                                                             \
        accumulator.ReportRatio(title, variable, denominator);                    \
        denominator = variable = 0;                                               \
    }                                                                             \
    static StatsRegisterer _PBR_STATS_REG_##variable(_PBR_STATS_FUNC_##variable); \

#else // PBR_ENABLE_STATS_COUNT

#define PBR_STATS_VARIABLE_INCREMENT(variable)
#define PBR_STATS_VARIABLE_ADD(variable, value)

#define PBR_STATS_COUNTER(title, variable)
#define PBR_STATS_MEMORY_COUNTER(title, variable)
#define PBR_STATS_PERCENT(title, variable, denominator)
#define PBR_STATS_RATIO(title, variable, denominator)

#endif // PBR_ENABLE_STATS_COUNT


// ******************************************************************************
// ---------------------------------- PROFILER ----------------------------------
// ******************************************************************************
#if PBR_ENABLE_PROFILING == 1

// NOTE: Keep the same order as in the book.
enum class ProfileCategory : i32
{
    Triangle_Intersect,
    Triangle_IsIntersecting,
    Shape_Intersect,
    Shape_IsIntersecting
};


extern thread_local ui64 g_ProfilerState;

inline
ui64 ProfileCategoryToBits(ProfileCategory pc)
{
    return 1ull << i32(pc);
}


// DIFFERENCE: Copy constructor and assign operator not marked as =delete.
class ProfilePhase
{
public:
    ProfilePhase(ProfileCategory pc)
    {
        m_categoryBit = ProfileCategoryToBits(pc);
        m_reset = (g_ProfilerState & m_categoryBit) == 0;
        g_ProfilerState |= m_categoryBit;
    }
    ~ProfilePhase()
    {
        if (m_reset)
            g_ProfilerState &= ~m_categoryBit;
    }

private:
    ui64 m_categoryBit;
    bool m_reset;
};

#define PBR_PROFILE_FUNCTION(category) ProfilePhase _PBR_profile_phase(category);

#else // PBR_ENABLE_PROFILING

#define PBR_PROFILE_FUNCTION(category)

#endif // PBR_ENABLE_PROFILING

PBR_NAMESPACE_END
