#include <iostream>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <players/Color.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>

using namespace tile;

std::vector<std::shared_ptr<Area>>
copy_areas(std::vector<std::shared_ptr<Area>> areas)
{
  std::vector<std::shared_ptr<Area>> copied;
  for (auto area : areas)
  {
    copied.push_back(std::make_shared<Area>(area->get_borders()));
  }
  return copied;
}

std::vector<std::shared_ptr<River>>
copy_rivers(std::vector<std::shared_ptr<River>> rivers)
{
  std::vector<std::shared_ptr<River>> copied;
  for (auto river : rivers)
  {
    copied.push_back(std::make_shared<River>(river->get_points()));
  }
  return copied;
}

void check_rivers(std::vector<std::shared_ptr<River>> exp,
                  std::vector<std::shared_ptr<River>> actual, bool should_equal)
{
  ASSERT_EQ(exp.size(), actual.size());
  for (size_t i = 0; i < exp.size(); i++)
  {
    if (should_equal)
    {
      EXPECT_EQ(exp.at(i)->get_points(), actual.at(i)->get_points());
    }
    else
    {
      EXPECT_NE(exp.at(i)->get_points(), actual.at(i)->get_points());
    }
  }
}

void check_areas(std::vector<std::shared_ptr<Area>> exp,
                 std::vector<std::shared_ptr<Area>> actual, bool should_equal)
{
  ASSERT_EQ(exp.size(), actual.size());
  for (size_t i = 0; i < exp.size(); i++)
  {
    if (should_equal)
    {
      EXPECT_EQ(exp.at(i)->get_borders(), actual.at(i)->get_borders());
    }
    else
    {
      EXPECT_NE(exp.at(i)->get_borders(), actual.at(i)->get_borders());
    }
  }
}

