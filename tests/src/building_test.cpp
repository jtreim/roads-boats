#include <memory>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <buildings/Primary.h>
#include <buildings/Secondary.h>

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

#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/Tile.h>

using namespace building;

TEST(building_test, create_building_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;

  Woodcutter w = Woodcutter();

  EXPECT_EQ(Building::Type::woodcutter, w.get_type());
  // A woodcutter is a primary producer. They can be powered, but not managed.
  EXPECT_TRUE(w.can_add_electricity());
  EXPECT_FALSE(w.can_add_manager());
  // Primary producers don't require resources to produce. After creation, they
  // should be able to immediately produce.
  EXPECT_TRUE(w.can_produce(cache, transporters));
  // After adding electricity, a primary producer shouldn't be able to add it
  // again until the building is reset.
  EXPECT_EQ(common::ERR_NONE, w.add_electricity());
  EXPECT_FALSE(w.can_add_electricity());

  // Copying should transfer everything
  Woodcutter other = w;
  EXPECT_EQ(Building::Type::woodcutter, other.get_type());
  EXPECT_FALSE(other.can_add_electricity());
  EXPECT_FALSE(other.can_add_manager());
  EXPECT_TRUE(other.can_produce(cache, transporters));

  Sawmill s = Sawmill();

  EXPECT_EQ(Building::Type::sawmill, s.get_type());
  // A Sawmill is a secondary producer. They can be managed, not powered.
  EXPECT_TRUE(s.can_add_manager());
  EXPECT_FALSE(s.can_add_electricity());
  // Secondary producers require resources to produce.
  EXPECT_FALSE(s.can_produce(cache, transporters));
  // With the right input resources, they should be able to produce though.
  cache.add(portable::Resource::Type::trunks);
  EXPECT_TRUE(s.can_produce(cache, transporters));
  // After adding a manager, a secondary producer shouldn't be able to add it
  // again until the building is reset.
  EXPECT_EQ(common::ERR_NONE, s.add_manager());
  EXPECT_FALSE(s.can_add_manager());

  // Copying should transfer everything
  Sawmill other_s = s;
  EXPECT_EQ(Building::Type::sawmill, other_s.get_type());
  EXPECT_FALSE(other_s.can_add_manager());
  EXPECT_FALSE(other_s.can_add_electricity());
  EXPECT_TRUE(other_s.can_produce(cache, transporters));
}

TEST(building_test, produce_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  cache.add(portable::Resource::goose);
  cache.add(portable::Resource::gold);
  std::vector<portable::Portable *> output;

  Woodcutter w = Woodcutter();
  Sawmill s = Sawmill();

  // A secondary producer requires input resources
  EXPECT_EQ(6, s.count_remaining_production());
  EXPECT_FALSE(s.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, s.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());

  // A primary producer does not require input resources
  EXPECT_TRUE(w.can_produce(cache, transporters));
  EXPECT_EQ(1, w.count_remaining_production());
  EXPECT_EQ(common::ERR_NONE, w.produce(cache, transporters, output));
  EXPECT_EQ(1, output.size());
  ASSERT_EQ(portable::Portable::Object::resource, output.at(0)->get_object());
  EXPECT_EQ(portable::Resource::Type::trunks,
            static_cast<portable::Resource *>(output.at(0))->get_type());

  // After producing everything allowed by the woodcutter, it shouldn't be
  // able to produce until the building is reset.
  EXPECT_FALSE(w.can_produce(cache, transporters));
  EXPECT_EQ(0, w.count_remaining_production());
  w.reset();
  EXPECT_TRUE(w.can_produce(cache, transporters));
  EXPECT_EQ(1, w.count_remaining_production());

  // Go ahead and put the produced trunks in the cache
  ASSERT_EQ(common::ERR_NONE, cache.add(output));
  output.clear();

  // With the proper input, the secondary producer should produce
  EXPECT_EQ(0, output.size());
  EXPECT_TRUE(s.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, s.produce(cache, transporters, output));
  // Only resources needed for production should be removed from the cache
  EXPECT_EQ(1, cache.count(portable::Resource::goose));
  EXPECT_EQ(1, cache.count(portable::Resource::gold));
  EXPECT_EQ(0, cache.count(portable::Resource::trunks));
  // Remaining production should be decremented accordingly
  EXPECT_EQ(4, s.count_remaining_production());
  EXPECT_EQ(2, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource, output.at(i)->get_object());
    EXPECT_EQ(portable::Resource::Type::boards,
              static_cast<portable::Resource *>(output.at(0))->get_type());
  }

  ASSERT_EQ(common::ERR_NONE, cache.add(output));
  output.clear();
  
  // The sawmill hasn't maxed out its production, so with the proper input, it
  // should still be able to produce
  cache.add(portable::Resource::trunks);
  cache.add(portable::Resource::trunks);
  cache.add(portable::Resource::trunks);
  EXPECT_EQ(0, output.size());
  EXPECT_TRUE(s.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, s.produce(cache, transporters, output));
  EXPECT_EQ(1, cache.count(portable::Resource::goose));
  EXPECT_EQ(1, cache.count(portable::Resource::gold));
  // Leftover input resources should be untouched
  EXPECT_EQ(1, cache.count(portable::Resource::trunks));
  EXPECT_EQ(0, s.count_remaining_production());
  EXPECT_EQ(4, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource, output.at(i)->get_object());
    EXPECT_EQ(portable::Resource::Type::boards,
              static_cast<portable::Resource *>(output.at(0))->get_type());
  }
}

