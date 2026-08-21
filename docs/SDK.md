# Covariant Script SDK

This document describes the embedding (C++) API of Covariant Script and, most
importantly, the **resource ownership contracts** every embedder must respect.
Covariant Script manages all of its memory with deterministic RAII, so object
lifetimes are precise but must be understood.

## Table of Contents

+ [Quick Start](#quick-start)
+ [Ownership Model](#ownership-model)
+ [Resource Contracts](#resource-contracts)
  + [1. Context lifetime](#1-context-lifetime)
  + [2. `current_process` and threading](#2-current_process-and-threading)
  + [3. Structure finalizers](#3-structure-finalizers)
  + [4. `var` lifetime](#4-var-lifetime)
  + [5. Token arena and recompilation](#5-token-arena-and-recompilation)
+ [Migration Guide (ABI 2608xx → ABI 2609xx)](#migration-guide-abi-2608xx--abi-2609xx)
+ [Migration Guide (ABI 2609xx → ABI 2610xx)](#migration-guide-abi-2609xx--abi-2610xx)

---

## Quick Start

The simplest embedding path is `cs::bootstrap`:

```cpp
#include <covscript/covscript.hpp>

int main()
{
    cs::bootstrap env;                       // owns a context
    env.run("script.csc");                   // compile + interpret
    // ... use cs::eval(env.context, "...") or cs::function_invoker ...
}
```

For more control, build a context directly:

```cpp
cs::array args;
args.push_back(cs::var::make<cs::string>("<MY_APP>"));
auto ctx = cs::create_context(args);         // context_t = shared_ptr<context_type>
ctx->instance->compile("script.csc");
ctx->instance->interpret();
```

The central handle is `cs::context_t` (a `std::shared_ptr<cs::context_type>`).
Everything else hangs off it:

| Object | Owned by | Lifetime |
|---|---|---|
| `context_type` | the embedder's `context_t` | until the last `context_t` is dropped |
| `process_context` | the context | context lifetime |
| `instance_type` | the context (`context->instance`) | context lifetime |
| `compiler_type` | the context (`context->compiler`) | context lifetime (shared with subcontexts) |
| token arena (`compile_unit`) | the instance, functions, and struct builders | until the last owner drops |
| `var` values | reference counting | until the last `var` reference is dropped |
| module subcontexts | the context's `subcontexts` pool | context lifetime |

## Ownership Model

Covariant Script removed the historical global garbage collector in favor of
explicit, deterministic ownership:

+ **Statements** form a tree; each parent deletes its children.
+ **Tokens** live in a per-program bump arena (`compile_unit`) that is released
  as one block.
+ **`var`** is a reference-counted, copy-on-write handle to a heap value.
  Copying a `var` is cheap (a refcount bump); `cs::copy(var)` / `var::clone()`
  performs a deep copy.
+ **The context is the runtime root owner** of the process, compiler, instance
  and their children. Script objects hold raw pointers back to their defining
  context; once the context is destroyed, those pointers are all invalidated, and
  using them is undefined behavior.

The consequence is that **each resource is reclaimed when its last owning
reference goes away** — no `collect_garbage()`, no deferred sweep.

## Resource Contracts

These are the rules an embedder must follow. Script objects hold raw pointers
back to their context; once the context is destroyed, those pointers are
invalidated. Using any escaped script object after its context is destroyed is
undefined behavior.

### 1. Context lifetime

**All script objects assume their defining context is alive when used.**

Invoking a script function, structure method, etc. after its defining context
has been destroyed is **undefined behavior** — no check is performed and no
exception is guaranteed.

Therefore:

```cpp
cs::var f;
{
    auto ctx = cs::create_context({...});
    f = cs::eval(ctx, "[](x)->x+1");   // escape a lambda
}                                      // ctx destroyed here
f.const_val<cs::callable>().call(...); // undefined behavior — do not do this
```

Keep the `context_t` alive for as long as you use the object:

```cpp
auto ctx = cs::create_context({...});
cs::var f = cs::eval(ctx, "[](x)->x+1");
// ... use f freely while ctx is alive ...
```

Keep the context alive for as long as you use any escaped object; once the
context is destroyed, escaped objects (including structure member data) are no
longer usable.

### 2. `current_process` and threading

`cs::current_process` is a `process_context_ref` backed by thread-local storage;
it converts to `process_context*` and is `nullptr` unless a run is active on the
thread. `cs::process_run_scope(ctx)` installs `ctx`'s process for the scope.
Nested scopes on the **same** process are transparent; a **different** active
process throws, so a thread cannot run two instances concurrently. Native code
that calls `callable::call()` must hold its own `process_run_scope`.

Consequences:

+ Native (CNI) callbacks and async futures may observe `current_process ==
  nullptr`; the SDK's error paths tolerate this, but an extension that reads
  `current_process` must null-check.
+ Two contexts may run on **different threads** concurrently. The `var`
  proxy allocator pool is **per-thread** (`thread_local`) and fills on demand,
  so separate contexts never share pool slots; values freed on a different
  thread fall back to the direct allocator path (all `std::allocator`
  instances are interchangeable).

An extension DLL that needs its own compilation environment (independent
storage, namespace, or compiled program) should create a **subcontext**
rather than a separate context:

```cpp
// Inside an extension function — host process is active.
auto ctx = cs::create_subcontext(host_ctx); // shares process, owns storage
ctx->instance->compile(in);
ctx->instance->interpret();
// ctx goes out of scope: subcontext destroyed, host unaffected.
```

A subcontext shares its parent's process (so `process_run_scope` is
transparent) and its compiler (bound to the subcontext during compilation),
but owns its own storage and token arena. This is the same mechanism used
for module imports.

### 3. Structure finalizers

A structure's `finalize` method runs when the structure is released **while its
runtime is still alive**. This happens naturally for block-scoped variables
(they go out of scope during `interpret`), but global variables are released
only when the global domain is cleared.

For a script run to completion, the high-level entry points do this for you:

+ `cs::bootstrap::~bootstrap()` and the `cs` CLI call
  `instance->storage.clear_global()` after interpreting.
+ The REPL clears the global domain on exit.

If you drive `create_context` + `compile` + `interpret` manually, finalizers run
during context teardown when no unrelated process is active on that thread.
For deterministic behavior, call `ctx->instance->storage.clear_global()` while
the context's own process is active before releasing it; this is required when
another unrelated process will remain active during destruction.

### 4. `var` lifetime

`cs::var` is a pointer-sized handle (8 bytes on 64-bit platforms); copying it
bumps a reference count, and the value is freed when the last reference drops.
To detach a value from its original storage, use `cs::copy(var)` (deep copy).
Once a value escapes its context, it is no longer usable after the context is
destroyed. A script callable holds a raw function pointer; invoking it after
context destruction is undefined behavior, and the same applies to a
structure's member data and type identity.

### 5. Token arena and recompilation

Each `compile()` produces a program whose tokens live in a per-instance arena.
Compiling again (or calling `release_statements()`) drops the previous program
and its arena.

A script function/lambda registered in the function store keeps that arena
alive via `function::m_unit`, so it remains callable after recompilation —
*provided its context is still alive* (§1). This is the one surviving keep-alive
mechanism, because it concerns the arena rather than the context.

### 6. Runtime diagnostics (`COVSCRIPT_DEBUG`)

Defensive runtime guards (for example destroying an unfinished fiber, a
non-empty function value stack at program entry, or a structure finalizer
that throws during teardown) behave according to the
`COVSCRIPT_DEBUG` environment variable:

| Value | Behavior |
| :-- | :-- |
| `none` | Ignore silently; the program keeps running (partial leaks accepted) |
| `warning` | Print a warning to stderr and continue (default) |
| `strict` | Print a warning and abort (fail-fast) |

The value is case-insensitive; unset or unknown values default to `warning`.
Guards are diagnostic only — they do not change the ownership contracts above.

## Migration Guide (ABI 2608xx → ABI 2609xx)

All extensions must be recompiled.

- `cs::current_process` type changed to `process_context_ref` — usage is
  identical (`->`, `*`, `== nullptr` all work as before).
- `process_context::raise_sigint()` / `raise_exit()` removed — use
  `cs::global_signals.raise_sigint()` / `raise_exit()` instead.
- DLL entry point signature changed — recompile against the new headers.
- `COVSCRIPT_DEBUG` environment variable controls runtime diagnostic level
  (`none` / `warning` / `strict`).
- `process_context::on_process_sigint` default handler changed from exit to
  ignore (no-op). Embedders who want Ctrl+C to terminate should add a
  listener: `ctx->on_process_sigint.add_listener([](void*) -> bool { /* shutdown */ return true; })`.
- Signal flags are OS-process-global (`cs::global_signals`). With multiple
  embedded processes running concurrently, the first process to poll consumes
  the pending signal; a signal raised while nothing is polling stays pending
  until the next poll.
- `cs::invoke` on a non-callable now throws `cs::lang_error` (previously
  `cs::runtime_error`) to match script-level semantics. Note that
  `cs::lang_error` does not derive from `std::exception`.
- `process_activation` removed. Callers of `callable::call()` from native
  code must now hold their own `process_run_scope`. Extension DLLs share
  the host's thread-local slot via the accessor — reads and writes are
  unified, no manual accessor management needed.

### New APIs

- `cs::process_run_scope` — RAII guard for `current_process` management.
- `cs::global_signals` — global signal control (replaces the removed
  `process_context::raise_sigint/exit`).
- `cs::fiber::schedule_parameters` / `get_schedule_parameters()` /
  `set_schedule_parameters()` — tune fiber backoff.
- `cs::callable::argument_count()` — query function arity.
- `cs::create_context` accepts an optional `stack_size` parameter.

## Migration Guide (ABI 2609xx → ABI 2610xx)

All extensions must be recompiled.

### Breaking changes

- **`function_ptr` no longer holds an `owner`**. It was
  `{function*, shared_ptr<function>}`, now just `{function*}`. Code that
  constructed `function_ptr(f, owner)` or accessed `.owner` must be updated.
- **`contains_callable` family removed**. `callable_contains_function`,
  `callable_is_member_function`, and similar helpers that probed whether a
  callable was backed by a script function are deleted.
- **Escape behavior: UB instead of throwing**. Using an escaped script object
  (function, structure method, type constructor, fiber, etc.) after its context
  is destroyed no longer throws `runtime_error` — it is undefined behavior.
  Code that catches such exceptions will no longer trigger.
- **`function::get_context()` returns a raw pointer**. Previously returned
  `shared_ptr<context_type>`, now returns `context_type*`.
- **`process_context::teardown_ctx()` removed**.
- **`structure::m_process` changed to a raw pointer**. Previously
  `shared_ptr<process_context>`, now `process_context*`.
- **Named functions registered at compile time**. `statement_function::mFunc`
  changed from `unique_ptr<function>` to `function*` (owned by the
  `function_store`).

### New APIs

- `cs::invoke(func, args...)` — unified callable invocation, replacing
  `func.val<cs::callable>().call(args)`.
