#include <iostream>
#include <set>

#include <gtest/gtest.h>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex_point.h>
#include <tiles/components/River.h>

using namespace tile;

TEST(tile_test, create_tile_test)
{
  // // When creating tiles, an unique ID should be added for each tile.
  // // This is implemented as a UUID for each tile.
  // // This should not matter what type of Tile, or how many river points it is
  // // initialized with.
  // std::set<uuids::uuid> ids;
  // Desert desert = Desert();

  // // New ID should not be empty, and should follow the expected uuid format.
  // uuids::uuid id = desert.get_id();
  // ASSERT_TRUE(!id.is_nil());
  // ASSERT_EQ(16, id.as_bytes().size());
  // ASSERT_EQ(uuids::uuid_version::random_number_based, id.version());
  // ASSERT_EQ(uuids::uuid_variant::rfc, id.variant());

  // ids.insert(desert.get_id());

  // Forest forest = Forest();
  // ASSERT_EQ(ids.end(), ids.find(forest.get_id()));
  // ids.insert(forest.get_id());

  // Mountain mountain = Mountain();
  // ASSERT_EQ(ids.end(), ids.find(mountain.get_id()));
  // ids.insert(mountain.get_id());

  // Plains plains = Plains();
  // ASSERT_EQ(ids.end(), ids.find(plains.get_id()));
  // ids.insert(plains.get_id());

  // Rock rock = Rock();
  // ASSERT_EQ(ids.end(), ids.find(rock.get_id()));
  // ids.insert(rock.get_id());

  // Sea sea = Sea();
  // ASSERT_EQ(ids.end(), ids.find(sea.get_id()));
  // ids.insert(sea.get_id());

  // Tile tile = Tile();
  // ASSERT_EQ(ids.end(), ids.find(tile.get_id()));
  // ids.insert(tile.get_id());
}

