#include <map>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <players/Player.h>
#include <tiles/Tile.h>
#include <tiles/Tile_map.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>

using namespace tile;

TEST(tile_map_test, create_map_test)
{
  // Not much to creating a blank map; assert the initial parameters are
  // as expected.
  Tile_map test_object = Tile_map();
  EXPECT_FALSE(test_object.is_locked());
  EXPECT_TRUE(test_object.empty());
}

TEST(tile_map_test, get_tile_test)
{
  Tile_map test_object = Tile_map();
  Hex desert_hex(0, 0);
  Hex sea_hex(1, 0);
  Hex forest_hex(2, 0);
  std::shared_ptr<Tile> desert_tile = std::make_shared<Tile>(Terrain::desert);
  std::shared_ptr<Tile> sea_tile = std::make_shared<Tile>(Terrain::sea);
  std::shared_ptr<Tile> forest_tile = std::make_shared<Tile>(Terrain::forest);
  ASSERT_EQ(common::ERR_NONE, test_object.insert(0, 0, desert_tile));
  ASSERT_EQ(common::ERR_NONE, test_object.insert(1, 0, sea_tile));
  ASSERT_EQ(common::ERR_NONE, test_object.insert(2, 0, forest_tile));

  // We should be able to look up a tile based on hex coordinates.
  std::shared_ptr<Tile> actual_tile;
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(desert_hex, actual_tile));
  EXPECT_EQ(desert_tile, actual_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(sea_hex, actual_tile));
  EXPECT_EQ(sea_tile, actual_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(forest_hex, actual_tile));
  EXPECT_EQ(forest_tile, actual_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(0, 0, actual_tile));
  EXPECT_EQ(desert_tile, actual_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(1, 0, actual_tile));
  EXPECT_EQ(sea_tile, actual_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(2, 0, actual_tile));
  EXPECT_EQ(forest_tile, actual_tile);

  // Retrieving a tile at a location we haven't added at should fail.
  Hex bad_hex(0, 1);
  actual_tile.reset();
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(0, 1, actual_tile));
  EXPECT_EQ(nullptr, actual_tile);
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(bad_hex, actual_tile));
  EXPECT_EQ(nullptr, actual_tile);
}

TEST(tile_map_test, add_tile_test)
{
  // Don't allow inserting a tile at coordinates that are already filled.
  Tile_map test_object = Tile_map();
  EXPECT_EQ(common::ERR_NONE,
            test_object.insert(0, 0, std::make_shared<Tile>()));
  EXPECT_EQ(common::ERR_FAIL,
            test_object.insert(0, 0, std::make_shared<Tile>()));
}
