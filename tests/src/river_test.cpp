#include <memory>
#include <set>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <portables/Transporter.h>
#include <tiles/components/Border.h>
#include <tiles/components/River.h>

using namespace tile;

TEST(river_test, create_river_test)
{
  std::set<Direction> points;
  points.insert(Direction::north_west);
  points.insert(Direction::south_west);
  // When creating rivers, an unique ID should be added for each river.
  std::set<uuids::uuid> ids;
  River a = River(points);

  // New ID should not be empty, and should follow the expected uuid format.
  uuids::uuid id = a.get_id();
  ASSERT_TRUE(!id.is_nil());
  ASSERT_EQ(16, id.as_bytes().size());
  ASSERT_EQ(uuids::uuid_version::random_number_based, id.version());
  ASSERT_EQ(uuids::uuid_variant::rfc, id.variant());

  ids.insert(a.get_id());

  // IDs should not be duplicated
  a = River(points);
  id = a.get_id();
  ASSERT_FALSE(ids.contains(id));
  ids.insert(id);

  // Creating with a different amount of river points shouldn't matter
  points.insert(Direction::east);
  a = River(points);
  id = a.get_id();
  ASSERT_FALSE(ids.contains(id));
  ids.insert(id);

  // Copying should transfer everything, including the ID.
  River b = River(a);
  uuids::uuid copied_id = b.get_id();
  ASSERT_TRUE(ids.contains(copied_id));
  ASSERT_EQ(id, copied_id);
  ASSERT_EQ(a, b);
}

TEST(river_test, rotate_river_test)
{
  // Rotating a river should be clockwise. If the input value is negative, the
  // the rotation is counter-clockwise. This should rotate all of the river
  // points and bridges accordingly.
  std::set<Direction> points;
  points.insert(Direction::north_west);
  points.insert(Direction::east);
  points.insert(Direction::south_east);

  River test_object(points);
  EXPECT_EQ(common::ERR_NONE, test_object.build_bridge(Direction::north_west));

  // When rotating 0, nothing should move
  test_object.rotate(0);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
  EXPECT_TRUE(test_object.has_bridge(Direction::north_west));

  // When rotating 1, everything should move clockwise 1 step
  test_object.rotate(1);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  EXPECT_FALSE(test_object.has_point(Direction::north_west));
  EXPECT_TRUE(test_object.has_point(Direction::north_east));
  EXPECT_FALSE(test_object.has_point(Direction::east));
  EXPECT_TRUE(test_object.has_point(Direction::south_east));
  EXPECT_TRUE(test_object.has_point(Direction::south_west));
  EXPECT_TRUE(test_object.has_bridge(Direction::north_east));

  // When rotating -1, everything should move counter-clockwise 1 step
  // Subsequent rotations should be additive.
  test_object.rotate(-1);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
  EXPECT_TRUE(test_object.has_bridge(Direction::north_west));

  // When rotating 8, everything should move effectively clockwise 2 steps
  test_object.rotate(8);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  EXPECT_FALSE(test_object.has_point(Direction::north_west));
  EXPECT_TRUE(test_object.has_point(Direction::east));
  EXPECT_TRUE(test_object.has_point(Direction::south_west));
  EXPECT_FALSE(test_object.has_point(Direction::south_east));
  EXPECT_TRUE(test_object.has_point(Direction::west));
  EXPECT_TRUE(test_object.has_bridge(Direction::east));

  // When rotating -14, everything should move effectively counter-clockwise 2
  // steps
  test_object.rotate(-14);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
  EXPECT_TRUE(test_object.has_bridge(Direction::north_west));
}