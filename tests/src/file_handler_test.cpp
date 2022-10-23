#include <filesystem>
#include <memory>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>
#include <utils/file_handler.h>

static const std::filesystem::path test_dir(
    std::filesystem::current_path().append("test_files").append("json"));

TEST(file_handler_test, load_resource_test)
{
  std::filesystem::path res_test_dir = test_dir;
  res_test_dir.append("resource");
  std::filesystem::path test_file;
  portable::Resource actual;
  std::set<player::Color> expected_carriers;
  expected_carriers.insert(player::Color::black);
  portable::Resource expected(portable::Resource::Type::trunks,
                              expected_carriers);

  // Invalid JSON keys
  test_file = res_test_dir;
  test_file /= "res_invalid_key.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Resource>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid field type
  test_file = res_test_dir;
  test_file /= "res_invalid_field.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Resource>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid carriers
  test_file = res_test_dir;
  test_file /= "res_invalid_carrier1.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Resource>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid carriers
  test_file = res_test_dir;
  test_file /= "res_invalid_carrier2.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Resource>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Loading valid json files should be fine
  test_file = res_test_dir;
  test_file /= "res_valid.json";
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<portable::Resource>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_cache_test)
{
  std::filesystem::path cache_test_dir = test_dir;
  cache_test_dir.append("cache");
  std::filesystem::path test_file = cache_test_dir;
  portable::Cache actual;

  // Match expected to cache_sample.json
  portable::Cache expected;
  std::set<player::Color> carriers;
  carriers.insert(player::Color::blue);
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::iron));
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::goose));
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::goose));
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::goose));
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::goose));
  ASSERT_EQ(common::ERR_NONE, expected.add(portable::Resource::Type::gold));
  carriers.insert(player::red);
  carriers.insert(player::black);
  portable::Resource *fuel =
      new portable::Resource(portable::Resource::Type::fuel, carriers);
  ASSERT_EQ(common::ERR_NONE, expected.add(fuel));

  // Invalid JSON keys
  test_file = cache_test_dir;
  test_file /= "cache_invalid_key.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Cache>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid field type
  test_file = cache_test_dir;
  test_file /= "cache_invalid_field_type.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Cache>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Mixed resource within a single list
  test_file = cache_test_dir;
  test_file /= "cache_mixed_resource_list.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<portable::Cache>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Loading valid json files should be fine
  test_file = cache_test_dir;
  test_file /= "cache_sample.json";
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<portable::Cache>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_hex_test)
{
  std::filesystem::path hex_test_dir = test_dir;
  hex_test_dir.append("hex");
  std::filesystem::path test_file;
  tile::Hex actual;
  tile::Hex expected(0, 1);

  // Invalid JSON keys
  test_file = hex_test_dir;
  test_file /= "hex_exception1.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Hex>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid JSON field type
  test_file = hex_test_dir;
  test_file /= "hex_exception2.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Hex>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Loading valid json files should be fine
  test_file = hex_test_dir;
  test_file /= "hex_sample.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::Hex>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_river_test)
{
  std::filesystem::path river_test_dir = test_dir;
  river_test_dir.append("river");
  std::filesystem::path test_file;
  std::set<tile::Direction> exp_points;
  exp_points.insert(tile::Direction::north_west);
  exp_points.insert(tile::Direction::north_east);
  exp_points.insert(tile::Direction::south_east);
  tile::River actual;
  tile::River expected(exp_points);

  // Invalid JSON keys
  test_file = river_test_dir;
  test_file /= "river_invalid_key.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::River>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Missing JSON field
  test_file = river_test_dir;
  test_file /= "river_missing_field.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::River>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid river point
  test_file = river_test_dir;
  test_file /= "river_invalid_point.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::River>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid bridge
  test_file = river_test_dir;
  test_file /= "river_invalid_bridge.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::River>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Loading valid json files should be fine
  test_file = river_test_dir;
  test_file /= "river_sample_1.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::River>(test_file, actual));
  EXPECT_EQ(expected, actual);

  // Loading bridges should be fine too
  ASSERT_EQ(common::ERR_NONE, expected.build(tile::Direction::north_west));
  test_file = river_test_dir;
  test_file /= "river_sample_2.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::River>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_area_test)
{
  std::filesystem::path area_test_dir = test_dir;
  area_test_dir.append("area");
  std::filesystem::path test_file;
  tile::Area actual;
  std::set<tile::Border> exp_borders;
  exp_borders.insert(tile::Border::NW_left);
  exp_borders.insert(tile::Border::NW_right);
  exp_borders.insert(tile::Border::NE_left);
  exp_borders.insert(tile::Border::NE_right);
  exp_borders.insert(tile::Border::E_left);
  exp_borders.insert(tile::Border::W_right);
  tile::Area expected(exp_borders);

  // Invalid border listed
  test_file = area_test_dir;
  test_file /= "area_invalid_border.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Area>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid road listed
  test_file = area_test_dir;
  test_file /= "area_invalid_road.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Area>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid field type
  test_file = area_test_dir;
  test_file /= "area_invalid_field_type.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Area>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Missing field
  test_file = area_test_dir;
  test_file /= "area_missing_field.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Area>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Loading valid json files should be fine
  test_file = area_test_dir;
  test_file /= "area_sample_1.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::Area>(test_file, actual));
  EXPECT_EQ(expected, actual);

  // Test that resources/roads are loaded as expected
  portable::Resource goose1 =
      portable::Resource(portable::Resource::Type::goose);
  std::set<player::Color> carriers;
  carriers.insert(player::Color::blue);
  ASSERT_EQ(common::ERR_NONE, expected.build(tile::Border::NW_left));
  ASSERT_EQ(common::ERR_NONE, expected.build(tile::Border::E_left));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(new portable::Resource(
                                  portable::Resource::Type::goose)));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(new portable::Resource(
                                  portable::Resource::Type::goose, carriers)));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(new portable::Resource(
                                  portable::Resource::Type::fuel)));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(new portable::Resource(
                                  portable::Resource::Type::iron)));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(new portable::Resource(
                                  portable::Resource::Type::gold, carriers)));
  test_file = area_test_dir;
  test_file /= "area_sample_2.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::Area>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_tile_test)
{
  std::filesystem::path tile_test_dir = test_dir;
  std::filesystem::path test_file;
  tile_test_dir.append("tile");
  tile::Tile actual;

  // Create the expected tile to test against
  std::set<tile::Direction> rp;
  rp.insert(tile::Direction::north_west);
  rp.insert(tile::Direction::south_east);
  rp.insert(tile::Direction::west);
  tile::Hex hex(1, 0);
  tile::Tile expected(hex, rp, tile::Terrain::forest);
  std::set<tile::Direction> neighbor_rp;
  neighbor_rp.insert(tile::Direction::east);
  std::shared_ptr<tile::Tile> neighbor = std::make_shared<tile::Tile>(
      hex.neighbor(tile::Direction::west), neighbor_rp);
  ASSERT_EQ(common::ERR_NONE,
            expected.add_neighbor(neighbor, tile::Direction::west));
  ASSERT_EQ(common::ERR_NONE,
            neighbor->add_neighbor(std::make_shared<tile::Tile>(expected),
                                   tile::Direction::east));
  ASSERT_EQ(common::ERR_NONE,
            expected.build_bridge(tile::Direction::south_east));
  ASSERT_EQ(common::ERR_NONE, expected.build_road(tile::Border::W_right));
  ASSERT_EQ(common::ERR_NONE,
            expected.build_wall(tile::Direction::west, player::Color::blue, 2));

  // Missing field
  test_file = tile_test_dir;
  test_file /= "tile_missing_field.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid field type
  test_file = tile_test_dir;
  test_file /= "tile_invalid_field_type.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid Rivers listed
  test_file = tile_test_dir;
  test_file /= "tile_invalid_river.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // One of the rivers has at least one duplicate river point
  test_file = tile_test_dir;
  test_file /= "tile_duplicate_river_points.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid Areas listed
  test_file = tile_test_dir;
  test_file /= "tile_invalid_area.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Areas listed are inconsistent with river points
  test_file = tile_test_dir;
  test_file /= "tile_inconsistent_areas.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid neighbor coordinates
  test_file = tile_test_dir;
  test_file /= "tile_invalid_neighbor.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // Invalid walls listed
  test_file = tile_test_dir;
  test_file /= "tile_invalid_walls_1.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  test_file = tile_test_dir;
  test_file /= "tile_invalid_walls_2.json";
  EXPECT_EQ(common::ERR_INVALID,
            utils::load_json<tile::Tile>(test_file, actual));
  EXPECT_NE(expected, actual);

  // A valid tile should be okay
  test_file = tile_test_dir;
  test_file /= "tile_sample.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json(test_file, actual));
  EXPECT_EQ(expected.get_rivers().size(), actual.get_rivers().size());
  EXPECT_EQ(expected.get_river_points(), actual.get_river_points());
  // TODO: Loading tiles from a json seems to drop bridges...
  // EXPECT_EQ(expected.get_bridges(), actual.get_bridges());
  EXPECT_EQ(expected.get_areas().size(), actual.get_areas().size());
  for (auto area : expected.get_areas())
  {
    tile::Border border = (*area->get_borders().begin());
    std::shared_ptr<tile::Area> matching_area = actual.get_area(border);
    EXPECT_EQ(area->get_borders(), matching_area->get_borders());
    // TODO: Loading tiles from a json seems to drop roads...
    // EXPECT_EQ(area->get_roads(), matching_area->get_roads());
    EXPECT_EQ(area->get_building(), matching_area->get_building());
  }
  // Loading a tile with neighbors should indicate it needs its neighbors' data
  // before further use
  EXPECT_FALSE(actual.neighbors_are_current());
  EXPECT_EQ(common::ERR_FAIL, actual.build_road(tile::Border::W_left));
  EXPECT_EQ(common::ERR_FAIL, actual.build_bridge(tile::Direction::south_east));
}

