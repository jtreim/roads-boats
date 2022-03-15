#include <filesystem>
#include <memory>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <common/Errors.h>
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

TEST(file_handler_test, dump_hex_test)
{
  std::filesystem::path hex_test_dir = test_dir;
  hex_test_dir.append("hex");
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