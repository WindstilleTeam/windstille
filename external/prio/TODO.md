# TODO

## Unambiguous sexpr-like syntax (for linearize / a future text format)

### The problem

prio's data model has three different compound shapes:

| Shape | Meaning |
|-------|---------|
| **Object** | A *named* node with a property mapping (`ReaderObject`) |
| **Mapping** | A bag of key → value pairs (`ReaderMapping`) |
| **Collection** | An ordered list of objects (`ReaderCollection`) |

Classic sexpr uses `(…)` for almost everything:

```sexpr
(server
  (host "example.com")
  (port 8080)
  (tls
    (enabled #t)
    (certificate "cert.pem"))
  (items
    (wall (texture "brick.png"))
    (floor (texture "wood.png"))))
```

Here `(tls …)` and `(items …)` look the same, but one is a nested
mapping of properties and the other is a collection of named objects.
Whether `(wall (texture …))` is “a mapping key `wall`” or “an object
named `wall`” is not visible in the parentheses alone — the reader
decides by *how* the value is requested (`read(key, mapping)` vs
`read(key, collection)` vs `read(key, object)`).

That is fine for the C++ API. It is not fine for a greppable
path = value dump, or for a text format that should be readable without
the API's intent.

**Constraint:** avoid lots of visual noise. Most documents are mostly
mappings of scalars; any scheme that marks *every* key or *every* map
taxes the common case for the sake of rare shapes.

**Requirement for a “full” solution:** unambiguous from the syntax
alone, even if that means **not** being plain sexpr anymore.

---

### Is “only mark collections” enough?

Idea: leave property trees as today’s sexpr; use `[]` only for
collections.

```sexpr
(server
  (host "example.com")
  (tls
    (enabled #t))
  (items
    [ (wall (texture "brick.png"))
      (floor (texture "wood.png")) ]))
```

| Case | Solved? |
|------|---------|
| Collection vs nested mapping | **Yes** — `[]` vs `(key …)` |
| Empty collection | **Yes** — `[]` |
| Common scalar properties | **Yes** — no extra noise |
| Nested mapping vs **single object under a key** | **No** |
| Object name vs mapping key (lexically) | **No** — both are symbols |
| Empty mapping | Convention only (omit key / empty form) |

Example that stays ambiguous without further marks:

```sexpr
(submap (nested (a 1) (b 2)))           ; mapping with one nested key?
(object (realthing (prop1 5) (prop2 7))) ; one named object?
```

Same shape: one child list headed by a symbol. So **marking only
collections is a strong, low-noise improvement, not a complete
solution.**

Linearize can still use `[N]` when the source used a collection:

```text
server.host = example.com
server.tls.enabled = true
server.items[0].wall.texture = brick.png
```

`[0]` is honest; `wall` vs mapping key remains soft unless paths gain
another marker.

---

### Brainstorm: low-noise levers (and their cost)

#### 1. `[]` only for collections (minimum change)

- **Noise:** only on collections.
- **Pros:** small diff from today’s sexpr; empty collection is clear.
- **Cons:** object-under-key vs nested mapping still ambiguous.
- **Verdict:** best first step if we optimize for little noise.

#### 2. Keywords for mapping keys (`:host`)

```sexpr
(server
  (:host "example.com")
  (:tls
    (:enabled #t))
  (:items
    [ (wall (:texture "brick.png")) ]))
```

- **Rule of thumb:** keyword head → mapping entry; symbol head → object name.
- **Pros:** key vs object name is explicit; familiar from Scheme/Clojure.
- **Cons:** **every** property pays a `:`; high noise on the common case.
- **Empty collection vs empty mapping:** still needs `[]` or a convention.
- **Verdict:** powerful but noisy if applied globally. Optional hybrid:
  keywords only where needed is inconsistent and hard to teach.

#### 3. Full three-delimiter split: `()` / `{}` / `[]`

```sexpr
(server {
  host "example.com"
  tls { enabled #t }
  items [ (wall { texture "brick.png" }) ]
})
```

- **Pros:** fully unambiguous; linearize can mirror
  `(server){host}`, `(server){items}[0](wall){texture}`.
- **Cons:** braces on every mapping level — noticeable noise.
- **Verdict:** right answer if we accept a new dialect and drop plain
  sexpr compatibility.

#### 4. Scheme dotted pairs `( key . value )`

```sexpr
(server
  (host . "example.com")
  (tls . ((enabled . #t)
          (certificate . "cert.pem")))
  (items . [ (wall . ((texture . "brick.png"))) ]))
```

- **Pros:** already in Scheme; pairs are not “just another list”; a
  true alist is a list of pairs, which is a known pattern.