TEST(file_handler_test, load_tile_map_test)
{
  std::filesystem::path tile_map_test_dir = test_dir;
  tile_map_test_dir.append("tile_map");
}

TEST(file_handler_test, dump_resource_test)
{
  std::filesystem::path res_test_dir = test_dir;
  res_test_dir.append("resource");
  std::filesystem::path test_file = res_test_dir;
  std::set<player::Color> exp_carriers;
  exp_carriers.insert(player::Color::black);
  exp_carriers.insert(player::Color::red);
  portable::Resource test_object(portable::Resource::Type::stone, exp_carriers);
  portable::Resource loaded_object;

  // Dumping to a file that didn't exist prior should create the file for us.
  test_file /= "new_resource.json";
  std::filesystem::remove(test_file);
  ASSERT_FALSE(std::filesystem::exists(test_file));
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<portable::Resource>(test_file, test_object));
  EXPECT_TRUE(std::filesystem::exists(test_file));

  // Dumping a resource should result in a file we can read
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<portable::Resource>(test_file, loaded_object));
  // Reloading the same file should result in the same object
  EXPECT_EQ(test_object, loaded_object);

  // Reset testing folder
  std::filesystem::remove(test_file);
}

TEST(file_handler_test, dump_cache_test)
{
  std::filesystem::path cache_test_dir = test_dir;
  cache_test_dir.append("cache");
  std::filesystem::path test_file = cache_test_dir;
  portable::Cache test_object;
  std::map<portable::Resource::Type, std::vector<portable::Resource>> cache_map;
  std::set<player::Color> carriers;
  portable::Resource *bomb_ptr =
      new portable::Resource(portable::Resource::bomb, carriers);
  carriers.insert(player::Color::blue);
  ASSERT_EQ(common::ERR_NONE, test_object.add(portable::Resource::Type::bomb));
  ASSERT_EQ(common::ERR_NONE, test_object.add(bomb_ptr));
  ASSERT_EQ(common::ERR_NONE,
            test_object.add(portable::Resource::Type::boards));
  portable::Cache loaded_object;

  // Dumping to a file that didn't exist prior should create the file for us.
  test_file /= "new_cache.json";
  std::filesystem::remove(test_file);
  ASSERT_FALSE(std::filesystem::exists(test_file));
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<portable::Cache>(test_file, test_object));
  EXPECT_TRUE(std::filesystem::exists(test_file));

  // Dumping a hex should result in a file we can read.
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<portable::Cache>(test_file, loaded_object));
  // Reloading the same file should result in the same object
  EXPECT_EQ(test_object, loaded_object);

  // Reset testing folder
  std::filesystem::remove(test_file);
}