TEST(building_test, double_output_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter *> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;

  Woodcutter w = Woodcutter();
  Sawmill s = Sawmill();

  // A woodcutter is a primary producer. They can be powered, but not managed.
  EXPECT_TRUE(w.can_add_electricity());
  EXPECT_FALSE(w.can_add_manager());
  EXPECT_EQ(common::ERR_FAIL, w.add_manager());
  // When powered, the woodcutter should double its max output
  EXPECT_EQ(1, w.count_remaining_production());
  EXPECT_EQ(common::ERR_NONE, w.add_electricity());
  EXPECT_EQ(2, w.count_remaining_production());
  EXPECT_EQ(common::ERR_NONE, w.produce(cache, transporters, output));
  EXPECT_EQ(2, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource, output.at(i)->get_object());
    EXPECT_EQ(portable::Resource::Type::trunks,
              static_cast<portable::Resource *>(output.at(i))->get_type());
  }
  EXPECT_EQ(0, w.count_remaining_production());
  
  // Clear output list for testing sawmill
  output.clear();
  
  // A managed sawmill can produce up to 12 boards. It requires 6 trunks as
  // input.
  // Add the requisite trunks, plus a couple extra for testing purposes.
  for (uint8_t i = 0; i < 8; i++)
  {
    cache.add(portable::Resource::trunks);
  }

  // A Sawmill is a secondary producer. They can be managed, not powered.
  EXPECT_TRUE(s.can_add_manager());
  EXPECT_FALSE(s.can_add_electricity());
  EXPECT_EQ(common::ERR_FAIL, s.add_electricity());

  // Run a standard production without a manager
  EXPECT_EQ(6, s.count_remaining_production());
  EXPECT_EQ(common::ERR_NONE, s.produce(cache, transporters, output));
  EXPECT_EQ(0, s.count_remaining_production());
  EXPECT_EQ(6, output.size());
  EXPECT_EQ(5, cache.count(portable::Resource::trunks));
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource, output.at(i)->get_object());
    EXPECT_EQ(portable::Resource::Type::boards,
              static_cast<portable::Resource *>(output.at(i))->get_type());
  }

  // When managed, the sawmill should double its max output
  EXPECT_EQ(common::ERR_NONE, s.add_manager());
  // Max production: 12. Remaining production: 12 - 6 = 6.
  EXPECT_EQ(6, s.count_remaining_production());
  EXPECT_EQ(common::ERR_NONE, s.produce(cache, transporters, output));
  EXPECT_EQ(12, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource, output.at(i)->get_object());
    EXPECT_EQ(portable::Resource::Type::boards,
              static_cast<portable::Resource *>(output.at(i))->get_type());
  }
  EXPECT_EQ(0, s.count_remaining_production());
  // 6 of the 8 trunks should have been used for production
  EXPECT_EQ(2, cache.count(portable::Resource::trunks));

  // After adding electricity, a primary producer shouldn't be able to add it
  // again until the building is reset.
  EXPECT_FALSE(w.can_add_electricity());
  EXPECT_EQ(common::ERR_FAIL, w.add_electricity());
  w.reset();
  EXPECT_TRUE(w.can_add_electricity());
  // After adding a manager, a secondary producer shouldn't be able to add it
  // again until the building is reset.
  EXPECT_FALSE(s.can_add_manager());
  EXPECT_EQ(common::ERR_FAIL, s.add_manager());
  s.reset();
  EXPECT_TRUE(s.can_add_manager());
}

TEST(building_test, construct_building_test)
{

}

TEST(building_test, raft_factory_test)
{

}

TEST(building_test, rowboat_factory_test)
{
  
}

TEST(building_test, steamer_factory_test)
{
  
}

TEST(building_test, truck_factory_test)
{
  
}

TEST(building_test, wagon_factory_test)
{
  
}

TEST(building_test, clay_pit_factory_test)
{
  
}

TEST(building_test, coal_burner_factory_test)
{
  
}

TEST(building_test, mine_factory_test)
{
  
}

TEST(building_test, mint_factory_test)
{
  
}

TEST(building_test, oil_rig_factory_test)
{
  
}

TEST(building_test, papermill_factory_test)
{
  
}

TEST(building_test, quarry_factory_test)
{
  
}

TEST(building_test, sawmill_factory_test)
{
  
}

TEST(building_test, stock_exchange_factory_test)
{
  
}

TEST(building_test, stone_factory_factory_test)
{
  
}

TEST(building_test, woodcutter_factory_test)
{
  
}