TEST(tile_test, add_neighbor_test)
{
  // // Tile placement is limited by river points for each tile, and whether
  // // either tile already has a neighbor in the given direction.
  // // Rivers should continue from tile to tile; this means if a tile has a
  // river
  // // point on the side that we're trying to add a neighbor, the neighbor must
  // // also have a river point on its corresponding side.

  // std::set<Direction> river_points;
  // river_points.insert(Direction::north_east);

  // std::shared_ptr<Tile> test = std::make_shared<Tile>();
  // std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  // std::shared_ptr<Tile> river_neighbor =
  // std::make_shared<Tile>(river_points);

  // // Adding a nonexistant neighbor or direction should fail.
  // EXPECT_EQ(common::ERR_INVALID,
  //           test->add_neighbor(neighbor, static_cast<Direction>(-1)));
  // std::shared_ptr<Tile> added = test->get_neighbor(Direction::east);
  // EXPECT_EQ(nullptr, added);
  // EXPECT_EQ(common::ERR_INVALID,
  //           test->add_neighbor(neighbor, static_cast<Direction>(6)));
  // added = test->get_neighbor(Direction::east);
  // EXPECT_EQ(nullptr, added);
  // EXPECT_EQ(common::ERR_INVALID, test->add_neighbor(nullptr,
  // Direction::east)); added = test->get_neighbor(Direction::east);
  // EXPECT_EQ(nullptr, added);

  // // Adding ourselves as a neighbor should fail.
  // EXPECT_EQ(common::ERR_FAIL, test->add_neighbor(test, Direction::east));
  // added = test->get_neighbor(Direction::east);
  // EXPECT_EQ(nullptr, added);

  // // Adding a neighbor without any rivers should be fine.
  // EXPECT_EQ(common::ERR_NONE, test->add_neighbor(neighbor, Direction::east));
  // added = test->get_neighbor(Direction::east);
  // EXPECT_NE(nullptr, added);
  // EXPECT_EQ(added->get_id(), neighbor->get_id());

  // // Adding the same neighbor again should fail.
  // EXPECT_EQ(common::ERR_FAIL,
  //           test->add_neighbor(neighbor, Direction::north_east));
  // added = test->get_neighbor(Direction::north_east);
  // EXPECT_EQ(nullptr, added);

  // // Trying to add another neighbor in the same direction should fail.
  // EXPECT_EQ(common::ERR_FAIL,
  //           test->add_neighbor(river_neighbor, Direction::east));
  // added = test->get_neighbor(Direction::east);
  // EXPECT_NE(nullptr, added);
  // EXPECT_EQ(added->get_id(), neighbor->get_id());

  // // Remove from tile's neighbors.
  // EXPECT_EQ(common::ERR_NONE, test->remove_neighbor(Direction::east));
  // EXPECT_EQ(nullptr, test->get_neighbor(Direction::east));

  // // Trying to add a neighbor with a river where we don't match should fail.
  // EXPECT_EQ(common::ERR_FAIL,
  //           test->add_neighbor(river_neighbor, Direction::south_west));
  // added = test->get_neighbor(Direction::south_west);
  // EXPECT_EQ(nullptr, added);

  // // Here we reset the test tile to have a river point on the south_west
  // border. std::set<Direction> test_river;
  // test_river.insert(Direction::south_west);
  // test.reset();
  // test = std::make_shared<Tile>(test_river);
  // // Adding a neighbor with a shared river point on the boundary should be
  // fine. EXPECT_EQ(common::ERR_NONE,
  //           test->add_neighbor(river_neighbor, Direction::south_west));
  // added = test->get_neighbor(Direction::south_west);
  // EXPECT_NE(nullptr, added);
  // EXPECT_EQ(added->get_id(), river_neighbor->get_id());

  // // Remove from tile's neighbors.
  // EXPECT_EQ(common::ERR_NONE, test->remove_neighbor(Direction::south_west));
  // EXPECT_EQ(nullptr, test->get_neighbor(Direction::south_west));

  // // Adding a neighbor that doesn't have a river where we do should fail
  // EXPECT_EQ(common::ERR_FAIL,
  //           test->add_neighbor(neighbor, Direction::south_west));
  // added = test->get_neighbor(Direction::south_west);
  // EXPECT_EQ(nullptr, added);

  // // Sea tiles are the exception to this; they can ignore river/land
  // // border restrictions.
  // std::shared_ptr<Sea> sea_neighbor = std::make_shared<Sea>();
  // EXPECT_EQ(common::ERR_NONE,
  //           test->add_neighbor(sea_neighbor, Direction::east));
  // added = test->get_neighbor(Direction::east);
  // EXPECT_NE(nullptr, added);
  // EXPECT_EQ(added->get_id(), sea_neighbor->get_id());
  // // Remove from tile's neighbors.
  // EXPECT_EQ(common::ERR_NONE, test->remove_neighbor(Direction::east));
  // EXPECT_EQ(nullptr, test->get_neighbor(Direction::east));

  // // The reverse should also be true; any tile can be placed next to a sea
  // tile,
  // // provided all other checks pass.
  // EXPECT_EQ(common::ERR_NONE,
  //           sea_neighbor->add_neighbor(test, Direction::east));
  // added = sea_neighbor->get_neighbor(Direction::east);
  // EXPECT_NE(nullptr, added);
  // EXPECT_EQ(added->get_id(), test->get_id());
}

