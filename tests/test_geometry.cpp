#include "doctest.h"

#include "core/geometry.hpp"


// may be try TEST_CASE_TEMPLATE
// https://github.com/onqtam/doctest/blob/master/doc/markdown/parameterized-tests.md


TEST_CASE("Vector3")
{
    using namespace pbr;

    Vector3f v1(-1, -2, -3);
    Vector3f v2(2, 4, 6);

    SUBCASE("Constructors")
    {
        Vector3f v_zero;
        CHECK_EQ((v_zero.x == 0 && v_zero.y == 0 && v_zero.z == 0), true);

        Vector3f v_one(1);
        CHECK_EQ((v_one.x == 1 && v_one.y == 1 && v_one.z == 1), true);

        CHECK_EQ((v2.x == 2 && v2.y == 4 && v2.z == 6), true);
    }
    SUBCASE("Relational operators")
    {
        REQUIRE_EQ(v1, v1);
        REQUIRE_NE(v1, v2);
        REQUIRE_LT(v1, v2);
        REQUIRE_GT(v2, v1);
        REQUIRE_LE(v1, v2);
        REQUIRE_LE(v1, v1);
        REQUIRE_GE(v2, v1);
        REQUIRE_GE(v2, v2);
    }
    SUBCASE("Compound ariphmetic operators (vector,vector)")
    {
        Vector3f v_t(0);
        v_t += v1;
        CHECK_EQ(v_t, Vector3f(-1, -2, -3));
        v_t -= v2;
        CHECK_EQ(v_t, Vector3f(-3, -6, -9));
    }
    SUBCASE("Compound ariphmetic operators (vector,scalar)")
    {
        Vector3f v_t(2);
        v_t *= 2;
        CHECK_EQ(v_t, Vector3f(4));
        v_t /= -8;
        CHECK_EQ(v_t, Vector3f(-0.5f));
    }
    SUBCASE("Unray ariphmetic operators")
    {
        CHECK_EQ(-v1, Vector3f(1, 2, 3));
    }
    SUBCASE("Binary ariphmetic operators (vector,vector)")
    {
        CHECK_EQ(v1 + v2, Vector3f(1, 2, 3));
        CHECK_EQ(v1 - v2, Vector3f(-3, -6, -9));
        CHECK_EQ(v1 * v2, Vector3f(-2, -8, -18));
    }
    SUBCASE("Binary ariphmetic operators (vector,scalar)")
    {
        CHECK_EQ(v1 * -2.f, Vector3f(2, 4, 6));
        CHECK_EQ(v1 / 2.f, Vector3f(-0.5f, -1, -1.5f));
    }
    SUBCASE("Binary ariphmetic operators (scalar,vector)")
    {
        CHECK_EQ(-2.f * v1, Vector3f(2, 4, 6));
    }
}
