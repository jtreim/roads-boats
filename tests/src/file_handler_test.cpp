#include <filesystem>
#include <memory>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <tiles/components/Hex.h>
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