TEST(tile_test, create_tile_test)
{
  Hex hex(1, 0);
  Terrain terrain = Terrain::plains;
  std::vector<std::shared_ptr<River>> rivers;
  std::vector<std::shared_ptr<Area>> areas;
  Tile test_object = Tile(hex, terrain);

  // Terrain and hex point should match input
  ASSERT_EQ(terrain, test_object.get_terrain());
  ASSERT_EQ(hex, test_object.get_hex());
  // We didn't specify neighbors/rivers, so those should be empty.
  ASSERT_EQ(0, test_object.get_rivers().size());
  ASSERT_EQ(1, test_object.get_areas().size());
  // Walls should also initialize as empty
  for (int i = 0; i < MAX_DIRECTIONS; i++)
  {
    ASSERT_EQ(nullptr, test_object.get_neighbor(static_cast<Direction>(i)));
    building::Wall wall = test_object.get_wall(static_cast<Direction>(i));
    ASSERT_EQ(player::Color::neutral, wall.color);
    ASSERT_EQ(0, wall.thickness);
  }

  ASSERT_EQ(ALL_BORDERS, (*test_object.get_areas().at(0)));

  // When we create a tile with rivers, it should create areas based on
  // those river points.
  // A river with just one point should result in a single area.
  std::set<Direction> river_points;
  river_points.insert(Direction::north_west);
  test_object = Tile(hex, river_points, terrain);

  ASSERT_EQ(river_points, test_object.get_river_points());
  ASSERT_EQ(1, test_object.get_areas().size());
  ASSERT_EQ(ALL_BORDERS, (*test_object.get_area(Border::NW_left)));

  // A river with two points should result in two areas.
  // With river points at north_west and south_west, the areas created should
  // be: NW_right->SW_left SW_right->NW_left
  std::set<Border> area_1, area_2;
  area_1.insert(Border::NW_right);
  area_1.insert(Border::NE_left);
  area_1.insert(Border::NE_right);
  area_1.insert(Border::E_left);
  area_1.insert(Border::E_right);
  area_1.insert(Border::SE_left);
  area_1.insert(Border::SE_right);
  area_1.insert(Border::SW_left);
  area_2.insert(Border::SW_right);
  area_2.insert(Border::W_left);
  area_2.insert(Border::W_right);
  area_2.insert(Border::NW_left);
  river_points.insert(Direction::south_west);
  test_object = Tile(hex, river_points, terrain);

  ASSERT_EQ(river_points, test_object.get_river_points());
  ASSERT_EQ(2, test_object.get_areas().size());

  ASSERT_EQ(area_1, (*test_object.get_area(Border::NW_right)));
  ASSERT_EQ(area_2, (*test_object.get_area(Border::SW_right)));

  // A river with 3 points should result in 3 areas.
  // With river points at north_west, south_east, and south_west, the areas
  // created should be:
  // NW_right->SE_left,
  // SE_right->SW_left, &
  // SW_right->NW_left
  std::set<Border> area_3;
  area_1.erase(Border::SE_right);
  area_1.erase(Border::SW_left);
  area_3.insert(Border::SE_right);
  area_3.insert(Border::SW_left);
  river_points.insert(Direction::south_east);
  test_object = Tile(hex, river_points, terrain);

  ASSERT_EQ(river_points, test_object.get_river_points());
  ASSERT_EQ(3, test_object.get_areas().size());

  ASSERT_EQ(area_1, (*test_object.get_area(Border::NW_right)));
  ASSERT_EQ(area_2, (*test_object.get_area(Border::SW_right)));
  ASSERT_EQ(area_3, (*test_object.get_area(Border::SE_right)));

  // Two rivers with 2 points each should result in 3 areas.
  // With a river flowing north_west->south_west, and another flowing
  // east->south_east, the areas created should be:
  // NW_right->E_left + SE_right->SW_left,
  // E_right->SE_left, &
  // SW_right->NW_left
  area_1.clear();
  area_2.clear();
  area_3.clear();
  area_1.insert(Border::NW_right);
  area_1.insert(Border::NE_left);
  area_1.insert(Border::NE_right);
  area_1.insert(Border::E_left);
  area_1.insert(Border::SE_right);
  area_1.insert(Border::SW_left);
  area_2.insert(Border::E_right);
  area_2.insert(Border::SE_left);
  area_3.insert(Border::SW_right);
  area_3.insert(Border::W_left);
  area_3.insert(Border::W_right);
  area_3.insert(Border::NW_left);

  river_points.erase(Direction::south_east);
  std::set<Direction> river_points_2;
  river_points_2.insert(Direction::east);
  river_points_2.insert(Direction::south_east);
  std::vector<std::set<Direction>> river_point_sets;
  river_point_sets.push_back(river_points);
  river_point_sets.push_back(river_points_2);
  test_object = Tile(hex, river_point_sets, terrain);

  ASSERT_EQ(2, test_object.get_rivers().size());
  ASSERT_EQ(river_points, test_object.get_river_points(Direction::north_west));
  ASSERT_EQ(river_points_2, test_object.get_river_points(Direction::east));

  ASSERT_EQ(3, test_object.get_areas().size());
  ASSERT_EQ(area_1, (*test_object.get_area(Border::NW_right)));
  ASSERT_EQ(area_2, (*test_object.get_area(Border::E_right)));
  ASSERT_EQ(area_3, (*test_object.get_area(Border::SW_right)));
}