- **Cons:**
  - Very noisy: a `.` on every association.
  - Nested alists become hard to read quickly.
  - Collections still need a separate rule (`[]` or list of objects).
  - Improper lists interact poorly with “rest is property list” habits.
- **Verdict:** theoretically nice, practically too much punctuation for
  config-like files.

#### 5. Quoting (`'host` or `' (wall …)`)

- **Pros:** Scheme already has quote; could mark “this is data, not a
  call” or “this is a key.”
- **Cons:** quote means something else to Lisp readers; easy to confuse
  with strings; does not by itself separate mapping vs collection vs
  object without a second convention.
- **Verdict:** poor fit as the primary disambiguator; at best a minor
  tool for awkward symbols.

#### 6. String vs symbol abuse

```sexpr
(server
  ("host" "example.com")      ; string key → mapping entry?
  (wall ("texture" "brick"))  ; symbol head → object?
)
```

or the inverse (symbols for keys, strings for object type names).

- **Pros:** no new delimiters; only a lexical category shift.
- **Cons:**
  - **Abusive:** strings stop meaning “text values” only; keys that look
    like words become quoted noise (`"host"` everywhere).
  - Object names as strings (`"wall"`) fight the usual “type is a symbol”
    intuition.
  - Values that are strings are already `"…"`; overloading strings for
    structure is confusing when reading.
  - Homogeneous string arrays vs list of keys become murkier.
- **Verdict:** low *new* syntax noise, high *semantic* noise. Prefer not
  to overload strings.

#### 7. Hybrid: quiet default + rare marks

Goal: pay noise only where shapes compete.

| Construct | Syntax | Noise |
|-----------|--------|--------|
| Object | `(name …)` as now | none |
| Scalar / nested mapping properties | `(key value)` as now | none |
| Collection | `[ (obj …) … ]` | only here |
| Object under a key (rare) | keep `(key (Name …))` with a documented rule, **or** a rare marker e.g. `(key #(Name …))` / `(key @Name …)` only in that position | rare |

Optional refinement for the remaining object-vs-mapping hole without
taxing every key:

- **Arity / structure heuristic (no new syntax):**  
  - one symbol-headed child whose *rest* is property-like → object  
  - several `(key atom)` children → mapping  
  - fragile when a mapping has a single nested mapping value
- **Tagged object only under keys:**  
  `(object # (realthing (prop1 5)))` or `(object (realthing . props))`  
  still rare if collections already use `[]`.

#### 8. Linearize-only dialects (dump, not document format)

If the goal is grep, not a new file format:

- Paths with `[N]` for collections (from `[]` in source or from the API).
- Avoid global `@` on every object name if possible; prefer structure that
  was already explicit in the file.
- A linearize-only grammar can be stricter than the writer (e.g. always
  emit `(name)` / `{key}` / `[N]`) without forcing authors to type that
  in source files — but then dump and source dialect diverge.

---

### Comparison (noise vs unambiguity)

| Approach | Noise | Collection vs map | Object vs map under key | Notes |
|----------|-------|-------------------|-------------------------|--------|
| Status quo sexpr | none | no | no | API decides |
| `[]` for collections only | low | yes | no | Best low-noise step |
| Keywords on all keys | high | needs `[]` too | yes (with rules) | Noisy common case |
| `()` / `{}` / `[]` | medium–high | yes | yes | Clean model, new dialect |
| Dotted pairs | high | needs extra rule | partial | Scheme-native, heavy |
| Quote | medium | no | weak | Wrong tool |
| String-as-key / string-as-name | medium | weak | forced | Overloads strings |
| Hybrid quiet + `[]` + rare object tag | low | yes | yes if tag used | Preferable compromise |

---

### Suggested direction

1. **Short term:** treat “`[]` for collections only” as the low-noise
   improvement worth prototyping; document the remaining object-under-key
   ambiguity as known.
2. **If full unambiguity is required:** prefer explicit
   `(name { … })` / `{ … }` / `[ … ]` over keywords-everywhere or
   dotted-pairs-everywhere — more noise than `[]`-only, but structured
   rather than a tax on every identifier.
3. **Avoid:** string/symbol abuse and quote-as-structure; **be wary of**
   global keywords and global dotted pairs (noise).
4. **Linearize:** follow whatever the document form marks; do not invent
   a third path dialect heavier than the source language.

---

### Possible next steps

- [ ] Decide dump-only linearize vs a real alternate sexpr dialect.
- [ ] Prototype `[]` for collections in the sexpr writer/reader (opt-in).
- [ ] If needed, prototype `{ }` for mappings or a rare object-under-key tag.
- [ ] Align `priotool --linearize` path segments with the chosen marks.
- [ ] Keep classic sexpr as default input; new syntax opt-in
      (`Format::…` / flag).

