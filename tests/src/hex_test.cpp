#include <memory>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex.h>

using namespace tile;

TEST(hex_test, create_hex_test)
{
  // When creating hex points, the input coordinates should always sum to 0.
  // Default coordinates should be q=0, r=0, s=0.
  Hex test_object;
  ASSERT_EQ(0, test_object.q());
  ASSERT_EQ(0, test_object.r());
  ASSERT_EQ(0, test_object.s());

  // s = -q - r. When q & r are given, s should be set accordingly.
  int8_t q = 0;
  int8_t r = 1;
  int8_t s = -1;
  test_object = Hex(q, r);
  ASSERT_EQ(q, test_object.q());
  ASSERT_EQ(r, test_object.r());
  ASSERT_EQ(s, test_object.s());

  // You should be able to copy Hexs
  Hex other(q + 1, r + 1);

  test_object = Hex(other);
  ASSERT_EQ(other.q(), test_object.q());
  ASSERT_EQ(other.r(), test_object.r());
  ASSERT_EQ(other.s(), test_object.s());
}

TEST(hex_test, eq_hex_test)
{
  Hex test_object(0, 0);
  Hex eq_object(0, 0);
  Hex diff_object(1, 1);

  ASSERT_TRUE(test_object == eq_object);
  ASSERT_FALSE(test_object == diff_object);
  ASSERT_FALSE(test_object != eq_object);
  ASSERT_TRUE(test_object != diff_object);
}

TEST(hex_test, add_hexes_test)
{
  int8_t q_1 = 0;
  int8_t r_1 = -1;
  int8_t s_1 = 1;
  Hex test_object_1(q_1, r_1);

  int8_t q_2 = 1;
  int8_t r_2 = 1;
  int8_t s_2 = -2;
  Hex test_object_2(q_2, r_2);

  Hex added = test_object_1 + test_object_2;
  ASSERT_EQ(q_1 + q_2, added.q());
  ASSERT_EQ(r_1 + r_2, added.r());
  ASSERT_EQ(s_1 + s_2, added.s());

  test_object_1 += test_object_2;
  ASSERT_EQ(q_1 + q_2, test_object_1.q());
  ASSERT_EQ(r_1 + r_2, test_object_1.r());
  ASSERT_EQ(s_1 + s_2, test_object_1.s());
}

TEST(hex_test, subtract_hexes_test)
{
  int8_t q_1 = 0;
  int8_t r_1 = -1;
  int8_t s_1 = 1;
  Hex test_object_1(q_1, r_1);

  int8_t q_2 = 1;
  int8_t r_2 = 1;
  int8_t s_2 = -2;
  Hex test_object_2(q_2, r_2);

  Hex subracted = test_object_1 - test_object_2;
  ASSERT_EQ(q_1 - q_2, subracted.q());
  ASSERT_EQ(r_1 - r_2, subracted.r());
  ASSERT_EQ(s_1 - s_2, subracted.s());

  test_object_1 -= test_object_2;
  ASSERT_EQ(q_1 - q_2, test_object_1.q());
  ASSERT_EQ(r_1 - r_2, test_object_1.r());
  ASSERT_EQ(s_1 - s_2, test_object_1.s());
}

TEST(hex_test, distance_test)
{
  Hex a(0, 0);
  Hex b(1, -1);
  Hex c(0, 1);
  Hex d(-1, 0);

  // Should be able to handle all axis.
  ASSERT_EQ(1, a.distance(b));
  ASSERT_EQ(1, a.distance(c));
  ASSERT_EQ(1, a.distance(d));

  // Shouldn't matter which hex we measure from.
  ASSERT_EQ(1, b.distance(a));

  // Hexes more than 1 away should be okay.
  ASSERT_EQ(2, b.distance(c));
}

TEST(hex_test, neighboring_points_test)
{
  Hex test_object(0, 0);

  Hex neighbor = test_object.neighbor(Direction::north_west);
  ASSERT_EQ(0, neighbor.q());
  ASSERT_EQ(-1, neighbor.r());
  ASSERT_EQ(1, neighbor.s());

  neighbor = test_object.neighbor(Direction::north_east);
  ASSERT_EQ(1, neighbor.q());
  ASSERT_EQ(-1, neighbor.r());
  ASSERT_EQ(0, neighbor.s());

  neighbor = test_object.neighbor(Direction::east);
  ASSERT_EQ(1, neighbor.q());
  ASSERT_EQ(0, neighbor.r());
  ASSERT_EQ(-1, neighbor.s());

  neighbor = test_object.neighbor(Direction::south_east);
  ASSERT_EQ(0, neighbor.q());
  ASSERT_EQ(1, neighbor.r());
  ASSERT_EQ(-1, neighbor.s());

  neighbor = test_object.neighbor(Direction::south_west);
  ASSERT_EQ(-1, neighbor.q());
  ASSERT_EQ(1, neighbor.r());
  ASSERT_EQ(0, neighbor.s());

  neighbor = test_object.neighbor(Direction::west);
  ASSERT_EQ(-1, neighbor.q());
  ASSERT_EQ(0, neighbor.r());
  ASSERT_EQ(1, neighbor.s());

  neighbor = test_object.move_q(3);
  ASSERT_EQ(0, neighbor.q());
  ASSERT_EQ(-3, neighbor.r());
  ASSERT_EQ(3, neighbor.s());

  neighbor = test_object.move_r(4);
  ASSERT_EQ(4, neighbor.q());
  ASSERT_EQ(0, neighbor.r());
  ASSERT_EQ(-4, neighbor.s());

  neighbor = test_object.move_s(5);
  ASSERT_EQ(-5, neighbor.q());
  ASSERT_EQ(5, neighbor.r());
  ASSERT_EQ(0, neighbor.s());

  // Chaining neighbor calls should be okay.
  ASSERT_EQ(
      test_object.neighbor(Direction::north_west),
      test_object.neighbor(Direction::west).neighbor(Direction::north_east));

  ASSERT_EQ(test_object.move_q(2), test_object.neighbor(Direction::west)
                                       .neighbor(Direction::north_west)
                                       .neighbor(Direction::north_west)
                                       .neighbor(Direction::east));
}