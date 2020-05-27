//#include "doctest.h"
//
//#include "core/transform.hpp"
//#include "core/geometry.hpp"
//
//TEST_CASE("Vector3")
//{
//    using namespace pbr;
//    
//    Matrix4x4 m(1, 2, 3, 4,
//                1, 2, 1, 2,
//                4, 5, 6, 2,
//                3, 4, 3, 4);
//
//    auto inv = Inverse(m);
//
//    MESSAGE('\n' << inv[0][0] << '\t' << inv[0][1] << '\t' << inv[0][2] << '\t' << inv[0][3] << '\n'
//                 << inv[1][0] << '\t' << inv[1][1] << '\t' << inv[1][2] << '\t' << inv[1][3] << '\n'
//                 << inv[2][0] << '\t' << inv[2][1] << '\t' << inv[2][2] << '\t' << inv[2][3] << '\n'
//                 << inv[3][0] << '\t' << inv[3][1] << '\t' << inv[3][2] << '\t' << inv[3][3] << '\n');
//
//    CHECK_EQ(1, 1);
//
//    /*SUBCASE("Constructors")
//    {
//        Vector3f v_zero;
//        CHECK_EQ((v_zero.x == 0 && v_zero.y == 0 && v_zero.z == 0), true);
//
//        Vector3f v_one(1);
//        CHECK_EQ((v_one.x == 1 && v_one.y == 1 && v_one.z == 1), true);
//
//        CHECK_EQ((v2.x == 2 && v2.y == 4 && v2.z == 6), true);
//    }*/
//
//}
