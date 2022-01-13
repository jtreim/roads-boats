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
  BLACK,
  BLUE,
  GREEN,
  GREY,
  RED,
  YELLOW
};

/// Base Player class
///
class Player
{
public:
  Color get_color() const;

  std::vector<std::unique_ptr<portable::Transporter>> get_transporters() const;

  bool create_transporter(portable::Transporter_type trans, Tile location);

  bool remove_transporter(std::shared_ptr<portable::Transporter> transporter);

protected:
private:
  Color mp_color;
  std::vector<std::unique_ptr<portable::Transporter>> mp_transporters;
};
} // namespace player
#endif // end PLAYER_H