TEST(tile_test, rotate_test)
{
  // // Rotating a tile should be clockwise. If the input value is negative,
  // then
  // // rotation is counter-clockwise.
  // // This should rotate all of the Tile's river points and neighbors
  // // accordingly.
  // std::set<Direction> rp;
  // rp.insert(Direction::north_east);
  // rp.insert(Direction::south_west);
  // rp.insert(Direction::west);
  // std::shared_ptr<Tile> test = std::make_shared<Tile>(rp);
  // std::shared_ptr<Tile> neighbor = std::make_shared<Tile>();
  // std::shared_ptr<Tile> river_neighbor = std::make_shared<Tile>(rp);
  // EXPECT_EQ(common::ERR_NONE,
  //           test->add_neighbor(river_neighbor, Direction::north_east));
  // EXPECT_EQ(common::ERR_NONE, test->add_neighbor(neighbor, Direction::east));

  // // When rotating 0, nothing should actually move.
  // EXPECT_EQ(common::ERR_NONE, test->rotate(0));
  // auto river_points = test->get_river_points();

  // EXPECT_EQ(3, river_points.size());
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::north_east)); EXPECT_TRUE(river_points.end()
  // != river_points.find(Direction::south_west));
  // EXPECT_TRUE(river_points.end() != river_points.find(Direction::west));

  // for (int d = 0; d < 6; d++)
  // {
  //   if (Direction::north_east == d)
  //   {
  //     EXPECT_TRUE(*river_neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else if (Direction::east == d)
  //   {
  //     EXPECT_TRUE(*neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else
  //   {
  //     EXPECT_EQ(nullptr, test->get_neighbor(static_cast<Direction>(d)));
  //   }
  // }

  // // When rotating 1, everything should move clockwise 1 step
  // EXPECT_EQ(common::ERR_NONE, test->rotate(1));
  // river_points = test->get_river_points();

  // EXPECT_EQ(3, river_points.size());
  // EXPECT_TRUE(river_points.end() != river_points.find(Direction::east));
  // EXPECT_TRUE(river_points.end() != river_points.find(Direction::west));
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::north_west));

  // for (int d = 0; d < 6; d++)
  // {
  //   if (Direction::east == d)
  //   {
  //     EXPECT_TRUE(*river_neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else if (Direction::south_east == d)
  //   {
  //     EXPECT_TRUE(*neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else
  //   {
  //     EXPECT_EQ(nullptr, test->get_neighbor(static_cast<Direction>(d)));
  //   }
  // }

  // // When rotating -1, everything should move counter-clockwise 1 step
  // EXPECT_EQ(common::ERR_NONE, test->rotate(-1));
  // river_points = test->get_river_points();

  // EXPECT_EQ(3, river_points.size());
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::north_east)); EXPECT_TRUE(river_points.end()
  // != river_points.find(Direction::south_west));
  // EXPECT_TRUE(river_points.end() != river_points.find(Direction::west));

  // for (int d = 0; d < 6; d++)
  // {
  //   if (Direction::north_east == d)
  //   {
  //     EXPECT_TRUE(*river_neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else if (Direction::east == d)
  //   {
  //     EXPECT_TRUE(*neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else
  //   {
  //     EXPECT_EQ(nullptr, test->get_neighbor(static_cast<Direction>(d)));
  //   }
  // }

  // // When rotating 8, everything should effectively move 2 steps clockwise
  // EXPECT_EQ(common::ERR_NONE, test->rotate(8));
  // river_points = test->get_river_points();

  // EXPECT_EQ(3, river_points.size());
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::south_east)); EXPECT_TRUE(river_points.end()
  // != river_points.find(Direction::north_west));
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::north_east));

  // for (int d = 0; d < 6; d++)
  // {
  //   if (Direction::south_east == d)
  //   {
  //     EXPECT_TRUE(*river_neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else if (Direction::south_west == d)
  //   {
  //     EXPECT_TRUE(*neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else
  //   {
  //     EXPECT_EQ(nullptr, test->get_neighbor(static_cast<Direction>(d)));
  //   }
  // }

  // // When rotating -15, everything should effectively move 3 steps
  // // counter-clockwise
  // EXPECT_EQ(common::ERR_NONE, test->rotate(-15));
  // river_points = test->get_river_points();

  // EXPECT_EQ(3, river_points.size());
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::north_west)); EXPECT_TRUE(river_points.end()
  // != river_points.find(Direction::south_east));
  // EXPECT_TRUE(river_points.end() !=
  // river_points.find(Direction::south_west));

  // for (int d = 0; d < 6; d++)
  // {
  //   if (Direction::north_west == d)
  //   {
  //     EXPECT_TRUE(*river_neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else if (Direction::north_east == d)
  //   {
  //     EXPECT_TRUE(*neighbor ==
  //                 *(test->get_neighbor(static_cast<Direction>(d))));
  //   }
  //   else
  //   {
  //     EXPECT_EQ(nullptr, test->get_neighbor(static_cast<Direction>(d)));
  //   }
  // }
}