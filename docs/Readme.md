# sfml_impl

version: 0.1.0

[![Docs](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://p-e-r-r-y.github.io/sfml_impl)

> SFML 3.0.2 behind [igraphic](https://github.com/P-E-R-R-Y/igraphic) and
> [iaudio](https://github.com/P-E-R-R-Y/iaudio).

Exports `getGraphic2Module()` and `getAudioModule()`. **No
`getGraphic3Module`** — sfml has no 3D, so the symbol is absent rather than
returning something broken. `Get<IGraphic3Module>("sfml")` is `nullptr`.

## Two ways to take it

| target | artefact | use |
|---|---|---|
| `sfml_impl` | `sfml_impl.dylib` | the plugin, what `ModuleManager::Load()` opens |
| `sfml_impl_static` | `libsfml_impl.a` | linked in, brings its include dirs |

Neither follows `BUILD_SHARED_LIBS` : a plugin turned into a `.a` would fail
at runtime with an obscure message. Pick by target name.

## Examples

```bash
build/sfml_impl_demo_dynamic    # dlopen at runtime
build/sfml_impl_demo_static     # linked in, no dlopen at all
```

`dynamic` takes a dll path in argv[1], so the same binary runs any vendor —
including raylib, which it has never seen :

```bash
build/sfml_impl_demo_dynamic ../../raylib_impl/build/raylib_impl.dylib ../../raylib_impl/assets
```

## What sfml forced

- **Input comes from events, never `sf::Keyboard::isKeyPressed()`.** That
  query is global and needs macOS *Input Monitoring*, without which it
  silently returns nothing.
- `setKeyRepeatEnabled(false)`, else a held key re-fires `isKeyPressed`.
- `FocusLost` clears every key, else one stays stuck after `cmd+tab`.
- `sf::Joystick` has raw numbered buttons with no guaranteed meaning across
  controllers, unlike raylib's mapping. Uses the common Xbox ordering.
- Volume is 0-100 natively, `[0, 1]` in the contract.
- Positional audio is real here (OpenAL). Velocity has no API, so it stays a
  plain accessor.

## Tests

`tests/TestSfmlImpl.cpp` loads the built dll through `ModuleManager` like a
real game would, checks `IGraphic3Module` is `nullptr`, then exercises every
2D and audio contract against real assets.
