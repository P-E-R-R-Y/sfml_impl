# sfml_impl — changelog

Markers: 🟢 added · 🔴 breaking · 🔵 fix · ⚪ internal or docs · 🟡 proposed
in the plan, no code written yet.

## v0.1.0

- 🟢 fills `graphic2` + `audio`
- 🟢 single entry point `getModules()`
- 🟢 SHARED and STATIC targets (STATIC recompiles `sources/sfml.cpp` for
  a consumer that doesn't need it — known, not fixed)

### Measured failure, not fixed

- 🔴 `createTexture`/`createSoundBuffer` etc. return a dead object
  (`isReady()==false`) instead of `nullptr` when loading fails.
  `createSprite`/`createSound` on a dead object throw `std::bad_optional_access`: `SfmlTexture`/`SfmlSoundBuffer` store a `std::optional` that's never checked

## Proposed, not written

- 🟡 `claims()`: `{"opengl"}` — sfml and raylib excluded without naming each other
- 🟡 STATIC target → INTERFACE, zero object code
