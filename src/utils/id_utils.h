#ifndef ID_UTILS_H
#define ID_UTILS_H

#include <stduuid/include/uuid.h>

namespace utils
{

/// Generates a new UUID
/// @return The new UUID
uuids::uuid gen_uuid()
{
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  return gen();
};
} // namespace utils

#endif