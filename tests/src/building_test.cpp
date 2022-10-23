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

template<class Bldg>
void primary_producer_test(portable::Resource::Type output_res)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Bldg b = Bldg();
  // Check max production limits
  EXPECT_EQ(1, b.count_remaining_production());
  // Should be immediately ready to produce
  EXPECT_TRUE(b.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, b.produce(cache, transporters, output));
  EXPECT_EQ(1, output.size());
  EXPECT_EQ(0, b.count_remaining_production());
  ASSERT_EQ(portable::Portable::Object::resource,
            output.at(0)->get_object());
  EXPECT_EQ(output_res, static_cast<portable::Resource *>(output.at(0))->get_type());

  // After producing once, shouldn't keep producing this round.
  EXPECT_FALSE(b.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, b.produce(cache, transporters, output));
  EXPECT_EQ(1, output.size());
  EXPECT_EQ(0, b.count_remaining_production());
  ASSERT_EQ(portable::Portable::Object::resource,
            output.at(0)->get_object());
  EXPECT_EQ(output_res, static_cast<portable::Resource *>(output.at(0))->get_type());
}

TEST(building_test, primary_producers)
{
  // Clay pits: produces 1 clay per round
  primary_producer_test<Clay_pit>(portable::Resource::clay);
  // Oil rigs: produces 1 fuel per round
  primary_producer_test<Oil_rig>(portable::Resource::fuel);
  // Quarry: produces 1 stone per round
  primary_producer_test<Quarry>(portable::Resource::stone);
  // Woodcutter: produces 1 trunk per round
  primary_producer_test<Woodcutter>(portable::Resource::trunks);
}

TEST(building_test, mine_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;

  // Default type of mine should be filled with 3 gold, 3 iron
  Mine mine = Mine();
  EXPECT_EQ(3, mine.count(portable::Resource::gold));
  EXPECT_EQ(3, mine.count(portable::Resource::iron));
  // Check max production limits
  EXPECT_EQ(1, mine.count_remaining_production());
  // Should be immediately ready to produce
  EXPECT_TRUE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, mine.produce(cache, transporters, output));
  EXPECT_EQ(1, output.size());
  EXPECT_EQ(0, mine.count_remaining_production());
  ASSERT_EQ(portable::Portable::Object::resource,
            output.at(0)->get_object());
  portable::Resource::Type produced = static_cast<portable::Resource *>(output.at(0))->get_type();
  EXPECT_TRUE((portable::Resource::iron == produced) ||
              (portable::Resource::gold == produced));

  // After producing once, shouldn't keep producing this round.
  EXPECT_FALSE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, mine.produce(cache, transporters, output));
  EXPECT_EQ(1, output.size());
  EXPECT_EQ(0, mine.count_remaining_production());
  ASSERT_EQ(portable::Portable::Object::resource,
            output.at(0)->get_object());
  EXPECT_TRUE((portable::Resource::iron == produced) ||
              (portable::Resource::gold == produced));

  // Regular mines should produce 3 gold & 3 iron over 6 rounds.
  // After that, they can't produce any more.
  // Produce 5 more times to see if mine produces as it should.
  mine.reset(); // This signals a new production round.
  for (uint8_t i = 0; i < 5; i++)
  {
    EXPECT_TRUE(mine.can_produce(cache, transporters));
    EXPECT_EQ(common::ERR_NONE, mine.produce(cache, transporters, output));
    EXPECT_EQ(i + 2, output.size());
    EXPECT_EQ(0, mine.count_remaining_production());
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i+1)->get_object());
    produced = static_cast<portable::Resource *>(output.at(i+1))->get_type();
    EXPECT_TRUE((portable::Resource::iron == produced) ||
                (portable::Resource::gold == produced));
    mine.reset();
  }
  // Now it's produced 6 times, which is the most it should be able to.
  EXPECT_FALSE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, mine.produce(cache, transporters, output));
  EXPECT_EQ(6, output.size());
  
  // We should have 3 gold and 3 iron produced total.
  // Easier to count in a cache...
  cache.add(output);
  output.clear();
  EXPECT_EQ(3, cache.count(portable::Resource::gold));
  EXPECT_EQ(3, cache.count(portable::Resource::iron));
  cache.clear();

  // Big mines are the same as regular mines, except they're 5 gold and 5 iron
  mine = Mine(Mine::Mine_type::big);
  EXPECT_EQ(1, mine.count_remaining_production());
  EXPECT_EQ(5, mine.count(portable::Resource::gold));
  EXPECT_EQ(5, mine.count(portable::Resource::iron));
  for (uint8_t i = 0; i < 10; i++)
  {
    EXPECT_TRUE(mine.can_produce(cache, transporters));
    EXPECT_EQ(common::ERR_NONE, mine.produce(cache, transporters, output));
    EXPECT_EQ(i+1, output.size());
    EXPECT_EQ(0, mine.count_remaining_production());
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i)->get_object());
    produced = static_cast<portable::Resource *>(output.at(i))->get_type();
    EXPECT_TRUE((portable::Resource::iron == produced) ||
                (portable::Resource::gold == produced));
    mine.reset();
  }
  // Now it's produced 10 times, which is the most it should be able to.
  EXPECT_FALSE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, mine.produce(cache, transporters, output));
  EXPECT_EQ(10, output.size());
  
  // We should have 5 gold and 5 iron produced total.
  cache.add(output);
  output.clear();
  EXPECT_EQ(5, cache.count(portable::Resource::gold));
  EXPECT_EQ(5, cache.count(portable::Resource::iron));
  cache.clear();

  // Specialized gold mines should produce 4 gold over 4 turns
  mine = Mine(Mine::Mine_type::specialized_gold);
  EXPECT_EQ(4, mine.count(portable::Resource::gold));
  EXPECT_EQ(0, mine.count(portable::Resource::iron));
  for (uint8_t i = 0; i < 4; i++)
  {
    EXPECT_TRUE(mine.can_produce(cache, transporters));
    EXPECT_EQ(common::ERR_NONE, mine.produce(cache, transporters, output));
    EXPECT_EQ(i + 1, output.size());
    EXPECT_EQ(0, mine.count_remaining_production());
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i)->get_object());
    produced = static_cast<portable::Resource *>(output.at(i))->get_type();
    EXPECT_EQ(portable::Resource::gold, produced);
    mine.reset();
  }
  // Now it's produced 4 times, which is the most it should be able to.
  EXPECT_FALSE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, mine.produce(cache, transporters, output));
  EXPECT_EQ(4, output.size());
  
  // We should have 4 gold produced total.
  cache.add(output);
  output.clear();
  EXPECT_EQ(4, cache.count(portable::Resource::gold));
  cache.clear();

  // Specialized iron mines should produce 4 iron over 4 turns
  mine = Mine(Mine::Mine_type::specialized_iron);
  EXPECT_EQ(0, mine.count(portable::Resource::gold));
  EXPECT_EQ(4, mine.count(portable::Resource::iron));
  for (uint8_t i = 0; i < 4; i++)
  {
    EXPECT_TRUE(mine.can_produce(cache, transporters));
    EXPECT_EQ(common::ERR_NONE, mine.produce(cache, transporters, output));
    EXPECT_EQ(i + 1, output.size());
    EXPECT_EQ(0, mine.count_remaining_production());
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i)->get_object());
    produced = static_cast<portable::Resource *>(output.at(i))->get_type();
    EXPECT_EQ(portable::Resource::iron, produced);
    mine.reset();
  }
  // Now it's produced 4 times, which is the most it should be able to.
  EXPECT_FALSE(mine.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, mine.produce(cache, transporters, output));
  EXPECT_EQ(4, output.size());
  
  // We should have 4 iron produced total.
  cache.add(output);
  output.clear();
  EXPECT_EQ(4, cache.count(portable::Resource::iron));
  cache.clear();
}