TEST(tile_test, add_neighbor_test)
{
  // Tile placement is limited by river points for each tile, and whether
  // either tile already has a neighbor in the given direction.
  // Rivers should continue from tile to tile; this means if a tile has a
  // river point on the side that we're trying to add a neighbor, the neighbor
  // must also have a river point on its corresponding side.
  std::set<Direction> river_points;
  river_points.insert(Direction::north_east);

  Hex hex(0, 0);
  std::shared_ptr<Tile> test_object = std::make_shared<Tile>(hex);
  std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  std::shared_ptr<Tile> river_neighbor = std::make_shared<Tile>(river_points);

  // Adding a nonexistent neighbor or direction should be invalid.
  ASSERT_EQ(common::ERR_INVALID,
            test_object->add_neighbor(neighbor, static_cast<Direction>(-1)));
  std::shared_ptr<Tile> added = test_object->get_neighbor(Direction::east);
  EXPECT_EQ(nullptr, added);
  EXPECT_EQ(common::ERR_INVALID,
            test_object->add_neighbor(neighbor, static_cast<Direction>(6)));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_EQ(nullptr, added);
  EXPECT_EQ(common::ERR_INVALID,
            test_object->add_neighbor(nullptr, Direction::east));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_EQ(nullptr, added);

  // Adding ourselves as a neighbor should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test_object->add_neighbor(test_object, Direction::east));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_EQ(nullptr, added);

  // Adding a neighbor without any rivers where we don't have rivers should be
  // fine.
  EXPECT_EQ(common::ERR_NONE,
            test_object->add_neighbor(neighbor, Direction::east));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added, neighbor);

  // Adding the same neighbor again should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test_object->add_neighbor(neighbor, Direction::north_east));
  added = test_object->get_neighbor(Direction::north_east);
  EXPECT_EQ(nullptr, added);

  // Trying to add another neighbor in the same direction should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test_object->add_neighbor(river_neighbor, Direction::east));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added, neighbor);

  // Remove from tile's neighbors.
  EXPECT_EQ(common::ERR_NONE, test_object->remove_neighbor(Direction::east));
  EXPECT_EQ(nullptr, test_object->get_neighbor(Direction::east));

  // Trying to add a neighbor with a river where we don't match should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test_object->add_neighbor(river_neighbor, Direction::south_west));
  added = test_object->get_neighbor(Direction::south_west);
  EXPECT_EQ(nullptr, added);

  // Here we reset the test tile to have a river point on the south_west
  // border.
  std::set<Direction> test_river_points;
  test_river_points.insert(Direction::south_west);
  test_object.reset();
  test_object = std::make_shared<Tile>(Tile(hex, test_river_points));
  // Adding a neighbor with a shared river point on the boundary should be
  // fine.
  EXPECT_EQ(common::ERR_NONE,
            test_object->add_neighbor(river_neighbor, Direction::south_west));
  added = test_object->get_neighbor(Direction::south_west);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added, river_neighbor);

  // Remove from tile's neighbors.
  EXPECT_EQ(common::ERR_NONE,
            test_object->remove_neighbor(Direction::south_west));
  EXPECT_EQ(nullptr, test_object->get_neighbor(Direction::south_west));

  // Adding a neighbor that doesn't have a river where we do should fail
  EXPECT_EQ(common::ERR_FAIL,
            test_object->add_neighbor(neighbor, Direction::south_west));
  added = test_object->get_neighbor(Direction::south_west);
  EXPECT_EQ(nullptr, added);

  // Sea tiles are the exception to this; they can ignore river/land
  // border restrictions.
  std::shared_ptr<Tile> sea_neighbor =
      std::make_shared<Tile>(Tile(Terrain::sea));
  EXPECT_EQ(common::ERR_NONE,
            test_object->add_neighbor(sea_neighbor, Direction::east));
  added = test_object->get_neighbor(Direction::east);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added, sea_neighbor);
  // Remove from tile's neighbors.
  EXPECT_EQ(common::ERR_NONE, test_object->remove_neighbor(Direction::east));
  EXPECT_EQ(nullptr, test_object->get_neighbor(Direction::east));

  // The reverse should also be true; any tile can be placed next to a sea
  // tile, provided all other checks pass.
  EXPECT_EQ(common::ERR_NONE,
            sea_neighbor->add_neighbor(test_object, Direction::west));
  added = sea_neighbor->get_neighbor(Direction::west);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added, test_object);
}

