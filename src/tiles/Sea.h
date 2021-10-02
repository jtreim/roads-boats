#ifndef SEA_H
#define SEA_H

#include <memory>
#include <set>

#include <tiles/Land.h>


namespace tile
{
  class Sea : public Land
  {
    public:
      Sea();

      Sea(std::set<Direction> p_river_points);

      ~Sea();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

      bool is_shore() const;

    protected:
    private:
  };
}
#endif // end SEA_H