template<class Bldg>
void secondary_producer_test(
  std::vector<portable::Resource::Type> input_resources,
  portable::Resource::Type output_res, uint8_t output_per_input,
  uint8_t max_output)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Bldg b = Bldg();
  // Check max production limits
  EXPECT_EQ(max_output, b.count_remaining_production());
  // Shouldn't be able to produce without input resources
  EXPECT_FALSE(b.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, b.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(max_output, b.count_remaining_production());
  // Add requisite resources, plus some extra for testing
  uint8_t to_add = max_output / output_per_input + 1;
  for (uint8_t i = 0; i < to_add; i++)
  {
    for (size_t j = 0; j < input_resources.size(); j++)
    {
      cache.add(input_resources.at(j));
    }
  }

  // With required resources, should be able to produce
  EXPECT_TRUE(b.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, b.produce(cache, transporters, output));
  // Should only produce up to max per round.
  EXPECT_EQ(max_output, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i)->get_object());
    EXPECT_EQ(output_res,
              static_cast<portable::Resource *>(output.at(i))->get_type());
  }
  EXPECT_EQ(0, b.count_remaining_production());
  // Extra resources should be untouched.
  for (size_t i = 0; i < input_resources.size(); i++)
  {
    EXPECT_TRUE(cache.count(input_resources.at(i)) >= 1);
  }
  // All other resources should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  EXPECT_EQ(common::ERR_FAIL, b.produce(cache, transporters, output));
  EXPECT_EQ(0, b.count_remaining_production());
  for (size_t i = 0; i < input_resources.size(); i++)
  {
    EXPECT_TRUE(cache.count(input_resources.at(i)) >= 1);
  }
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  EXPECT_EQ(max_output, output.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    ASSERT_EQ(portable::Portable::Object::resource,
              output.at(i)->get_object());
    EXPECT_EQ(output_res,
              static_cast<portable::Resource *>(output.at(i))->get_type());
  }
}