TEST(file_handler_test, dump_hex_test)
{
  std::filesystem::path hex_test_dir = test_dir;
  hex_test_dir.append("hex");
  std::filesystem::path test_file = hex_test_dir;
  tile::Hex test_object(1, 0);
  tile::Hex loaded_object;

  // Dumping to a file that didn't exist prior should create the file for us.
  test_file /= "new_hex.json";
  std::filesystem::remove(test_file);
  ASSERT_FALSE(std::filesystem::exists(test_file));
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<tile::Hex>(test_file, test_object));
  EXPECT_TRUE(std::filesystem::exists(test_file));

  // Dumping a hex should result in a file we can read.
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<tile::Hex>(test_file, loaded_object));
  // Reloading the same file should result in the same object
  EXPECT_EQ(test_object, loaded_object);

  // Reset testing folder
  std::filesystem::remove(test_file);
}

TEST(file_handler_test, dump_river_test)
{
  std::filesystem::path river_test_dir = test_dir;
  river_test_dir.append("river");
  std::filesystem::path test_file = river_test_dir;
  std::set<tile::Direction> rp;
  rp.insert(tile::Direction::east);
  rp.insert(tile::Direction::west);
  tile::River test_object(rp);
  tile::River loaded_object;

  test_file /= "new_river.json";
  std::filesystem::remove(test_file);
  ASSERT_FALSE(std::filesystem::exists(test_file));

  // Dumping to a file that didn't exist prior should create the file for us.
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<tile::River>(test_file, test_object));
  EXPECT_TRUE(std::filesystem::exists(test_file));

  // Dumping a river should result in a file we can read.
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<tile::River>(test_file, loaded_object));
  // Reloading the same file should result in the same object.
  EXPECT_EQ(test_object, loaded_object);

  // Make sure bridges can be loaded too.
  ASSERT_EQ(common::ERR_NONE, test_object.build(tile::Direction::east));
  ASSERT_NE(test_object, loaded_object);
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<tile::River>(test_file, test_object));
  // If the file is found, it should get overwritten when dumping.
  // Bridge changes should be reflected now.
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<tile::River>(test_file, loaded_object));
  EXPECT_EQ(test_object, loaded_object);

  // Reset testing folder
  std::filesystem::remove(test_file);
}

