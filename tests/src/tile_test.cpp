#include <gtest/gtest.h>
#include <set>

#include <common/Errors.h>
#include <tiles/Tile.h>

using namespace tile;

TEST(tile_test, create_tile_test)
{
  // When creating tiles, a unique ID should be added for each tile.
  // This is implemented as a count for each tile, starting at 1 and
  // incrementing by 1 for each additional tile created.

  uint8_t count = 1;
  EXPECT_EQ(count, Tile::unique_tile_id);

  // Check that tile id increments with each new tile
  std::unique_ptr<Tile> t = std::make_unique<Tile>();
  EXPECT_EQ(count, t->get_id());
  count++;
  EXPECT_EQ(count, Tile::unique_tile_id);

  t = std::make_unique<Tile>();
  EXPECT_EQ(count, t->get_id());
  count++;
  EXPECT_EQ(count, Tile::unique_tile_id);
}

TEST(tile_test, add_neighbor_test)
{
  // Tile placement is limited by river points for each tile, and whether
  // either tile already has a neighbor in the given direction.
  // Rivers should continue from tile to tile; this means if a tile has a river
  // point on the side that we're trying to add a neighbor, the neighbor must
  // also have a river point on its corresponding side.

  std::set<Direction> river_points;
  river_points.insert(Direction::north_east);

  std::shared_ptr<Tile> test = std::make_shared<Tile>();
  std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  std::shared_ptr<Tile> river_neighbor = std::make_shared<Tile>(river_points);

  // Adding a neighbor without any rivers should be fine.
  EXPECT_EQ(common::ERR_NONE, test->add_neighbor(neighbor, Direction::east));
  std::shared_ptr<Tile> added = test->get_neighbor(Direction::east);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added->get_id(), neighbor->get_id());

  // Trying to add another neighbor in the same direction should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test->add_neighbor(river_neighbor, Direction::east));
  added = test->get_neighbor(Direction::east);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added->get_id(), neighbor->get_id());

  // Remove from tile's neighbors.
  EXPECT_EQ(common::ERR_NONE, test->remove_neighbor(Direction::east));
  EXPECT_EQ(nullptr, test->get_neighbor(Direction::east));

  // Trying to add a neighbor with a river where we don't match should fail.
  EXPECT_EQ(common::ERR_FAIL,
            test->add_neighbor(river_neighbor, Direction::south_west));
  added = test->get_neighbor(Direction::south_west);
  EXPECT_EQ(nullptr, added);

  // Adding a neighbor with a shared river point on the boundary should be fine.
  // Here we reset the test tile to have a river point on the south_west border.
  std::set<Direction> test_river;
  test_river.insert(Direction::south_west);
  test.reset();
  test = std::make_shared<Tile>(test_river);
  EXPECT_EQ(common::ERR_NONE,
            test->add_neighbor(river_neighbor, Direction::south_west));
  added = test->get_neighbor(Direction::south_west);
  EXPECT_NE(nullptr, added);
  EXPECT_EQ(added->get_id(), river_neighbor->get_id());

  // Remove from tile's neighbors.
  EXPECT_EQ(common::ERR_NONE, test->remove_neighbor(Direction::south_west));
  EXPECT_EQ(nullptr, test->get_neighbor(Direction::south_west));

  // Adding a neighbor that doesn't have a river where we do should fail
  EXPECT_EQ(common::ERR_FAIL,
            test->add_neighbor(neighbor, Direction::south_west));
  added = test->get_neighbor(Direction::south_west);
  EXPECT_EQ(nullptr, added);

  // Sea tiles are the exception to this; they can always be placed next to any
  // other tile, provided both tiles can agree on a side to share.
  // TODO: add testing for Sea tiles
}