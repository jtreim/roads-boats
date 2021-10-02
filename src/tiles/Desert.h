#ifndef DESERT_H
#define DESERT_H

#include <memory>
#include <set>

#include <tiles/Land.h>


namespace tile
{
  class Desert : public Land
  {
    public:
      Desert();

      Desert(std::set<Direction> p_river_points);

      ~Desert();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

    protected:
    private:
  };
}
#endif // end DESERT_H