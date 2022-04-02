#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>

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
class Resource_cache
{
public:
  Resource_cache();
  Resource_cache(const Resource_cache &other);
  ~Resource_cache();

  bool operator==(Resource_cache &other);
  bool operator==(Resource_cache const &other) const;
  bool operator!=(Resource_cache &other);
  bool operator!=(Resource_cache const &other) const;
  Resource_cache operator=(const Resource_cache &other);
  Resource_cache operator+(const Resource_cache &other);
  Resource_cache operator+(const Resource_cache &other) const;
  Resource_cache operator+(const std::map<Resource, uint16_t> &res_list);
  Resource_cache operator+(const std::map<Resource, uint16_t> &res_list) const;
  void operator+=(Resource_cache const &other);
  void operator+=(std::map<Resource, uint16_t> const &res_list);

  /// Removes all resources from the cache
  void clear();

  /// Resets all resources for a new round. Maintains resource amounts.
  void reset();

  /// Returns the total resource amount
  /// @param[in] res Resource to check
  /// @return A read of the total amount of the input resource in the cache
  uint16_t get(const Resource res) const;

  /// Returns the resource amount the player can move
  /// @param[in] res Resource to check
  /// @param[in] player Requesting player
  /// @return A read of the amount of the input resource the player can move
  uint16_t get_moveable(const Resource res, const player::Color player) const;

  /// Returns a list of all the resources in the cache
  std::map<Resource, uint16_t> all() const;

  /// Returns a list of all moveable resources in the cache
  /// @param[in] p  Player color requesting list of resources
  /// @return  A list of all resources that can be moved by the input player
  std::map<Resource, uint16_t> all_moveable(const player::Color p) const;

  /// Adds resource to the cache
  /// @param[in] res Resource to add
  /// @param[in] player Last player's color that moved the resource
  /// @param[in] amount Amount of resource to add.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource or player color specified
  common::Error add(const Resource res, const player::Color player,
                    const uint16_t amount = 1);

  /// Removes resource from the cache
  /// @param[in] res Resource to remove
  /// @param[in] amount Amount of resource to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource
  ///   - common::ERR_FAIL if insufficient resource amount to remove
  ///   - common::ERR_UNKNOWN on any other error
  common::Error remove(const Resource res, const uint16_t amount = 1);

  /// Removes the resource from the cache to be moved by player transporter
  /// @param[in] res Resource to be moved
  /// @param[in] player Player owning the transporter to move resource
  /// @param[in] amount Amount of the resource to be moved
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource or player color
  ///   - common::ERR_FAIL on insufficient resource that is moveable
  common::Error move(const Resource res, const player::Color player,
                     const uint16_t amount = 1);

  /// Returns a total for all resources in the cache
  uint32_t size() const;

  friend std::ostream &operator<<(std::ostream &os,
                                  Resource_cache const &res_cache);
  friend void to_json(nlohmann::json &j, const Resource_cache &res_cache);
  friend void from_json(const nlohmann::json &j, Resource_cache &res_cache);

protected:
private:
  uint16_t m_resources[player::MAX_COLORS][RESOURCE_NAMES_SIZE];
};
}; // namespace portable

#endif