#include <memory>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <buildings/producers/Mine.h>
#include <buildings/producers/Mint.h>
#include <buildings/producers/Woodcutter.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <utils/id_utils.h>

using namespace tile;

TEST(area_test, create_area_test)
{
  std::set<Border> borders;
  borders.insert(Border::NW_right);
  borders.insert(Border::NE_left);
  borders.insert(Border::NE_right);
  borders.insert(Border::E_left);

  Area a = Area(borders);

  EXPECT_EQ(borders, a);

  // Copying should transfer everything
  Area b = Area(a);
  ASSERT_EQ(a, b);
}

TEST(area_test, build_road_test)
{
  std::set<Border> borders;
  borders.insert(Border::NW_right);
  borders.insert(Border::NE_left);
  borders.insert(Border::NE_right);
  borders.insert(Border::E_left);

  // An area defaults to starting without any roads.
  Area a = Area(borders);
  ASSERT_EQ(0, a.get_roads().size());

  // An area will not build a road on invalid border input
  ASSERT_EQ(common::ERR_INVALID, a.build(static_cast<Border>(-1)));
  ASSERT_EQ(0, a.get_roads().size());
  ASSERT_EQ(common::ERR_INVALID, a.build(static_cast<Border>(15)));
  ASSERT_EQ(0, a.get_roads().size());

  // An area will not allow building a road on a border it does not have.
  ASSERT_EQ(common::ERR_FAIL, a.build(Border::NW_left));
  ASSERT_EQ(0, a.get_roads().size());

  // If the area uses the border, it will allow building a road on it.
  ASSERT_EQ(common::ERR_NONE, a.build(Border::NW_right));
  ASSERT_EQ(1, a.get_roads().size());
  ASSERT_TRUE(a.has_road(Border::NW_right));

  // Roads can only be built once per border.
  ASSERT_EQ(common::ERR_FAIL, a.build(Border::NW_right));
  ASSERT_EQ(1, a.get_roads().size());
  ASSERT_TRUE(a.has_road(Border::NW_right));

  // Adding another road should be fine.
  ASSERT_EQ(common::ERR_NONE, a.build(Border::NE_left));
  ASSERT_EQ(2, a.get_roads().size());
  ASSERT_TRUE(a.has_road(Border::NW_right));
  ASSERT_TRUE(a.has_road(Border::NE_left));

  // If an area contains both borders for a direction, only one road can be
  // built for either border.
  ASSERT_EQ(common::ERR_FAIL, a.build(Border::NE_right));
  ASSERT_EQ(2, a.get_roads().size());
  ASSERT_TRUE(a.has_road(Border::NW_right));
  ASSERT_TRUE(a.has_road(Border::NE_left));
}

TEST(area_test, build_building_test)
{
  Area *test_object = new Area();

  // An area defaults to not having a building
  ASSERT_EQ(nullptr, test_object->get_building());

  // Constructing a building requires resources. Without those, the building
  // should not be added.
  ASSERT_EQ(common::ERR_FAIL, test_object->build<building::Woodcutter>());
  ASSERT_EQ(nullptr, test_object->get_building());

  // Some buildings can only be built on specific terrains. A woodcutter can
  // only be built on a forest tile.
  Tile tile = Tile(Terrain::plains);
  // The tile only has one area, so first border should retrieve it.
  test_object = tile.get_area(Border::E_left).get();
  // Add resources so we only test that the terrain matters
  test_object->add_resource(
      new portable::Resource(portable::Resource::Type::boards));
  ASSERT_EQ(common::ERR_FAIL, test_object->build<building::Woodcutter>());
  ASSERT_EQ(nullptr, test_object->get_building());

  tile = Tile(Terrain::forest);
  test_object = tile.get_area(Border::E_left).get();
  ASSERT_EQ(common::ERR_FAIL, test_object->build<building::Woodcutter>());
  ASSERT_EQ(nullptr, test_object->get_building());

  // An area will add a building when valid
  test_object->add_resource(
      new portable::Resource(portable::Resource::Type::boards));
  ASSERT_EQ(common::ERR_NONE, test_object->build<building::Woodcutter>());
  ASSERT_NE(nullptr, test_object->get_building());
  ASSERT_EQ(building::Building::Type::woodcutter,
            test_object->get_building()->get_type());

  // Areas can only have one building on it
  ASSERT_EQ(common::ERR_FAIL, test_object->build<building::Mint>());
  ASSERT_EQ(building::Building::Type::woodcutter,
            test_object->get_building()->get_type());
}

TEST(area_test, rotate_area_test)
{
  // Rotating an area should be clockwise. If the input value is negative, the
  // the rotation is counter-clockwise. This should rotate all of the borders
  // and roads accordingly.
  std::set<Border> borders;
  borders.insert(Border::NW_left);
  borders.insert(Border::NW_right);
  borders.insert(Border::NE_left);

  Area test_object(borders);
  // Should fail trying to rotate after the area has something built
  EXPECT_EQ(common::ERR_NONE, test_object.build(Border::NW_left));
  ASSERT_EQ(common::ERR_FAIL, test_object.rotate(0));
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  for (auto b : borders)
  {
    EXPECT_TRUE(test_object.has_border(b));
  }

  // Reset for the rest of the tests.
  test_object = Area(borders);

  // When rotating 0, nothing should move
  test_object.rotate(0);
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  for (auto b : borders)
  {
    EXPECT_TRUE(test_object.has_border(b));
  }

  // When rotating 1, everything should move clockwise 1 step
  test_object.rotate(1);
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  EXPECT_FALSE(test_object.has_border(Border::NW_left));
  EXPECT_TRUE(test_object.has_border(Border::NE_left));
  EXPECT_FALSE(test_object.has_border(Border::NW_right));
  EXPECT_TRUE(test_object.has_border(Border::NE_right));
  EXPECT_TRUE(test_object.has_border(Border::E_left));

  // When rotating -1, everything should move counter-clockwise 1 step
  // Subsequent rotations should be additive.
  test_object.rotate(-1);
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  for (auto b : borders)
  {
    EXPECT_TRUE(test_object.has_border(b));
  }

  // When rotating 8, everything should move effectively clockwise 2 steps
  test_object.rotate(8);
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  EXPECT_FALSE(test_object.has_border(Border::NW_left));
  EXPECT_TRUE(test_object.has_border(Border::E_left));
  EXPECT_FALSE(test_object.has_border(Border::NW_right));
  EXPECT_TRUE(test_object.has_border(Border::E_right));
  EXPECT_FALSE(test_object.has_border(Border::NE_left));
  EXPECT_TRUE(test_object.has_border(Border::SE_left));

  // When rotating -14, everything should move effectively counter-clockwise 2
  // steps
  test_object.rotate(-14);
  EXPECT_EQ(borders.size(), test_object.get_borders().size());
  for (auto b : borders)
  {
    EXPECT_TRUE(test_object.has_border(b));
  }
}