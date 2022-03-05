#include <memory>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/Transporter.h>
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

  // When creating areas, an unique ID should be added for each area.
  std::set<uuids::uuid> ids;
  Area a = Area(borders);

  // New ID should not be empty, and should follow the expected uuid format.
  uuids::uuid id = a.get_id();
  ASSERT_TRUE(!id.is_nil());
  ASSERT_EQ(16, id.as_bytes().size());
  ASSERT_EQ(uuids::uuid_version::random_number_based, id.version());
  ASSERT_EQ(uuids::uuid_variant::rfc, id.variant());

  ids.insert(a.get_id());

  // IDs should not be duplicated
  a = Area(borders);
  id = a.get_id();
  ASSERT_FALSE(ids.contains(id));
  ids.insert(id);

  // Creating with a different input params shouldn't matter
  borders.insert(Border::E_right);
  borders.insert(Border::SE_left);
  a = Area(borders);
  id = a.get_id();
  ASSERT_FALSE(ids.contains(id));
  ids.insert(id);

  // Copying should transfer everything, including the ID
  Area b = Area(a);
  uuids::uuid copied_id = b.get_id();
  ASSERT_TRUE(ids.contains(copied_id));
  ASSERT_EQ(id, copied_id);
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
  Area test_object = Area();

  // An area defaults to not having a building
  ASSERT_EQ(nullptr, test_object.get_building());

  std::shared_ptr<building::Building> bldg;

  // An area will fail when "building" a nullptr
  ASSERT_EQ(common::ERR_INVALID, test_object.build(bldg));
  ASSERT_EQ(nullptr, test_object.get_building());

  // An area will add a building when valid
  bldg = std::make_shared<building::Building>(
      building::Building::Type::woodcutter);
  ASSERT_EQ(common::ERR_NONE, test_object.build(bldg));
  ASSERT_EQ(bldg, test_object.get_building());

  // Areas can only have one building on it
  std::shared_ptr<building::Building> other =
      std::make_shared<building::Building>(building::Building::mint);
  ASSERT_EQ(common::ERR_FAIL, test_object.build(other));
  ASSERT_EQ(bldg, test_object.get_building());
}

TEST(area_test, area_addition_test)
{
  std::set<Border> a_borders;
  a_borders.insert(Border::NW_right);
  a_borders.insert(Border::NE_left);
  a_borders.insert(Border::NE_right);
  a_borders.insert(Border::E_left);
  Area a = Area(a_borders);
  a.build(Border::NW_right);
  std::shared_ptr<building::Building> bldg =
      std::make_shared<building::Building>(building::Building::mine);
  a.build(bldg);
  a.add_resource(portable::Resource::trunks, 10);

  std::set<Border> b_borders;
  b_borders.insert(Border::E_right);
  b_borders.insert(Border::SE_left);
  b_borders.insert(Border::SE_right);
  b_borders.insert(Border::SW_left);
  Area b = Area(b_borders);
  b.build(Border::E_right);
  b.build(Border::SE_left);
  b.add_resource(portable::Resource::trunks, 2);
  b.add_resource(portable::Resource::goose, 3);

  // When merging two areas that can be combined, the result should contain all
  // borders, roads, resources, transporters, and whether the merged area
  // includes a building.
  Area c = a + b;

  ASSERT_EQ(8, c.get_borders().size());
  for (auto border : a.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (auto road : a.get_roads())
  {
    ASSERT_TRUE(c.has_road(road));
  }
  for (auto border : b.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (auto road : b.get_roads())
  {
    ASSERT_TRUE(c.has_road(road));
  }
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    uint16_t amount = a.get_resource_amount(r) + b.get_resource_amount(r);
    ASSERT_EQ(amount, c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  // The += operator should work too
  c = Area();
  c += a;
  c += b;
  ASSERT_EQ(8, c.get_borders().size());
  for (auto border : a.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (auto road : a.get_roads())
  {
    ASSERT_TRUE(c.has_road(road));
  }
  for (auto border : b.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (auto road : b.get_roads())
  {
    ASSERT_TRUE(c.has_road(road));
  }
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    uint16_t amount = a.get_resource_amount(r) + b.get_resource_amount(r);
    ASSERT_EQ(amount, c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  // These operators should also allow adding borders/resources directly.
  // This should merge in b's borders, but not anything else.
  c = a + b.get_borders();
  ASSERT_EQ(8, c.get_borders().size());
  for (auto border : a.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  ASSERT_EQ(a.get_roads(), c.get_roads());
  for (auto border : b.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    ASSERT_EQ(a.get_resource_amount(r), c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  c = Area(a);
  c += b.get_borders();
  ASSERT_EQ(8, c.get_borders().size());
  for (auto border : a.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  ASSERT_EQ(a.get_roads(), c.get_roads());
  for (auto border : b.get_borders())
  {
    ASSERT_TRUE(c.has_border(border));
  }
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    ASSERT_EQ(a.get_resource_amount(r), c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  // Adding a list of resources should only merge in those to the area.
  c = a + b.get_all_resources();
  ASSERT_EQ(a.get_borders(), c.get_borders());
  ASSERT_EQ(a.get_roads(), c.get_roads());
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    ASSERT_EQ(a.get_resource_amount(r) + b.get_resource_amount(r),
              c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  c = a;
  c += b.get_all_resources();
  ASSERT_EQ(a.get_borders(), c.get_borders());
  ASSERT_EQ(a.get_roads(), c.get_roads());
  for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(r);
    ASSERT_EQ(a.get_resource_amount(r) + b.get_resource_amount(r),
              c.get_resource_amount(r));
  }
  ASSERT_EQ(bldg, c.get_building());

  // the merge function should act like the += operator
  c = a;
  c += b;
  Area d = a;
  ASSERT_EQ(common::ERR_NONE, d.merge(b));
  ASSERT_EQ(c, d);

  // if the both areas that are being combined have buildings, don't merge them
  // Report error for merge
  b.build(bldg);
  d = a + b;
  ASSERT_EQ(a, d);

  d = Area(a);
  d += b;
  ASSERT_EQ(a, d);

  d = Area(a);
  ASSERT_EQ(common::ERR_FAIL, d.merge(b));
  ASSERT_EQ(a, d);
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