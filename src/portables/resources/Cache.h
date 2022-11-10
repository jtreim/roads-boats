#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Color.h>
#include <portables/resources/Resource.h>

namespace portable
{

/// Class to manage resources left in a tile's area. This does not include any
/// resources that are being carried by transporters in that area, nor resources
/// left on a building.
///
/// Resource amounts are organized as follows:
/// There are lists of resources for each player, along with a neutral pile. At
/// the start of a move phase, all resources should be shifted into the "neutral
/// pile" (call the reset function). Any transporter can pickup these resources.
/// Whenever a transporter drops a resource in an area, that resource can no
/// longer be moved by any other transporter owned by that player for that
/// movement phase. To enforce this, players drop resources into a pile labeled
/// with their own color. Players can move resources from other players' piles
/// in the same movement phase.
///
/// Because this does not represent resource amounts held by transporters, all
/// resources can be used during the build and wonder brick phases.
///
/// Addition operators added to support Area addition.
class Cache
{
public:
  Cache();
  Cache(const Cache &other);
  ~Cache();

  bool operator==(Cache &other);
  bool operator==(Cache const &other) const;
  bool operator!=(Cache &other);
  bool operator!=(Cache const &other) const;
  Cache operator=(const Cache &other);
  Cache operator+(const Cache &other);
  Cache operator+(const Cache &other) const;
  Cache operator+(const std::vector<Resource *> &res_list);
  Cache operator+(const std::vector<Resource *> &res_list) const;
  void operator+=(const Cache &other);
  void operator+=(const std::vector<Resource *> &res_list);

  /// Removes all resources from the cache
  void clear();

  /// Resets all resources for a new round. Maintains resource amounts.
  void reset();

  /// Clears out any empty pointers or empty lists from cache.
  void clean();

  /// Returns the total resource amount
  /// @param[in] res Resource to check
  /// @return A read of the total amount of the input resource in the cache
  uint16_t count(const Resource::Type res) const;

  /// Returns the resource amount the player can move
  /// @param[in] res Resource to check
  /// @param[in] player Requesting player
  /// @return A read of the amount of the input resource the player can move
  uint16_t count_moveable(const Resource::Type res,
                          const player::Color player) const;

  /// Returns a list of all the resources in the cache
  // TODO: does it make more sense to use a vector or a map for viewing all
  // resources in a cache? std::map<Resource::Type, std::vector<Resource *>>
  // all() const;
  std::vector<Resource *> all() const;

  /// Returns a list of all moveable resources in the cache
  /// @param[in] p  Player color requesting list of resources
  /// @return  A list of all resources that can be moved by the input player
  std::vector<Resource *> all_moveable(const player::Color p) const;

  /// Adds resource to the cache
  /// @param[in] res  Resource to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource
  common::Error add(Resource *&res);

  /// Adds resource to the cache
  /// @param[in] res  Resource to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource
  common::Error add(const Resource::Type res);

  /// Adds resources to the cache
  /// @param[in] res_list  Resources to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource
  common::Error add(std::vector<Resource *> &res_list);

  /// Adds resources to the cache
  /// @param[in] res_list  Resources to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure to add anything
  common::Error add(std::vector<Portable *> &res_list);

  /// Removes resource from the cache
  /// @param[in] res  Resource to remove
  /// @param[in] amount Amount of resource to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource
  ///   - common::ERR_FAIL if insufficient resource amount to remove
  ///   - common::ERR_UNKNOWN on any other error
  common::Error remove(const Resource::Type res, const uint16_t amount = 1);

  /// Retrieves the specified amount of the input resource from the cache.
  /// Retrieved resources are removed from the cache.
  /// @param[in] res  The type of resource to remove
  /// @param[out] result  The resulting list of removed resources
  /// @param[in] amount  The amount to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on insufficient resources in cache
  ///   - common::ERR_INVALID on invalid resource type requested
  common::Error get(const Resource::Type res,
                    std::vector<std::unique_ptr<Resource>> &result,
                    const uint16_t amount);

  /// Retrieves the specified amount of the input resource from the cache.
  /// Retrieved resources are removed from the cache.
  /// @param[in] res  The type of resource to remove
  /// @param[in] clr  The color of player requesting the resources
  /// @param[out] result  The resulting list of removed resources
  /// @param[in] amount  The amount to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on insufficient resources in cache
  ///   - common::ERR_INVALID on invalid resource type requested
  common::Error get(const Resource::Type res, const player::Color clr,
                    std::vector<std::unique_ptr<Resource>> &result,
                    const uint16_t amount);

  /// Returns a total count of all resources in the cache
  uint32_t size() const;

  friend std::ostream &operator<<(std::ostream &os, Cache const &res_cache);
  friend void to_json(nlohmann::json &j, const Cache &res_cache);
  friend void from_json(const nlohmann::json &j, Cache &res_cache);

protected:
private:
  std::map<Resource::Type, std::vector<std::unique_ptr<Resource>>> m_resources;
};
}; // namespace portable

#endif