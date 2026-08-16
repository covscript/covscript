# Covariant Script SDK

This document describes the embedding (C++) API of Covariant Script and, most
importantly, the **resource ownership contracts** every embedder must respect.
Covariant Script manages all of its memory with deterministic RAII — there is no
garbage collector — so object lifetimes are precise but must be understood.

## Table of Contents

+ [Quick Start](#quick-start)
+ [Ownership Model](#ownership-model)
+ [Resource Contracts](#resource-contracts)
  + [1. Context lifetime (escaped objects)](#1-context-lifetime-escaped-objects)
  + [2. `current_process` and threading](#2-current_process-and-threading)
  + [3. Structure finalizers](#3-structure-finalizers)
  + [4. `var` lifetime](#4-var-lifetime)
  + [5. Token arena and recompilation](#5-token-arena-and-recompilation)

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
| `process_context` | the context (`context->process`) | context lifetime (forked fibers keep a parent alive) |
| `instance_type` | the context (`context->instance`) | context lifetime |
| `compiler_type` | the context (`context->compiler`) | context lifetime (shared with subcontexts) |
| token arena (`compile_unit`) | the instance | until recompiled or the instance dies |
| `var` values | reference counting | until the last `var` reference is dropped |
| module subcontexts | the context's `subcontexts` pool | context lifetime |

## Ownership Model

Covariant Script removed the historical global garbage collector in favour of
explicit, deterministic ownership:

+ **Statements** form a tree; each parent deletes its children.
+ **Tokens** live in a per-program bump arena (`compile_unit`) that is released
  as one block.
+ **`var`** is a reference-counted, copy-on-write handle to a heap value.
  Copying a `var` is cheap (a refcount bump); `cs::copy(var)` / `var::clone()`
  performs a deep copy.
+ **The context is the single owner** of the process, compiler, instance and
  their children. Back-references from those children to the context are raw
  (non-owning) pointers, so releasing the last external `context_t` reclaims the
  whole tree with no cycle.

The consequence is that **everything is reclaimed exactly when the last owning
reference goes away** — no `collect_garbage()`, no deferred sweep.

## Resource Contracts

These are the rules an embedder must follow. Violating them is undefined
behaviour (typically use-after-free).

### 1. Context lifetime (escaped objects)

**An escaped object requires its defining context to stay alive.**

Objects returned by the runtime — script functions, lambdas, `structure`
instances, and module namespaces — do **not** own their context. They hold
non-owning back-references into it:

+ a script function / lambda stores a raw `context_type*` (`function::mContext`);
+ a `structure` pins its owning process (so its type identity node and member
  data stay valid), but its *methods* are script functions and still hold the
  raw `function::mContext` back-reference — invoking them requires the context
  to be alive (they throw "the function's context has been destroyed" otherwise).

Therefore:

```cpp
cs::var f;
{
    auto ctx = cs::create_context({...});
    f = cs::eval(ctx, "[](x)->x+1");   // escape a lambda
}                                      // ctx destroyed here
f.const_val<cs::callable>().call(...); // UNDEFINED BEHAVIOUR
```

Keep the `context_t` alive for as long as you use the object:

```cpp
auto ctx = cs::create_context({...});
cs::var f = cs::eval(ctx, "[](x)->x+1");
// ... use f freely while ctx is alive ...
```

This contract is uniform across functions, lambdas, structures and module
namespaces. (No other language runtime guarantees an escaped value outlives its
defining realm either; Covariant Script simply makes the requirement explicit.)

### 2. `current_process` and threading

`cs::current_process` is a `thread_local process_context*`, `nullptr` unless a
run is active on this thread. It is installed by two RAII guards:

+ `cs::process_run_scope(ctx)` — installs `ctx`'s process for the scope. Nested
  scopes on the **same** process are transparent; a **different** active process
  throws, so a thread cannot run two instances concurrently.
+ `cs::process_activation(ctx)` — activates the process only if none is active
  (used for bare native calls and async worker threads).

Consequences:

+ Native (CNI) callbacks and async futures may observe `current_process ==
  nullptr`; the SDK's error paths tolerate this, but an extension that reads
  `current_process` must null-check.
+ Two contexts may run on **different threads** concurrently. The `var`
  proxy allocator pool is **per-thread** (`thread_local`) and fills on demand,
  so separate contexts never share pool slots; values freed on a different
  thread fall back to the direct allocator path (all `std::allocator`
  instances are interchangeable).

### 3. Structure finalizers

A structure's `finalize` method runs when the structure is released **while its
runtime is still alive**. This happens naturally for block-scoped variables
(they go out of scope during `interpret`), but global variables are released
only when the global domain is cleared.

For a script run to completion, the high-level entry points do this for you:

+ `cs::bootstrap::~bootstrap()` and the `cs` CLI call
  `instance->storage.clear_global()` after interpreting.
+ The REPL clears the global domain on exit.

If you drive `create_context` + `compile` + `interpret` manually and then drop
the context without clearing the global domain, global structures are released
during context teardown — after the process/instance have begun dying — and
their `finalize` will not run correctly. Call
`ctx->instance->storage.clear_global()` while the context is alive (the process
must still be active) to run finalizers deterministically.

### 4. `var` lifetime

`cs::var` is a 8-byte handle; copying it bumps a reference count, and the value
is freed when the last reference drops. To detach a value from its original
storage use `cs::copy(var)` (deep copy). Values that escape a context are safe
*as values* (they are self-contained), but a `callable`/`structure` value
*also* carries the raw back-references described in §1, so the context rule
still applies to them.

### 5. Token arena and recompilation

Each `compile()` produces a program whose tokens live in a per-instance arena.
Compiling again (or calling `release_statements()`) drops the previous program
and its arena.

A script function/lambda escaped from the previous program keeps that arena
alive via `function::m_unit`, so it remains callable after recompilation —
*provided its context is still alive* (§1). This is the one pinning mechanism
that survives, because it concerns the arena rather than the context.

### 6. Runtime diagnostics (`COVSCRIPT_DEBUG`)

Defensive runtime guards (for example destroying an unfinished fiber, or a
non-empty function value stack at program entry) behave according to the
`COVSCRIPT_DEBUG` environment variable:

| Value | Behaviour |
| :-- | :-- |
| `none` | Ignore silently; the program keeps running (partial leaks accepted) |
| `warning` | Print a warning to stderr and continue (default) |
| `strict` | Print a warning and abort (fail-fast) |

The value is case-insensitive; unset or unknown values default to `warning`.
Guards are diagnostic only — they do not change the ownership contracts above.