TEST(building_test, secondary_producers)
{
  std::vector<portable::Resource::Type> input_res;
  
  // Sawmills: 2 boards per 1 trunks input. Max production: 6.
  input_res.push_back(portable::Resource::trunks);
  secondary_producer_test<Sawmill>(
    input_res, portable::Resource::boards, 2, 6);

  // Papermills: 1 paper per 2 trunks input. Max production: 1.
  input_res.push_back(portable::Resource::trunks);
  secondary_producer_test<Papermill>(
    input_res, portable::Resource::paper, 1, 1);
  
  // Coal burners: 1 fuel per 2 trunks input. Max production: 6.
  secondary_producer_test<Coal_burner>(
    input_res, portable::Resource::fuel, 1, 6);

  input_res.clear();

  // Papermills: 1 paper per 2 boards input. Max production: 1.
  input_res.push_back(portable::Resource::boards);
  input_res.push_back(portable::Resource::boards);
  secondary_producer_test<Papermill>(
    input_res, portable::Resource::paper, 1, 1);

  // Coal burners: 1 fuel per 2 boards input. Max production: 6.
  secondary_producer_test<Coal_burner>(
    input_res, portable::Resource::fuel, 1, 6);

  input_res.clear();

  // Stone factories: 2 stone per 1 clay input. Max production: 6.
  input_res.push_back(portable::Resource::clay);
  secondary_producer_test<Stone_factory>(
    input_res, portable::Resource::stone, 2, 6);
  input_res.clear();

  // Mints: 1 coin per 2 gold & 1 fuel input. Max production: 1.
  input_res.push_back(portable::Resource::gold);
  input_res.push_back(portable::Resource::gold);
  input_res.push_back(portable::Resource::fuel);
  secondary_producer_test<Mint>(
    input_res, portable::Resource::coins, 1, 1);
  input_res.clear();

  // Stock exchange: 1 stock per 2 coins & 1 paper input. Max production: 6
  input_res.push_back(portable::Resource::coins);
  input_res.push_back(portable::Resource::coins);
  input_res.push_back(portable::Resource::paper);
  secondary_producer_test<Stock_exchange>(
    input_res, portable::Resource::stock, 1, 6);
}

