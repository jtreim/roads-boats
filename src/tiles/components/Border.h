#ifndef BORDER_H
#define BORDER_H

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <common/Errors.h>

namespace tile
{

enum Direction
{
  invalid_direction = -1,
  north_west = 0,
  north_east,
  east,
  south_east,
  south_west,
  west
};
static const uint8_t MAX_DIRECTIONS = 6;
static const std::string DIRECTION_NAMES[MAX_DIRECTIONS]{
    "northwest", "northeast", "east", "southeast", "southwest", "west"};
static bool is_valid(const Direction d)
{
  return ((0 <= d) && (MAX_DIRECTIONS > d));
}
static std::set<Direction> get_all_directions()
{
  std::set<Direction> retval;
  for (int i = 0; i < MAX_DIRECTIONS; i++)
  {
    retval.insert(static_cast<Direction>(i));
  }
  return retval;
}
static std::string to_string(const Direction d)
{
  if (is_valid(d))
  {
    return DIRECTION_NAMES[d];
  }
  return "unknown";
}
static Direction operator!(Direction d)
{
  return static_cast<Direction>((d + MAX_DIRECTIONS / 2) % MAX_DIRECTIONS);
}
static std::ostream &operator<<(std::ostream &output, const Direction &d)
{
  output << to_string(d);
  return output;
}
static nlohmann::json json(const Direction d)
{
  nlohmann::json retval;
  retval["val"] = d;
  retval["name"] = to_string(d);
  return retval;
}

enum Border
{
  invalid_border = -1,
  NW_left = 0,
  NW_right,
  NE_left,
  NE_right,
  E_left,
  E_right,
  SE_left,
  SE_right,
  SW_left,
  SW_right,
  W_left,
  W_right
};
static const uint8_t MAX_BORDERS = 12;
static const std::string BORDER_NAMES[MAX_BORDERS]{
    "northwest_left", "northwest_right", "northeast_left", "northeast_right",
    "east_left",      "east_right",      "southeast_left", "southeast_right",
    "southwest_left", "southwest_right", "west_left",      "west_right"};
static std::set<Border> get_all_borders()
{
  std::set<Border> retval;
  for (int i = 0; i < MAX_BORDERS; i++)
  {
    retval.insert(static_cast<Border>(i));
  }
  return retval;
}
static bool is_valid(const Border b) { return ((0 <= b) && (MAX_BORDERS > b)); }
static std::string to_string(const Border b)
{
  if (is_valid(b))
  {
    return BORDER_NAMES[b];
  }
  return "unknown";
}
static Border operator!(Border b)
{
  return static_cast<Border>((b + MAX_BORDERS / 2) % MAX_BORDERS);
}
static std::ostream &operator<<(std::ostream &output, const Border &b)
{
  output << to_string(b);
  return output;
}
static nlohmann::json json(const Border b)
{
  nlohmann::json retval;
  retval["val"] = b;
  retval["name"] = to_string(b);
  return retval;
}

static Direction direction_from_border(const Border b)
{
  // This works so long as each direction has 2 borders, and if both
  // Directions and Borders are sorted the same.
  int converted = static_cast<int>(b);
  return static_cast<Direction>(converted / 2);
}
static std::vector<Border> borders_from_direction(const Direction d)
{
  // This works so long as each direction has 2 borders, and if both
  // Directions and Borders are sorted the same.
  std::vector<Border> borders;
  borders.push_back(static_cast<Border>(d * 2));
  borders.push_back(static_cast<Border>(d * 2 + 1));
  return borders;
}

} // namespace tile

#endif