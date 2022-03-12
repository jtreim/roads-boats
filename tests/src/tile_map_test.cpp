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
  Tile_map test_object = Tile_map();
  std::shared_ptr<Tile> base_tile = std::make_shared<Tile>();
  std::shared_ptr<Tile> plains_tile = std::make_shared<Tile>(Terrain::plains);
  std::shared_ptr<Tile> rock_tile = std::make_shared<Tile>(Terrain::rock);
  std::shared_ptr<Tile> sea_tile = std::make_shared<Tile>(Terrain::sea);
  std::shared_ptr<Tile> test_tile;
  // Don't allow inserting into map if it is locked
  test_object.set_lock(true);
  EXPECT_EQ(common::ERR_FAIL, test_object.insert(0, 0, base_tile));
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(0, 0, test_tile));
  EXPECT_EQ(nullptr, test_tile);

  test_object.set_lock(false);
  // A basic insert into an empty map should be fine
  EXPECT_EQ(common::ERR_NONE, test_object.insert(0, 0, base_tile));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(0, 0, test_tile));
  EXPECT_NE(nullptr, test_tile);

  // Don't allow inserting a tile at coordinates that are already filled.
  EXPECT_EQ(common::ERR_FAIL, test_object.insert(0, 0, rock_tile));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(0, 0, test_tile));
  EXPECT_NE(nullptr, test_tile);
  EXPECT_EQ(base_tile, test_tile);

  // Adding a tile at a valid, adjacent location should be fine.
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(1, 0, test_tile));
  EXPECT_EQ(nullptr, test_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.insert(1, 0, sea_tile));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(1, 0, test_tile));
  EXPECT_NE(nullptr, test_tile);
  EXPECT_EQ(sea_tile, test_tile);
  // When successfully inserting, the tile should have its hex coordinates
  // updated.
  EXPECT_EQ(Hex(1, 0), test_tile->get_hex());
  // Tiles should be able to find their neighbors.
  EXPECT_EQ(sea_tile, base_tile->get_neighbor(Direction::east));
  EXPECT_EQ(base_tile, sea_tile->get_neighbor(Direction::west));
  // Tile neighbors should only update the appropriate directions
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    if (d != Direction::west)
    {
      EXPECT_EQ(nullptr, sea_tile->get_neighbor(d));
    }
    if (d != Direction::east)
    {
      EXPECT_EQ(nullptr, base_tile->get_neighbor(d));
    }
  }

  // Adding a tile at valid, non-adjacent location should be fine.
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(2, -2, test_tile));
  EXPECT_EQ(nullptr, test_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.insert(2, -2, plains_tile));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(2, -2, test_tile));
  ASSERT_NE(nullptr, test_tile);
  EXPECT_EQ(Hex(2, -2), test_tile->get_hex());

  // Adding a river tile means all potential neighbors have to match with
  // rivers/non-river sides.
  std::set<Direction> rp;
  rp.insert(Direction::east);
  rp.insert(Direction::south_west);
  std::shared_ptr<Tile> river_tile =
      std::make_shared<Tile>(rp, Terrain::forest);
  // This should fail because the tile that would be to the east of this one
  // does not have a river on the adjoining side.
  EXPECT_EQ(common::ERR_FAIL, test_object.insert(1, -2, river_tile));
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(1, -2, test_tile));
  EXPECT_EQ(nullptr, test_tile);
  // This should pass because the tiles that would be adjacent to this one does
  // match river/non-river sides.
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(2, -1, test_tile));
  EXPECT_EQ(nullptr, test_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.insert(2, -1, river_tile));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(2, -1, test_tile));
  EXPECT_EQ(river_tile, test_tile);
  EXPECT_EQ(Hex(2, -1), test_tile->get_hex());
  EXPECT_EQ(sea_tile, test_tile->get_neighbor(Direction::south_west));
  EXPECT_EQ(test_tile, sea_tile->get_neighbor(Direction::north_east));
  EXPECT_EQ(plains_tile, test_tile->get_neighbor(Direction::north_west));
  EXPECT_EQ(test_tile, plains_tile->get_neighbor(Direction::south_east));
}

TEST(tile_map_test, remove_tile_test)
{
  Tile_map test_object = Tile_map();
  std::shared_ptr<Tile> base_tile = std::make_shared<Tile>();
  std::shared_ptr<Tile> plains_tile = std::make_shared<Tile>(Terrain::plains);
  std::shared_ptr<Tile> rock_tile = std::make_shared<Tile>(Terrain::rock);
  std::shared_ptr<Tile> test_tile;
  ASSERT_EQ(common::ERR_NONE, test_object.insert(0, 0, base_tile));
  ASSERT_EQ(common::ERR_NONE, test_object.insert(1, 0, plains_tile));
  ASSERT_EQ(common::ERR_NONE, test_object.insert(2, 0, rock_tile));

  // Don't allow removing if the map is locked.
  test_object.set_lock(true);
  EXPECT_EQ(common::ERR_FAIL, test_object.remove(0, 0));
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(0, 0, test_tile));
  EXPECT_EQ(base_tile, test_tile);

  test_object.set_lock(false);
  // A remove of a non-existent tile should fail.
  EXPECT_EQ(common::ERR_FAIL, test_object.remove(1, 1));
  EXPECT_EQ(3, test_object.size());
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(0, 0, test_tile));
  EXPECT_EQ(base_tile, test_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(1, 0, test_tile));
  EXPECT_EQ(plains_tile, test_tile);
  EXPECT_EQ(common::ERR_NONE, test_object.get_tile(2, 0, test_tile));
  EXPECT_EQ(rock_tile, test_tile);

  // A valid remove should be fine.
  EXPECT_EQ(common::ERR_NONE, test_object.remove(1, 0));
  EXPECT_EQ(2, test_object.size());
  EXPECT_EQ(common::ERR_FAIL, test_object.get_tile(1, 0, test_tile));
  EXPECT_EQ(nullptr, test_tile);
  // The neighbors should no longer track the removed tile
  EXPECT_EQ(nullptr, base_tile->get_neighbor(Direction::east));
  EXPECT_EQ(nullptr, rock_tile->get_neighbor(Direction::west));
}

TEST(tile_map_test, valid_map_test)
{
  Tile_map test_object = Tile_map();
  std::shared_ptr<Tile> base_tile = std::make_shared<Tile>();
  std::shared_ptr<Tile> sea_tile = std::make_shared<Tile>(Terrain::sea);
  std::set<Direction> rp;
  rp.insert(Direction::east);
  std::shared_ptr<Tile> river_tile =
      std::make_shared<Tile>(rp, Terrain::mountain);

  // An empty map should not be valid
  EXPECT_FALSE(test_object.is_valid());

  // A map with a single tile should be valid (though real lame)
  ASSERT_EQ(common::ERR_NONE, test_object.insert(0, 0, base_tile));
  EXPECT_TRUE(test_object.is_valid());

  // A map should not be valid if a river does not run to a sea tile or another
  // tile's river
  ASSERT_EQ(common::ERR_NONE, test_object.insert(1, 0, river_tile));
  EXPECT_FALSE(test_object.is_valid());

  // River runs into a sea tile; map should be valid again
  ASSERT_EQ(common::ERR_NONE, test_object.insert(2, 0, sea_tile));
  EXPECT_TRUE(test_object.is_valid());
}