TEST(file_handler_test, dump_area_test)
{
  std::filesystem::path area_test_dir = test_dir;
  area_test_dir.append("area");
  std::filesystem::path test_file = area_test_dir;
  std::set<tile::Border> exp_borders;
  exp_borders.insert(tile::Border::NW_left);
  exp_borders.insert(tile::Border::NW_right);
  exp_borders.insert(tile::Border::NE_left);
  exp_borders.insert(tile::Border::NE_right);
  exp_borders.insert(tile::Border::E_left);
  exp_borders.insert(tile::Border::W_right);
  tile::Area test_object(exp_borders);
  ASSERT_EQ(common::ERR_NONE, test_object.build(tile::Border::E_left));
  ASSERT_EQ(common::ERR_NONE, test_object.build(tile::Border::NW_left));
  tile::Area loaded_object;

  // Dumping to a file that didn't exist prior should create the file for us.
  test_file /= "new_area.json";
  std::filesystem::remove(test_file);
  ASSERT_FALSE(std::filesystem::exists(test_file));
  EXPECT_EQ(common::ERR_NONE,
            utils::dump_json<tile::Area>(test_file, test_object));
  EXPECT_TRUE(std::filesystem::exists(test_file));

  // Dumping an area should result in a file we can read.
  EXPECT_EQ(common::ERR_NONE,
            utils::load_json<tile::Area>(test_file, loaded_object));
  // Reloading the same file should result in the same object
  EXPECT_EQ(test_object, loaded_object);

  // Reset testing folder
  std::filesystem::remove(test_file);
}

TEST(file_handler_test, dump_tile_test)
{
  std::filesystem::path tile_test_dir = test_dir;
  tile_test_dir.append("tile");
}

TEST(file_handler_test, dump_tile_map_test)
{
  std::filesystem::path tile_map_test_dir = test_dir;
  tile_map_test_dir.append("tile_map");
}