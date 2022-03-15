#include <filesystem>
#include <memory>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <portables/Resource.h>
#include <tiles/components/Area.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>
#include <utils/file_handler.h>

static const std::filesystem::path
    test_dir(std::filesystem::current_path().append("test_res").append("json"));

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

  ASSERT_EQ(common::ERR_NONE, expected.build(tile::Border::NW_left));
  ASSERT_EQ(common::ERR_NONE, expected.build(tile::Border::E_left));
  ASSERT_EQ(common::ERR_NONE, expected.add_resource(portable::Resource::goose));
  ASSERT_EQ(common::ERR_NONE,
            expected.add_resource(portable::Resource::fuel, 2));
  ASSERT_EQ(common::ERR_NONE,
            expected.add_resource(portable::Resource::iron, 2));
  ASSERT_EQ(common::ERR_NONE,
            expected.add_resource(portable::Resource::gold, 5));
  test_file = area_test_dir;
  test_file /= "area_sample_2.json";
  EXPECT_EQ(common::ERR_NONE, utils::load_json<tile::Area>(test_file, actual));
  EXPECT_EQ(expected, actual);
}

TEST(file_handler_test, load_tile_test)
{
  std::filesystem::path tile_test_dir = test_dir;
  tile_test_dir.append("tile");
}

TEST(file_handler_test, load_tile_map_test)
{
  std::filesystem::path tile_map_test_dir = test_dir;
  tile_map_test_dir.append("tile_map");
}

TEST(file_handler_test, dump_hex_test)
{
  std::filesystem::path hex_test_dir = test_dir;
  hex_test_dir.append("hex");
}

TEST(file_handler_test, dump_river_test)
{
  std::filesystem::path river_test_dir = test_dir;
  river_test_dir.append("river");
}

TEST(file_handler_test, dump_area_test)
{
  std::filesystem::path area_test_dir = test_dir;
  area_test_dir.append("area");
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