TEST(tile_test, build_road_test)
{
  // Building a road from the tile's perspective should be as simple as telling
  // the associated area to do it. That area should enforce any restrictions it
  // has to building a road (as in not allowing two roads built on an undivided
  // side, and only 1 road per border). The tile should add an additional check
  // that it has a neighboring tile to share the road with, and that neither are
  // sea tiles. On a successful build, the neighbor should list the road in its
  // corresponding border.
  std::set<Direction> rp;
  rp.insert(Direction::north_east);
  rp.insert(Direction::south_west);
  rp.insert(Direction::west);
  Hex hex(0, 0);
  std::shared_ptr<Tile> test_object = std::make_shared<Tile>(hex, rp);
  std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  std::shared_ptr<Tile> river_neighbor = std::make_shared<Tile>(rp);
  std::shared_ptr<Tile> sea_neighbor = std::make_shared<Tile>(Terrain::sea);
  ASSERT_EQ(common::ERR_NONE,
            test_object->add_neighbor(neighbor, Direction::east));
  ASSERT_EQ(common::ERR_NONE,
            test_object->add_neighbor(river_neighbor, Direction::south_west));
  ASSERT_EQ(common::ERR_NONE,
            test_object->add_neighbor(sea_neighbor, Direction::north_west));
  ASSERT_EQ(common::ERR_NONE,
            neighbor->add_neighbor(test_object, Direction::west));
  ASSERT_EQ(common::ERR_NONE,
            river_neighbor->add_neighbor(test_object, Direction::north_east));
  ASSERT_EQ(common::ERR_NONE,
            sea_neighbor->add_neighbor(test_object, Direction::south_east));

  // Don't allow a road to be built to a nonexistent neighbor or on sea tiles
  EXPECT_EQ(common::ERR_FAIL, test_object->build_road(Border::NE_left));
  EXPECT_FALSE(test_object->has_road(Border::NE_left));
  EXPECT_EQ(common::ERR_FAIL, test_object->build_road(Border::NW_left));
  EXPECT_FALSE(test_object->has_road(Border::NW_left));
  EXPECT_FALSE(test_object->has_road(Border::SE_right));

  // Building roads to each valid neighbor should be fine.
  EXPECT_EQ(common::ERR_NONE, test_object->build_road(Border::E_left));
  EXPECT_TRUE(test_object->has_road(Border::E_left));
  EXPECT_TRUE(neighbor->has_road(Border::W_right));
  EXPECT_EQ(common::ERR_NONE, test_object->build_road(Border::SW_left));
  EXPECT_TRUE(test_object->has_road(Border::SW_left));
  EXPECT_TRUE(river_neighbor->has_road(Border::NE_right));

  // Building a road on the same border should fail (from us or our neighbors).
  EXPECT_EQ(common::ERR_FAIL, test_object->build_road(Border::E_left));
  EXPECT_EQ(common::ERR_FAIL, neighbor->build_road(Border::W_right));
  EXPECT_EQ(common::ERR_FAIL, test_object->build_road(Border::SW_left));
  EXPECT_EQ(common::ERR_FAIL, river_neighbor->build_road(Border::NE_right));

  // Trying to build another road on an undivided side should fail.
  EXPECT_EQ(common::ERR_FAIL, test_object->build_road(Border::E_right));
  EXPECT_EQ(common::ERR_FAIL, neighbor->build_road(Border::W_left));

  // Building another road on a divided side should be fine.
  EXPECT_EQ(common::ERR_NONE, test_object->build_road(Border::SW_right));
  EXPECT_TRUE(test_object->has_road(Border::SW_right));
  EXPECT_TRUE(river_neighbor->has_road(Border::NE_left));
}

TEST(tile_test, build_bridge_test)
{
  // Building a bridge from the tile's perspective should be as simple as
  // telling the associated river to do it. That river should enforce any
  // restrictions it has to building a bridge.
  std::set<Direction> rp;
  rp.insert(Direction::north_east);
  rp.insert(Direction::south_west);
  rp.insert(Direction::west);
  std::shared_ptr<Tile> test_object = std::make_shared<Tile>(rp);
  test_object->set_hex(Hex(0, 0));

  // Don't allow a bridge to be built on a point that a river is not there for.
  EXPECT_EQ(common::ERR_FAIL, test_object->build_bridge(Direction::north_west));
  EXPECT_FALSE(test_object->get_river(Direction::north_east)
                   ->has_bridge(Direction::north_west));

  // Building a bridge at a valid location should be fine.
  EXPECT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::north_east));
  EXPECT_TRUE(test_object->get_river(Direction::north_east)
                  ->has_bridge(Direction::north_east));

  // Don't allow building at a location twice
  EXPECT_EQ(common::ERR_FAIL, test_object->build_bridge(Direction::north_east));

  // Building at another point on a river fork should be fine
  EXPECT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::south_west));
  EXPECT_TRUE(test_object->get_river(Direction::north_east)
                  ->has_bridge(Direction::north_east));
  EXPECT_TRUE(test_object->get_river(Direction::north_east)
                  ->has_bridge(Direction::south_west));

  // Trying to build an extranneous bridge is not allowed
  EXPECT_EQ(common::ERR_FAIL, test_object->build_bridge(Direction::west));
  EXPECT_FALSE(test_object->get_river(Direction::north_east)
                   ->has_bridge(Direction::west));
}

