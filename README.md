# Roads & Boats
The goal: make a working executable of the Splotter board game **Roads and Boats** in C++.

Code formatting should follow the .clang-format rules found in the repo.

See references for [condensed rules](https://github.com/jtreim/roads-boats/blob/main/misc/rules_condensed.pdf), [full rulebook](https://github.com/jtreim/roads-boats/blob/main/misc/rules.pdf), and [player aids](https://github.com/jtreim/roads-boats/blob/main/misc/player_aid.pdf) for more info about the game.

See [expansion rules](https://github.com/jtreim/roads-boats/blob/main/misc/etcetera.pdf) for any additional rules not in the base game.

## Milestones
### Base functionality
- [X] A valid map with land and sea tiles can be defined.
  - [x] Rivers flow naturally across land and into sea tiles.
  - [X] Tiles can be organized into a map structure.
  - [X] A map can be printed as a JSON structure.
  - [ ] A map can be loaded from a JSON structure.
- [X] Resources can exist on a map.
  - [X] Resource types are defined.
  - [X] Resources can be printed as JSON inside a map's JSON structure.
- [ ] Buildings can exist on a map.
  - [X] Buildings are restricted to existing on their respective tile types.
  - [X] Buildings can receive valid input goods and produce accurate outputs.
  - [ ] Buildings can be printed as JSON inside a map's JSON structure.
- [ ] Transporters can interact with the map.
  - [ ] Transporters can move from tile to tile.
  - [ ] Transporters can pickup, carry, and drop off resources on tiles.
  - [ ] Transporters can use nearby resources to construct a valid building.
  - [ ] Transporters can feed resources they are carrying to buildings.
  - [ ] Transporters can be printed as JSON inside a map's JSON structure.

### Gameplay
- [ ] Game states transition as expected
  - [ ] Player lobby: Player count is determined, colors are decided, game is ready to start.
  - [ ] Game start: Assets are loaded.
  - [ ] Map setup: Players either select a premade map, or create a valid map of their own.
  - [ ] Gameplay: Players given interact as defined by the rulebook.
    - [ ] Movement phase: Players simultaneously move their transporters.
    - [ ] Build phase: Players place buildings using their transporters and resources available.
    - [ ] Wonder brick phasae: Players contribute wonder bricks using resources from their home.
    - [ ] Production phase: Resources/livestock added to the map.
  - [ ] End game: Game is finished as defined by the rules. A winner is declared and players return to the lobby.

## Required software for development
- git
- g++
- make
