# Roads & Boats
The goal: make a working executable of the Splotter board game **Roads and Boats**

See references for [condensed rules](https://github.com/jtreim/roads-boats/blob/main/Roads_-_Rules_Condensed.pdf), [full rulebook](https://github.com/jtreim/roads-boats/blob/main/RB3Erules.pdf), and [player aids](https://github.com/jtreim/roads-boats/blob/main/RB_PlayerAid_Color_ENG_V4_0.pdf) for more info about the game.

## Milestones
### Base functionality
- [ ] A valid map with land and sea tiles can be defined.
  - [ ] Rivers flow naturally across land and into sea tiles.
  - [ ] Tiles can be organized into a map structure.
  - [ ] A map can be printed as a JSON structure.
- [ ] Resources can exist on a map.
  - [ ] Resource types are defined.
  - [ ] Resources can be printed as JSON inside a map's JSON structure.
- [ ] Buildings can exist on a map.
  - [ ] Buildings are restricted to existing on their respective tile types.
  - [ ] Buildings can receive valid input goods and produce accurate outputs.
  - [ ] Buildings can be printed as JSON inside a map's JSON structure.
- [ ] Transporters can interact with the map.
  - [ ] Transporters can move from tile to tile.
  - [ ] Transporters can pickup, carry, and drop off resources on tiles.
  - [ ] Transporters can use nearby resources to construct a valid building.
  - [ ] Transporters can feed resources they are carrying to buildings.
  - [ ] Transporters can be printed as JSON inside a map's JSON structure.

### Gameplay
- [ ] Game states transition as expected
  - [ ] Player lobby -> Game start
  - [ ] Game start -> Map setup
  - [ ] Map setup -> Gameplay
    - [ ] Movement phase -> Build phase
    - [ ] Build phase -> Wonder brick phase
    - [ ] Wonder brick phasae -> Production phase
    - [ ] Production phase -> Movement phase
  - [ ] Gameplay -> End game
  - [ ] End game -> Player lobby
