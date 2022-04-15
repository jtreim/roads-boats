#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include <portables/transporters/Transporter.h>

namespace player
{
class Transporter;
class Tile;

/// Possible Player colors
///
enum Color
{
  invalid = -1,
  black = 0,
  blue,
  green,
  grey,
  red,
  yellow,
  neutral
};
static const uint8_t MAX_PLAYER_COLORS = 6;
static const uint8_t MAX_COLORS = 7;
static const std::string COLOR_NAMES[MAX_COLORS]{
    "black", "blue", "green", "grey", "red", "yellow", "neutral"};
NLOHMANN_JSON_SERIALIZE_ENUM(Color, {{invalid, nullptr},
                                     {black, COLOR_NAMES[black]},
                                     {blue, COLOR_NAMES[blue]},
                                     {green, COLOR_NAMES[green]},
                                     {grey, COLOR_NAMES[grey]},
                                     {red, COLOR_NAMES[red]},
                                     {yellow, COLOR_NAMES[yellow]},
                                     {neutral, COLOR_NAMES[neutral]}});
static bool is_valid(const Color c) { return ((0 <= c) && (MAX_COLORS > c)); }
static std::string to_string(const Color c)
{
  if (is_valid(c))
  {
    return COLOR_NAMES[c];
  }
  return "unknown";
}

/// Base Player class
///
class Player
{
public:
  Color get_color() const;

  std::vector<std::unique_ptr<portable::Transporter>> get_transporters() const;

  bool create_transporter(Tile location);

  bool remove_transporter(std::shared_ptr<portable::Transporter> transporter);

protected:
private:
  Color m_p_color;
  std::vector<std::unique_ptr<portable::Transporter>> mp_transporters;
};
} // namespace player
#endif // end PLAYER_H