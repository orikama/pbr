#pragma once

#include "core.hpp"
#include "pbr_math.hpp"


// TODO: This class got big improvement, compare to book version.
// TODO: Write tests for it.


namespace pbr {

class EFloat
{
public:
    EFloat() = default;
    // NOTE: What's the use of error argument, why do we need it ?
    explicit EFloat(f32 value, f32 error = 0.f);
#if PBR_EFLOAT_DEBUG == 1
    explicit EFloat(f32 value, f32 error, f64 precise);
#endif

    friend bool operator==(EFloat ef1, EFloat ef2);

    friend EFloat operator-(EFloat ef);

    friend EFloat operator+(EFloat ef1, EFloat ef2);
    friend EFloat operator-(EFloat ef1, EFloat ef2);
    friend EFloat operator*(EFloat ef1, EFloat ef2);
    friend EFloat operator/(EFloat ef1, EFloat ef2);

    explicit operator f32() const { return value; }


    f32 GetAbsoluteError() const {return error; }
    // DIFFERENCE: I'm lazy, and I don't know if there is the difference between std and book implementation.
    f32 UpperBound() const { return std::nexttowardf(value + error, std::numeric_limits<f64>::infinity()); }
    f32 LowerBound() const { return std::nexttowardf(value - error, -std::numeric_limits<f64>::infinity()); }

#if PBR_EFLOAT_DEBUG == 1
    f64 GetPreciseValue() const;
    f32 GetRelativeError() const;
#endif

    friend EFloat Sqrt(EFloat ef);
    friend EFloat Abs(EFloat ef);
    friend bool Quadratic(EFloat a, EFloat b, EFloat c, EFloat &x0, EFloat &x1);


private:
    inline void CheckCorrectness() const;


    f32 value;
    f32 error;
#if PBR_EFLOAT_DEBUG == 1
    f64 precise;
#endif
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

EFloat::EFloat(f32 value, f32 error = 0.f)
    : value(value)
    , error(error)
{
#if PBR_EFLOAT_DEBUG == 1
    precise = value;
    CheckCorrectness();
#endif
}

#if PBR_EFLOAT_DEBUG == 1
EFloat::EFloat(f32 value, f32 error, f64 precise)
    : value(value)
    , error(error)
    , precise(precise)
{
    CheckCorrectness();
}
#endif


// ---------------------------------------
// -------- COMPARISON OPERATORS ---------
// ---------------------------------------

bool operator==(EFloat ef1, EFloat ef2)
{
    return ef1.value == ef2.value && ef1.error == ef2.error;
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

EFloat operator-(EFloat ef)
{
    return EFloat(-ef.value,
                  ef.error
#if PBR_EFLOAT_DEBUG == 1
                  , -ef.precise
#endif
                 );
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

EFloat operator+(EFloat ef1, EFloat ef2)
{
    EFloat result(ef1.value + ef2.value,
                  ef1.error + ef2.error + constants::machineEpsilon * (std::abs(ef1.value + ef2.value) + ef1.error + ef2.error)
#if PBR_EFLOAT_DEBUG == 1
                  , ef1.precise + ef2.precise
#endif
                  );
    result.CheckCorrectness();

    return result;
}

EFloat operator-(EFloat ef1, EFloat ef2)
{
    EFloat result(ef1.value - ef2.value,
                  ef1.error + ef2.error + constants::machineEpsilon * (std::abs(ef1.value - ef2.value) + ef1.error + ef2.error)
#if PBR_EFLOAT_DEBUG == 1
                  , ef1.precise - ef2.precise
#endif
                  );
    result.CheckCorrectness();

    return result;
}

EFloat operator*(EFloat ef1, EFloat ef2)
{
    EFloat result(ef1.value * ef2.value,
                  std::abs(ef1.value * ef2.error) + std::abs(ef2.value * ef1.error) + ef1.error * ef2.error + constants::machineEpsilon * std::abs(ef1.value * ef2.value)
#if PBR_EFLOAT_DEBUG == 1
                  , ef1.precise * ef2.precise
#endif
                  );
    result.CheckCorrectness();

    return result;
}

EFloat operator/(EFloat ef1, EFloat ef2)
{
    EFloat result(ef1.value / ef2.value,
                  (std::abs(ef1.value) + ef1.error) / (std::abs(ef2.value) - ef2.error) - std::abs(ef1.value / ef2.value) + constants::machineEpsilon * (std::abs(ef1.value) + ef1.error) / (std::abs(ef2.value) - ef2.error)
#if PBR_EFLOAT_DEBUG == 1
                  , ef1.precise / ef2.precise
#endif
                  );
    result.CheckCorrectness();

    return result;
}


EFloat operator+(f32 f, EFloat ef)
{
    return EFloat(f) + ef;
}

EFloat operator-(f32 f, EFloat ef)
{
    return EFloat(f) - ef;
}

EFloat operator*(f32 f, EFloat ef)
{
    return EFloat(f) * ef;
}

EFloat operator/(f32 f, EFloat ef)
{
    return EFloat(f) / ef;
}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

inline
void EFloat::CheckCorrectness() const
{
    // DIFFERENCE: In original they allow a value to be NaN or Inf, but why ?
    PBR_ASSERT(std::isfinite(value))
    PBR_ASSERT(error >= 0)
    PBR_ASSERT(value - error <= precise && value + error >= precise)
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

// NOTE: std::sqrt should be good enough
EFloat Sqrt(EFloat ef)
{
    return EFloat(pbr::Sqrt(ef.value),
                  pbr::Sqrt(ef.value + ef.error) - pbr::Sqrt(ef.value - ef.error) + constants::machineEpsilon * pbr::Sqrt(ef.value + ef.error)
#if PBR_EFLOAT_DEBUG == 1
                  , ef.precise / ef.precise
#endif
                 );
}

EFloat Abs(EFloat ef)
{
    return EFloat(std::abs(ef.value),
                  ef.error
#if PBR_EFLOAT_DEBUG == 1
                  , std::abs(ef.precise)
#endif
                 );
}

// DIFFERENCE: Discriminant was double
bool Quadratic(EFloat a, EFloat b, EFloat c, EFloat &x0, EFloat &x1)
{
    fp_t discriminant = b.value * b.value - 4.f * a.value * c.value;
    if(discriminant < 0.f)
        return false;

    fp_t rootD = pbr::Sqrt(discriminant);
    EFloat efD(rootD, constants::machineEpsilon * rootD);

    EFloat q;
    if(b.value < 0.f)
        q = -.5f * (b - efD);
    else
        q = -.5f * (b + efD);

    x0 = q / a;
    x1 = c / q;
    // NOTE: Why ?
    if(x0.value > x1.value)
        std::swap(x0, x1);

    return true;
}

} // namespace pbr
