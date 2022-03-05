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

  // Should fail trying to rotate with a bridge on it
  EXPECT_EQ(common::ERR_NONE, test_object.build(Direction::north_west));
  ASSERT_EQ(common::ERR_FAIL, test_object.rotate(0));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
  ASSERT_EQ(common::ERR_FAIL, test_object.rotate(1));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
  ASSERT_EQ(common::ERR_FAIL, test_object.rotate(-1));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }

  // Reset for the rest of the tests.
  test_object = River(points);

  // When rotating 0, nothing should move
  ASSERT_EQ(common::ERR_NONE, test_object.rotate(0));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }

  // When rotating 1, everything should move clockwise 1 step
  ASSERT_EQ(common::ERR_NONE, test_object.rotate(1));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  EXPECT_FALSE(test_object.has_point(Direction::north_west));
  EXPECT_TRUE(test_object.has_point(Direction::north_east));
  EXPECT_FALSE(test_object.has_point(Direction::east));
  EXPECT_TRUE(test_object.has_point(Direction::south_east));
  EXPECT_TRUE(test_object.has_point(Direction::south_west));

  // When rotating -1, everything should move counter-clockwise 1 step
  // Subsequent rotations should be additive.
  test_object.rotate(-1);
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }

  // When rotating 8, everything should move effectively clockwise 2 steps
  ASSERT_EQ(common::ERR_NONE, test_object.rotate(8));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  EXPECT_FALSE(test_object.has_point(Direction::north_west));
  EXPECT_TRUE(test_object.has_point(Direction::east));
  EXPECT_TRUE(test_object.has_point(Direction::south_west));
  EXPECT_FALSE(test_object.has_point(Direction::south_east));
  EXPECT_TRUE(test_object.has_point(Direction::west));

  // When rotating -14, everything should move effectively counter-clockwise 2
  // steps
  ASSERT_EQ(common::ERR_NONE, test_object.rotate(-14));
  EXPECT_EQ(points.size(), test_object.get_points().size());
  for (auto p : points)
  {
    EXPECT_TRUE(test_object.has_point(p));
  }
}

TEST(river_test, area_borders_test)
{
  // It should be possible to determine area borders based on how a river
  // divides the tile. A river should return pairings of borders that define
  // adjacent areas' borders.

  // These first tests assume only 1 river is on a tile.
  // When there's only one river point, the area should effectively not be
  // divided. This should still only create 1 area: NW_right->NW_left
  std::set<Direction> points;
  points.insert(Direction::north_west);
  River test_object(points);
  std::vector<std::set<Border>> results = test_object.get_area_borders();
  ASSERT_EQ(1, results.size());
  ASSERT_EQ(ALL_BORDERS, results[0]);

  // If we add another river point, the number of potential areas should
  // increase. This should create 2 areas: NW_right->SW_left, &
  // SW_right->NW_left
  points.insert(Direction::south_west);
  test_object = River(points);
  results = test_object.get_area_borders();
  ASSERT_EQ(2, results.size());

  ASSERT_EQ(8, results[0].size());
  for (int i = Border::NW_right; i <= Border::SW_left; i++)
  {
    ASSERT_TRUE(results[0].contains(static_cast<Border>(i)));
  }

  ASSERT_EQ(4, results[1].size());
  for (int i = Border::SW_right; i <= Border::W_right; i++)
  {
    ASSERT_TRUE(results[1].contains(static_cast<Border>(i)));
  }
  ASSERT_TRUE(results[1].contains(Border::NW_left));

  // If we instead split part of a tile with the river, we should only use the
  // borders we were given.
  // This should still create two areas:
  // NW_right->E_left + SE_right->SW_left
  // SW_right->NW_left
  std::set<Border> partial = ALL_BORDERS;
  partial.erase(Border::E_right);
  partial.erase(Border::SE_left);
  results = test_object.get_area_borders(partial);
  ASSERT_EQ(2, results.size());

  ASSERT_EQ(6, results[0].size());
  for (int i = Border::NW_right; i <= Border::E_left; i++)
  {
    ASSERT_TRUE(results[0].contains(static_cast<Border>(i)));
  }
  ASSERT_FALSE(results[0].contains(Border::E_right));
  ASSERT_FALSE(results[0].contains(Border::SE_left));
  for (int i = Border::SE_right; i <= Border::SW_left; i++)
  {
    ASSERT_TRUE(results[0].contains(static_cast<Border>(i)));
  }

  ASSERT_EQ(4, results[1].size());
  for (int i = Border::SW_right; i <= Border::W_right; i++)
  {
    ASSERT_TRUE(results[1].contains(static_cast<Border>(i)));
  }
  ASSERT_TRUE(results[1].contains(Border::NW_left));

  // If we try to split a partial area that this river does not flow throw, the
  // resulting list should just return the input.
  std::set<Border> unused;
  unused.insert(Border::E_right);
  unused.insert(Border::SE_left);
  results = test_object.get_area_borders(unused);
  ASSERT_EQ(1, results.size());
  ASSERT_TRUE(results[0].contains(Border::E_right));
  ASSERT_TRUE(results[0].contains(Border::SE_left));

  // Adding another point should create another area. This should create:
  // NW_right->E_left,
  // E_right->SW_left, &
  // SW_right->NW_left.
  points.insert(Direction::east);
  test_object = River(points);
  results = test_object.get_area_borders();
  ASSERT_EQ(3, results.size());

  ASSERT_EQ(4, results[0].size());
  for (int i = Border::NW_right; i <= Border::E_left; i++)
  {
    ASSERT_TRUE(results[0].contains(static_cast<Border>(i)));
  }

  ASSERT_EQ(4, results[1].size());
  for (int i = Border::E_right; i <= Border::SW_left; i++)
  {
    ASSERT_TRUE(results[1].contains(static_cast<Border>(i)));
  }

  ASSERT_EQ(4, results[2].size());
  for (int i = Border::SW_right; i <= Border::W_right; i++)
  {
    ASSERT_TRUE(results[2].contains(static_cast<Border>(i)));
  }
  ASSERT_TRUE(results[2].contains(Border::NW_left));
}