/// ------- Full Factory testing waiting on Transporter implementation -------
TEST(building_test, raft_factory_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Raft_factory rf = Raft_factory();
  // Raft factories require 2 trunks as input to produce
  EXPECT_EQ(1, rf.count_remaining_production());
  EXPECT_FALSE(rf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, rf.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(1, rf.count_remaining_production());
  // Add requisite resources, plus some extra for testing
  for (uint8_t i = 0; i < 4; i++)
  {
    cache.add(portable::Resource::trunks);
  }

  // With required resources, should be able to produce
  EXPECT_TRUE(rf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, rf.produce(cache, transporters, output));
  // Should only produce up to max per round.
  // TODO: Determine color of transporter based on players using the building
  // EXPECT_EQ(1, output.size());
  // Extra trunks should be untouched.
  EXPECT_EQ(2, cache.count(portable::Resource::trunks));
  // All other resources should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  EXPECT_EQ(0, rf.count_remaining_production());
  EXPECT_EQ(common::ERR_FAIL, rf.produce(cache, transporters, output));
  EXPECT_EQ(0, rf.count_remaining_production());
  EXPECT_EQ(2, cache.count(portable::Resource::trunks));
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
}

TEST(building_test, rowboat_factory_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Rowboat_factory rf = Rowboat_factory();
  // Rowboat factories require 5 boards as input to produce
  EXPECT_EQ(1, rf.count_remaining_production());
  EXPECT_FALSE(rf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, rf.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(1, rf.count_remaining_production());
  // Add requisite resources, plus extra for testing
  for (uint8_t i = 0; i < 10; i++)
  {
    cache.add(portable::Resource::boards);
  }

  // With required resources, should be able to produce
  EXPECT_TRUE(rf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, rf.produce(cache, transporters, output));
  // Should only produce up to max per round.
  // TODO: Determine color of transporter based on players using the factory
  // EXPECT_EQ(1, output.size());
  // Extra boards should be untouched.
  EXPECT_EQ(5, cache.count(portable::Resource::boards));
  // All other resources should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  EXPECT_EQ(0, rf.count_remaining_production());
  EXPECT_EQ(common::ERR_FAIL, rf.produce(cache, transporters, output));
  EXPECT_EQ(0, rf.count_remaining_production());
  EXPECT_EQ(5, cache.count(portable::Resource::boards));
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
}

TEST(building_test, steamer_factory_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Steamer_factory sf = Steamer_factory();
  // Steamer factories require 2 fuel and an iron as input to produce
  EXPECT_EQ(1, sf.count_remaining_production());
  EXPECT_FALSE(sf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, sf.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(1, sf.count_remaining_production());
  // Add requisite resources, plus some extra for testing
  for (uint8_t i = 0; i < 2; i++)
  {
    cache.add(portable::Resource::fuel);
    cache.add(portable::Resource::fuel);
    cache.add(portable::Resource::iron);
  }

  // With required resources, should be able to produce
  EXPECT_TRUE(sf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, sf.produce(cache, transporters, output));
  // Should only produce up to max per round.
  // TODO: Determine transporter color by user of factory
  // EXPECT_EQ(1, output.size());
  // Extra fuel and iron should be untouched.
  EXPECT_EQ(2, cache.count(portable::Resource::fuel));
  EXPECT_EQ(1, cache.count(portable::Resource::iron));
  // All other resources should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  EXPECT_EQ(0, sf.count_remaining_production());
  EXPECT_EQ(common::ERR_FAIL, sf.produce(cache, transporters, output));
  EXPECT_EQ(0, sf.count_remaining_production());
  EXPECT_EQ(2, cache.count(portable::Resource::fuel));
  EXPECT_EQ(1, cache.count(portable::Resource::iron));
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
}

TEST(building_test, truck_factory_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Truck_factory tf = Truck_factory();
  // Truck factories require a fuel and an iron as input to produce
  EXPECT_EQ(1, tf.count_remaining_production());
  EXPECT_FALSE(tf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, tf.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(1, tf.count_remaining_production());
  // Add requisite resources, plus some extra for testing
  for (uint8_t i = 0; i < 2; i++)
  {
    cache.add(portable::Resource::fuel);
    cache.add(portable::Resource::iron);
  }

  // With required resources, should be able to produce
  EXPECT_TRUE(tf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_NONE, tf.produce(cache, transporters, output));
  // Should only produce up to max per round.
  // TODO: Determine transporter color by user of factory
  // EXPECT_EQ(1, output.size());
  // Extra fuel and iron should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::fuel));
  EXPECT_EQ(1, cache.count(portable::Resource::iron));
  // All other resources should be untouched.
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  EXPECT_EQ(0, tf.count_remaining_production());
  EXPECT_EQ(common::ERR_FAIL, tf.produce(cache, transporters, output));
  EXPECT_EQ(0, tf.count_remaining_production());
  EXPECT_EQ(1, cache.count(portable::Resource::fuel));
  EXPECT_EQ(1, cache.count(portable::Resource::iron));
  EXPECT_EQ(1, cache.count(portable::Resource::stock));
}

TEST(building_test, wagon_factory_test)
{
  // Required for checking whether a building can produce.
  std::vector<portable::Transporter*> transporters;
  portable::Cache cache;
  std::vector<portable::Portable *> output;
  // Add extranneous resource not used in production
  cache.add(portable::Resource::stock);

  Wagon_factory wf = Wagon_factory();
  // Wagon factories require 2 boards and a donkey as input to produce
  EXPECT_EQ(1, wf.count_remaining_production());
  EXPECT_FALSE(wf.can_produce(cache, transporters));
  EXPECT_EQ(common::ERR_FAIL, wf.produce(cache, transporters, output));
  EXPECT_EQ(0, output.size());
  EXPECT_EQ(1, wf.count_remaining_production());
  // Add requisite resources, plus some extra for testing
  for (uint8_t i = 0; i < 4; i++)
  {
    cache.add(portable::Resource::boards);
  }

  // With required resources, should be able to produce
  // TODO: How to handle inputting donkeys for production?
  // EXPECT_TRUE(wf.can_produce(cache, transporters));
  // EXPECT_EQ(common::ERR_NONE, wf.produce(cache, transporters, output));
  // Should only produce up to max per round.
  // TODO: Determine transporter color by user of factory
  // EXPECT_EQ(1, output.size());
  // Extra boards should be untouched.
  // EXPECT_EQ(2, cache.count(portable::Resource::boards));
  // All other resources should be untouched.
  // EXPECT_EQ(1, cache.count(portable::Resource::stock));
  
  // Shouldn't keep producing beyond max per round.
  // Even if there are enough resources to do so.
  // EXPECT_EQ(0, wf.count_remaining_production());
  // EXPECT_EQ(common::ERR_FAIL, wf.produce(cache, transporters, output));
  // EXPECT_EQ(0, wf.count_remaining_production());
  // EXPECT_EQ(2, cache.count(portable::Resource::boards));
  // EXPECT_EQ(1, cache.count(portable::Resource::stock));
}
