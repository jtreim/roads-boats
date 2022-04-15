#include <gtest/gtest.h>

#include <common/Errors.h>

TEST(base_test, generic)
{
  ASSERT_TRUE(true);
  ASSERT_EQ(common::Error::ERR_INVALID, -2);
}