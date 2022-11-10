#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <players/Color.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>
#include <vector>

namespace player
{

/// Base Player class
///
class Player
{
public:
  Color get_color() const;

  std::vector<std::unique_ptr<portable::Transporter>> get_transporters() const;

  bool create_transporter(tile::Tile location);

  bool remove_transporter(std::shared_ptr<portable::Transporter> transporter);

protected:
private:
  Color m_color;
  std::vector<std::unique_ptr<portable::Transporter>> m_transporters;
};
} // namespace player
#endif // end PLAYER_H