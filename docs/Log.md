# sfml_impl — journal

Marqueurs : 🟢 ajout · 🔴 rupture · 🔵 correctif · ⚪ interne ou doc · 🟡 propose
dans le plan, code non ecrit.

## v0.1.0

- 🟢 remplit `graphic2` + `audio`
- 🟢 point d'entree unique `getModules()`
- 🟢 cibles SHARED et STATIC (la STATIC recompile `sources/sfml.cpp` pour
  un consommateur qui n'en a pas besoin — constate, pas corrige)

### Faille mesuree, pas corrigee

- 🔴 `createTexture`/`createSoundBuffer` etc. rendent un objet mort
  (`isReady()==false`) au lieu de `nullptr` quand le chargement echoue.
  `createSprite`/`createSound` sur un objet mort jette `std::bad_optional_access` : `SfmlTexture`/`SfmlSoundBuffer` stockent un `std::optional` jamais verifie

## Propose, pas ecrit

- 🟡 `claims()` : `{"opengl"}` — sfml et raylib exclus sans se nommer
- 🟡 cible STATIC → INTERFACE, zero object code