TEST(tile_test, accessible_areas_test)
{
  // Building a bridge effectively merges the two areas on either side of the
  // river. Since transporters need to know if they have access to
  // resources/buildings in areas due to bridges on a tile, the tile needs to
  // list out all areas available from a given side of the tile.
  std::set<Direction> rp;
  std::shared_ptr<Tile> test_object = std::make_shared<Tile>();
  test_object->set_hex(Hex(0, 0));

  // A tile with no rivers and no bridges should have just 1 area, and
  // therefore list that as accessible from any side of the tile.
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    EXPECT_EQ(1, actual.size());
    if (actual.size() > 0)
    {
      auto expected = test_object->get_area(Border::NW_left);
      EXPECT_EQ(expected, actual.at(0));
    }
  }
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    EXPECT_EQ(1, actual.size());
    if (actual.size() > 0)
    {
      auto expected = test_object->get_area(Border::NW_left);
      EXPECT_EQ(expected, actual.at(0));
    }
  }

  // A tile with an unbridged river should start to limit accessible areas
  // from either side.
  rp.insert(Direction::east);
  rp.insert(Direction::west);
  test_object = std::make_shared<Tile>(rp);
  test_object->set_hex(Hex(0, 0));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    if (test_object->has_river_point(d))
    {
      // If the transporter is a sea transporter on the river, it should still
      // have access to both sides.
      auto expected = test_object->get_areas();
      EXPECT_EQ(2, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else
    {
      // Otherwise, it should only have access to the area the transporter is
      // docked at.
      auto expected = test_object->get_areas(d);
      EXPECT_EQ(1, actual.size());
      EXPECT_EQ(expected, actual);
    }
  }
  // A land transporter on a given side of the river should only have access to
  // the area they are currently in.
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    EXPECT_EQ(1, actual.size());
    if (actual.size() > 0)
    {
      auto expected = test_object->get_area(b);
      EXPECT_EQ(expected, actual.at(0));
    }
  }
  // Bridging the river should only affect land transporters and docked sea
  // transporters in this case.
  ASSERT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::east));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    auto expected = test_object->get_areas();
    EXPECT_EQ(2, actual.size());
    EXPECT_EQ(expected, actual);
  }
  // With the bridge, all areas should be accessible from all sides
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    auto expected = test_object->get_areas();
    EXPECT_EQ(2, actual.size());
    EXPECT_EQ(expected, actual);
  }

  // A tile with a river that splits should provide more interesting tests.
  rp.clear();
  rp.insert(Direction::north_west);
  rp.insert(Direction::east);
  rp.insert(Direction::south_west);
  test_object = std::make_shared<Tile>(rp);
  test_object->set_hex(Hex(0, 0));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    if (test_object->has_river_point(d))
    {
      // Sea transporters on the river. Should have access to everything
      auto expected = test_object->get_areas();
      EXPECT_EQ(3, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else
    {
      // Sea transporter docked on the side of the tile. Should have access
      // matching land transporters in the area it's docked at.
      auto expected = test_object->get_areas(d);
      EXPECT_EQ(1, actual.size());
      EXPECT_EQ(expected, actual);
    }
  }
  // Land transporters should only have access to the area they are currently
  // in.
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    auto expected = test_object->get_area(b);
    EXPECT_EQ(1, actual.size());
    if (actual.size() > 0)
    {
      EXPECT_EQ(expected, actual.at(0));
    }
  }
  // Bridging one of the two river points should give some access to land
  // transporters.
  ASSERT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::east));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    if (test_object->has_river_point(d))
    {
      // Sea transporters on the river are unchanged
      auto expected = test_object->get_areas();
      EXPECT_EQ(3, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else if ((Direction::north_east == d) || (Direction::south_east == d))
    {
      auto expected = test_object->get_areas(Direction::east);
      EXPECT_EQ(2, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else
    {
      auto expected = test_object->get_area(Border::W_left);
      EXPECT_EQ(1, actual.size());
      if (actual.size() > 0)
      {
        EXPECT_EQ(expected, actual.at(0));
      }
    }
  }
  // Land transporters in areas touching the bridge should have access to
  // both bridged areas.
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    // one of the areas touching the bridge
    auto actual = test_object->get_accessible_areas(b);
    if ((b >= Border::NW_right) && (b <= Border::SW_left))
    {
      auto expected = test_object->get_areas(Direction::east);
      EXPECT_EQ(2, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else
    {
      auto expected = test_object->get_areas(Direction::west);
      EXPECT_EQ(1, actual.size());
      if (actual.size() > 0)
      {
        EXPECT_EQ(expected, actual);
      }
    }
  }
  // Adding a second bridge should grant access to all areas to both sea and
  // land transporters
  ASSERT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::south_west));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    auto expected = test_object->get_areas();
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(expected, actual);
  }
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    auto expected = test_object->get_areas();
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(expected, actual);
  }

  // A tile with two rivers should affect access for sea transporters
  rp.clear();
  std::vector<std::set<Direction>> rivers;
  rp.insert(Direction::north_west);
  rp.insert(Direction::south_west);
  rivers.push_back(rp);
  rp.clear();
  rp.insert(Direction::north_east);
  rp.insert(Direction::east);
  rivers.push_back(rp);
  test_object = std::make_shared<Tile>(rivers);
  test_object->set_hex(Hex(0, 0));

  // A sea transporter should have access to at most 2 of the 3 areas without
  // bridges
  // Without bridges, it should just be the areas the input direction touches.
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    auto expected = test_object->get_areas(d);
    EXPECT_EQ(expected.size(), actual.size());
    EXPECT_EQ(expected, actual);
  }
  // Land transporters should be restricted to their current area
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    auto expected = test_object->get_area(b);
    EXPECT_EQ(1, actual.size());
    if (actual.size() > 0)
    {
      EXPECT_EQ(expected, actual.at(0));
    }
  }

  ASSERT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::north_west));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    switch (d)
    {
    case Direction::north_west:
    case Direction::south_east:
    case Direction::south_west:
    case Direction::west:
    {
      // Sea transporters on the NW->SW river should have restricted access.
      // A transporter on the W or SE sides should coincidentally have the same
      // access.
      auto expected = test_object->get_areas(Direction::north_west);
      EXPECT_EQ(2, actual.size());
      EXPECT_EQ(expected, actual);
      break;
    }
    case Direction::north_east:
    case Direction::east:
    {
      // Sea transporters on the NE->E river should have access to
      // everything.
      auto expected = test_object->get_areas();
      EXPECT_EQ(3, actual.size());
      EXPECT_EQ(expected, actual);
      break;
    }
    }
  }
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    if ((b <= Border::NE_left) || (b >= Border::E_right))
    {
      auto expected = test_object->get_areas(Direction::north_west);
      EXPECT_EQ(2, actual.size());
      EXPECT_EQ(expected, actual);
    }
    else
    {
      auto expected = test_object->get_area(Border::E_left);
      EXPECT_EQ(1, actual.size());
      if (actual.size() > 0)
      {
        EXPECT_EQ(expected, actual.at(0));
      }
    }
  }

  // Adding another bridge should give full access for all transporters
  ASSERT_EQ(common::ERR_NONE, test_object->build_bridge(Direction::east));
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    auto actual = test_object->get_accessible_areas(d);
    auto expected = test_object->get_areas();
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(expected, actual);
  }
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    Border b = static_cast<Border>(i);
    auto actual = test_object->get_accessible_areas(b);
    auto expected = test_object->get_areas();
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(expected, actual);
  }
}

