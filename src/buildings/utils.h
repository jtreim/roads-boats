#include <memory>

#include <buildings/Building.h>
#include <buildings/factories/Raft_factory.h>
#include <buildings/factories/Rowboat_factory.h>
#include <buildings/factories/Steamer_factory.h>
#include <buildings/factories/Truck_factory.h>
#include <buildings/factories/Wagon_factory.h>
#include <buildings/producers/Clay_pit.h>
#include <buildings/producers/Coal_burner.h>
#include <buildings/producers/Mine.h>
#include <buildings/producers/Mint.h>
#include <buildings/producers/Oil_rig.h>
#include <buildings/producers/Papermill.h>
#include <buildings/producers/Quarry.h>
#include <buildings/producers/Sawmill.h>
#include <buildings/producers/Stock_exchange.h>
#include <buildings/producers/Stone_factory.h>
#include <buildings/producers/Woodcutter.h>
#include <common/Errors.h>

namespace building
{
static common::Error make_building(const Building::Type to_build,
                                   std::unique_ptr<Building> &result,
                                   Building *to_copy = nullptr)
{
  common::Error err;

  switch (to_build)
  {
  case building::Building::Type::woodcutter:
    if (nullptr != to_copy)
    {
      result =
          std::make_unique<Woodcutter>(*(static_cast<Woodcutter *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Woodcutter>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::oil_rig:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Oil_rig>(*(static_cast<Oil_rig *>(to_copy)));
    }
    else
      err = common::ERR_NONE;
    {
      result = std::make_unique<Oil_rig>();
    }
    break;
  case building::Building::Type::quarry:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Quarry>(*(static_cast<Quarry *>(to_copy)));
    }
    else
      err = common::ERR_NONE;
    {
      result = std::make_unique<Quarry>();
    }
    break;
  case building::Building::Type::clay_pit:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Clay_pit>(*(static_cast<Clay_pit *>(to_copy)));
    }
    else
      err = common::ERR_NONE;
    {
      result = std::make_unique<Clay_pit>();
    }
    break;
  case building::Building::Type::mine:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Mine>(*(static_cast<Mine *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Mine>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::sawmill:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Sawmill>(*(static_cast<Sawmill *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Sawmill>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::coal_burner:
    if (nullptr != to_copy)
    {
      result =
          std::make_unique<Coal_burner>(*(static_cast<Coal_burner *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Coal_burner>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::papermill:
    if (nullptr != to_copy)
    {
      result =
          std::make_unique<Papermill>(*(static_cast<Papermill *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Papermill>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::stone_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Stone_factory>(
          *(static_cast<Stone_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Stone_factory>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::mint:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Mint>(*(static_cast<Mint *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Mint>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::stock_exchange:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Stock_exchange>(
          *(static_cast<Stock_exchange *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Stock_exchange>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::wagon_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Wagon_factory>(
          *(static_cast<Wagon_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Wagon_factory>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::truck_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Truck_factory>(
          *(static_cast<Truck_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Truck_factory>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::raft_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Raft_factory>(
          *(static_cast<Raft_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Raft_factory>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::rowboat_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Rowboat_factory>(
          *(static_cast<Rowboat_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Rowboat_factory>();
    }
    err = common::ERR_NONE;
    break;
  case building::Building::Type::steamer_factory:
    if (nullptr != to_copy)
    {
      result = std::make_unique<Steamer_factory>(
          *(static_cast<Steamer_factory *>(to_copy)));
    }
    else
    {
      result = std::make_unique<Steamer_factory>();
    }
    err = common::ERR_NONE;
    break;
  default:
    result.reset();
    err = common::ERR_FAIL;
    break;
  }

  return err;
}
}; // namespace building