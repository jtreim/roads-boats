#ifndef DESERT_H
#define DESERT_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Desert : public Tile
{
public:
  Desert();

  Desert(std::set<Direction> p_river_points);

  ~Desert();

  // nlohmann::json to_json();

protected:
private:
};
} // namespace tile
#endif // end DESERT_H