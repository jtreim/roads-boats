#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Color.h>
#include <portables/Portable.h>
#include <portables/resources/Resource.h>

TEST(resource_test, create_resource_test)
{
  // The empty constructor should result in a resource that is invalid.
  portable::Resource test;
  EXPECT_EQ(portable::Resource::Type::invalid, test.get_type());
  // When creating a resource, it should always be have portable type "resource"
  EXPECT_EQ(portable::Portable::Object::resource, test.get_object());
  // Resources should start with an empty list of carriers
  EXPECT_EQ(0, test.get_carriers().size());

  // Initializing with params
  test = portable::Resource(portable::Resource::Type::trunks);
  EXPECT_EQ(portable::Resource::Type::trunks, test.get_type());
  EXPECT_EQ(portable::Portable::Object::resource, test.get_object());
  EXPECT_EQ(0, test.get_carriers().size());

  std::set<player::Color> carriers;
  carriers.insert(player::Color::blue);
  test = portable::Resource(portable::Resource::Type::gold, carriers);
  EXPECT_EQ(portable::Resource::Type::gold, test.get_type());
  EXPECT_EQ(portable::Portable::Object::resource, test.get_object());
  EXPECT_EQ(carriers, test.get_carriers());

  // Any invalid carriers should be removed from the list
  std::set<player::Color> invalid_carriers;
  invalid_carriers.insert(player::Color::invalid);
  invalid_carriers.insert(player::Color::neutral);
  test = portable::Resource(portable::Resource::Type::stock, invalid_carriers);
  EXPECT_EQ(portable::Resource::Type::stock, test.get_type());
  EXPECT_EQ(portable::Portable::Object::resource, test.get_object());
  EXPECT_EQ(0, test.get_carriers().size());
}