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
    "north_west", "north_east", "east", "south_east", "south_west", "west"};
NLOHMANN_JSON_SERIALIZE_ENUM(Direction,
                             {{invalid_direction, nullptr},
                              {north_west, DIRECTION_NAMES[north_west]},
                              {north_east, DIRECTION_NAMES[north_east]},
                              {east, DIRECTION_NAMES[east]},
                              {south_east, DIRECTION_NAMES[south_east]},
                              {south_west, DIRECTION_NAMES[south_west]},
                              {west, DIRECTION_NAMES[west]}});
static bool is_valid(const Direction d)
{
  return ((0 <= d) && (MAX_DIRECTIONS > d));
}
static const std::set<Direction> ALL_DIRECTIONS = {
    north_west, north_east, east, south_east, south_west, west};
static std::string to_string(const Direction d)
{
  if (is_valid(d))
  {
    return DIRECTION_NAMES[d];
  }
  return "unknown";
}
static Direction direction_from_string(const std::string str)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    if (DIRECTION_NAMES[i] == str)
    {
      return static_cast<Direction>(i);
    }
  }
  return Direction::invalid_direction;
}
static Direction operator!(Direction d)
{
  return static_cast<Direction>((d + MAX_DIRECTIONS / 2) % MAX_DIRECTIONS);
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
    "north_west_left",  "north_west_right", "north_east_left",
    "north_east_right", "east_left",        "east_right",
    "south_east_left",  "south_east_right", "south_west_left",
    "south_west_right", "west_left",        "west_right"};
static const std::set<Border> ALL_BORDERS = {
    NW_left, NW_right, NE_left, NE_right, E_left, E_right,
    SE_left, SE_right, SW_left, SW_right, W_left, W_right};
NLOHMANN_JSON_SERIALIZE_ENUM(Border, {{invalid_border, nullptr},
                                      {NW_left, BORDER_NAMES[NW_left]},
                                      {NW_right, BORDER_NAMES[NW_right]},
                                      {NE_left, BORDER_NAMES[NE_left]},
                                      {NE_right, BORDER_NAMES[NE_right]},
                                      {E_left, BORDER_NAMES[E_left]},
                                      {E_right, BORDER_NAMES[E_right]},
                                      {SE_left, BORDER_NAMES[SE_left]},
                                      {SE_right, BORDER_NAMES[SE_right]},
                                      {SW_left, BORDER_NAMES[SW_left]},
                                      {SW_right, BORDER_NAMES[SW_right]},
                                      {W_left, BORDER_NAMES[W_left]},
                                      {W_right, BORDER_NAMES[W_right]}});
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
  if (b % 2 == 0)
  {
    return static_cast<Border>((1 + b + MAX_BORDERS / 2) % MAX_BORDERS);
  }
  return static_cast<Border>(((b + MAX_BORDERS / 2) - 1) % MAX_BORDERS);
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

static Border border_from_string(const std::string str)
{
  for (uint8_t i = 0; i < MAX_BORDERS; i++)
  {
    if (BORDER_NAMES[i] == str)
    {
      return static_cast<Border>(i);
    }
  }
  return Border::invalid_border;
}
} // namespace tile

static std::ostream &operator<<(std::ostream &output, const tile::Direction &d)
{
  output << tile::to_string(d);
  return output;
}
static std::ostream &operator<<(std::ostream &output, const tile::Border &b)
{
  output << tile::to_string(b);
  return output;
}

#endif