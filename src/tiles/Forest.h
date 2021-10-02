#ifndef FOREST_H
#define FOREST_H

#include <memory>
#include <set>

#include <tiles/Land.h>

namespace tile
{
  class Forest : public Land
  {
    public:
      Forest();

      Forest(std::set<Direction> p_river_points);

      ~Forest();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

    protected:
    private:
  };
}
#endif // end FOREST_H