TEST(tile_test, rotate_test)
{
  // Rotating a tile should be clockwise. If the input value is negative, then
  // rotation is counter-clockwise.
  // This should rotate all of the Tile's river points accordingly.
  std::set<Direction> rp;
  rp.insert(Direction::north_east);
  rp.insert(Direction::south_west);
  rp.insert(Direction::west);
  Hex hex(0, 0);
  std::shared_ptr<Tile> test = std::make_shared<Tile>(hex, rp);
  std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  std::vector<std::shared_ptr<River>> rivers = copy_rivers(test->get_rivers());
  std::vector<std::shared_ptr<Area>> areas = copy_areas(test->get_areas());
  ASSERT_EQ(common::ERR_NONE,
            test->add_neighbor(neighbor, Direction::north_west));
  ASSERT_EQ(common::ERR_NONE,
            neighbor->add_neighbor(test, Direction::south_east));

  // If any of the areas can't rotate, the tile should not rotate
  EXPECT_EQ(common::ERR_NONE, test->build_road(Border::NW_left));
  ASSERT_EQ(common::ERR_FAIL, test->rotate(1));
  check_rivers(rivers, test->get_rivers(), true);
  check_areas(areas, test->get_areas(), true);
  // Same goes for if any of the rivers can't rotate
  test = std::make_shared<Tile>(hex, rp);
  rivers = copy_rivers(test->get_rivers());
  areas = copy_areas(test->get_areas());
  EXPECT_EQ(common::ERR_NONE, test->build_bridge(Direction::north_east));
  EXPECT_EQ(common::ERR_FAIL, test->rotate(1));
  check_rivers(rivers, test->get_rivers(), true);
  check_areas(areas, test->get_areas(), true);
  // Additionally, the tile won't rotate if it has any neighbors
  test = std::make_shared<Tile>(hex, rp);
  neighbor = std::make_shared<Tile>();
  rivers = copy_rivers(test->get_rivers());
  areas = copy_areas(test->get_areas());
  EXPECT_EQ(common::ERR_NONE, test->add_neighbor(neighbor, Direction::east));
  EXPECT_EQ(common::ERR_FAIL, test->rotate(1));
  check_rivers(rivers, test->get_rivers(), true);
  check_areas(areas, test->get_areas(), true);

  // Reset tile for the rest of the tests
  test = std::make_shared<Tile>(rp);
  rivers = copy_rivers(test->get_rivers());
  areas = copy_areas(test->get_areas());

  // When rotating 0, nothing should actually move.
  EXPECT_EQ(common::ERR_NONE, test->rotate(0));
  check_rivers(rivers, test->get_rivers(), true);
  check_areas(areas, test->get_areas(), true);

  // When rotating 1, everything should move clockwise 1 step
  EXPECT_EQ(common::ERR_NONE, test->rotate(1));
  check_rivers(rivers, test->get_rivers(), false);
  for (auto r : rivers)
  {
    EXPECT_EQ(common::ERR_NONE, r->rotate(1));
  }
  check_rivers(rivers, test->get_rivers(), true);

  check_areas(areas, test->get_areas(), false);
  for (auto a : areas)
  {
    EXPECT_EQ(common::ERR_NONE, a->rotate(1));
  }
  check_areas(areas, test->get_areas(), true);

  // When rotating -1, everything should move counter-clockwise 1 step
  EXPECT_EQ(common::ERR_NONE, test->rotate(-1));
  check_rivers(rivers, test->get_rivers(), false);
  for (auto r : rivers)
  {
    EXPECT_EQ(common::ERR_NONE, r->rotate(-1));
  }
  check_rivers(rivers, test->get_rivers(), true);

  check_areas(areas, test->get_areas(), false);
  for (auto a : areas)
  {
    EXPECT_EQ(common::ERR_NONE, a->rotate(-1));
  }
  check_areas(areas, test->get_areas(), true);

  // When rotating 8, everything should effectively move 2 steps clockwise
  EXPECT_EQ(common::ERR_NONE, test->rotate(8));
  check_rivers(rivers, test->get_rivers(), false);
  for (auto r : rivers)
  {
    EXPECT_EQ(common::ERR_NONE, r->rotate(8));
  }
  check_rivers(rivers, test->get_rivers(), true);

  check_areas(areas, test->get_areas(), false);
  for (auto a : areas)
  {
    EXPECT_EQ(common::ERR_NONE, a->rotate(8));
  }
  check_areas(areas, test->get_areas(), true);

  // When rotating -15, everything should effectively move 3 steps
  // counter-clockwise
  EXPECT_EQ(common::ERR_NONE, test->rotate(-15));
  check_rivers(rivers, test->get_rivers(), false);
  for (auto r : rivers)
  {
    EXPECT_EQ(common::ERR_NONE, r->rotate(-15));
  }
  check_rivers(rivers, test->get_rivers(), true);

  check_areas(areas, test->get_areas(), false);
  for (auto a : areas)
  {
    EXPECT_EQ(common::ERR_NONE, a->rotate(-15));
  }
  check_areas(areas, test->get_areas(), true);
}