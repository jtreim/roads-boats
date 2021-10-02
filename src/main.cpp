#include <iostream>
#include <buildings/Building.h>
#include <tiles/Sea.h>

int main(void)
{
  std::cout << "This is a test!" << std::endl;

  std::cout << "\"sawmill\"=" << building::Building_type::sawmill << std::endl;

  tile::Sea tile;
  std::cout << "tile.is_shore(): " << tile.is_shore() << std::endl;

  return 0;
}