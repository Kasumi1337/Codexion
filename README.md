*This project has been created as part of the 42 curriculum by thbeil.*

# Codexion

## Description

Codexion is a multithreaded simulation that models a variant of the classic
**dining philosophers problem**. Instead of philosophers and forks, the
simulation features **coders** sitting in a circular co-working hub who
compete for a shared, limited pool of **USB dongles** to compile their code.

Each coder cycles endlessly through three states — **compiling**,
**debugging**, and **refactoring** — and must acquire *two* dongles (one on
each side) before compiling. Dongles are protected resources with a mandatory
**cooldown** after being released, and access is arbitrated by a configurable
**scheduler** (`fifo` or `edf`). A dedicated **monitor thread** watches every
coder and stops the simulation the instant one of them **burns out** by
failing to start compiling again before their personal deadline — or once
every coder has compiled the required number of times.

The goal of the project is to practice real POSIX thread synchronization:
mutexes, condition variables, deadlock avoidance, starvation prevention, and
precise, race-free logging — all with strictly no global variables.

## Instructions

### Compilation

```sh
make
```

This produces the `codexion` binary at the root of the repository. The
Makefile also provides `clean`, `fclean`, and `re`, and only recompiles the
source files that actually changed.

### Running

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All 8 arguments are mandatory, must be positive integers (except `scheduler`),
and `scheduler` must be exactly `fifo` or `edf`. Any invalid input (negative
number, non-numeric value, wrong argument count, unknown scheduler, or a
value of `0` where it would make the simulation meaningless) is rejected with
a non-zero exit code.

Example:

```sh
./codexion 3 800 200 100 100 5 50 fifo
```

This runs a simulation with 3 coders, a burnout deadline of 800 ms, a
200 ms compile phase, 100 ms debug and refactor phases, requiring 5
successful compiles per coder, a 50 ms dongle cooldown, using FIFO
arbitration.

## Resources

- [POSIX Threads Programming (LLNL tutorial)](https://hpc-tutorials.llnl.gov/posix/)
- `man pthread_create`, `man pthread_mutex_lock`, `man pthread_cond_wait`,
  `man pthread_cond_timedwait`, `man gettimeofday`
- [Dining philosophers problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Coffman conditions for deadlock — Wikipedia](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- [Earliest deadline first scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

**AI usage**: an AI assistant (Claude) was used throughout this project as a
tutor rather than a code generator. It was prompted to explain concepts,
ask guiding questions, and point out bugs without writing the solution
outright, so that every line could be understood and defended during
evaluation. Concretely, AI helped with:
- Understanding pointer semantics, struct design, and header ordering issues
  encountered while building `t_config`, `t_dongle`, `t_coder`, and
  `t_simulation`.
- Reasoning through the FIFO/EDF arbitration logic and the deadlock caused by
  naive symmetric dongle acquisition (classic dining-philosophers deadlock),
  leading to the even/odd acquisition-order fix.
- Debugging a starvation bug where a coder waiting for a dongle stuck in
  cooldown never woke up (`pthread_cond_wait` never being signaled after
  cooldown expiry), which was fixed by switching to
  `pthread_cond_timedwait` with a short recurring deadline.
- Debugging a self-deadlock in the `number_of_coders == 1` edge case, where
  a single coder's left and right dongle are the same object.
- Diagnosing memory leaks reported by Valgrind (`sim->coders` and
  `sim->dongles` never freed, mutexes/condition variables never destroyed)
  and building the `cleanup` function.
- Splitting the final code into multiple Norm-compliant files (max 5
  functions per file, 25 lines per function, 4 parameters per function).

Every AI-assisted change was manually traced through by hand (with concrete
example values) before being accepted, and the resulting behaviour was
verified by running the program and inspecting the logs.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions)**: a classic circular-wait
  deadlock is possible if every coder always acquires their *left* dongle
  first, then their *right* one — under full contention, every coder could
  end up holding one dongle while waiting forever for the other. This is
  broken by having **odd-numbered coders** acquire left-then-right and
  **even-numbered coders** acquire right-then-left, which removes the
  circular-wait condition (one of the four necessary Coffman conditions)
  while keeping mutual exclusion, hold-and-wait and no-preemption intact,
  which is sufficient to prevent the cycle.
- **Starvation prevention**: dongle access is arbitrated by the chosen
  `scheduler`. In `fifo` mode the coder that started waiting first is
  served first; in `edf` mode the coder with the closest burnout deadline
  is served first. A deterministic tie-breaker (lowest coder number wins)
  guarantees a fully deterministic outcome even on ties, so no coder can be
  passed over indefinitely.
- **Cooldown handling**: after a dongle is released, it stays unusable until
  `dongle_cooldown` milliseconds have passed. A coder waiting on a dongle
  that is free but still cooling down is not signaled by anyone (no one
  releases the dongle again during that window), so waiting is done with
  `pthread_cond_timedwait` on a short recurring deadline instead of a plain
  `pthread_cond_wait`, guaranteeing the coder re-checks the condition on its
  own even without an external wake-up.
- **Precise burnout detection**: a dedicated monitor thread polls every
  coder's deadline (`when_last_compile + time_to_burnout`) every 1 ms,
  comfortably under the 10 ms precision required by the subject, and stops
  the whole simulation as soon as one is exceeded.
- **Log serialization**: all state-change messages go through a single
  `log_state` function that locks a dedicated `log_mutex` around the
  `printf` call, guaranteeing two messages can never interleave on the same
  line.

## Thread synchronization mechanisms

- **`pthread_mutex_t` per dongle**: protects `is_taken`, `released_at`, and
  `never_used` so that two coders can never both believe they've acquired
  the same dongle at the same time.
- **`pthread_cond_t` per dongle**: used with `pthread_cond_timedwait` so a
  waiting coder is woken up either when the dongle is released
  (`pthread_cond_broadcast` in `release_dongle`) or, failing that, at least
  every 5 ms to re-check cooldown expiry and scheduling priority.
  `pthread_cond_broadcast` (rather than `signal`) is used because a dongle
  only ever has two possible rivals, and only the one that actually wins the
  arbitration should proceed — broadcasting and re-checking the condition
  guarantees the correct rival is never missed.
- **`stop_mutex`**: protects the shared `stop` flag. Every read goes through
  `should_stop()` and every write through `set_stop()`, both of which lock
  the mutex, so coder threads and the monitor thread never race on this flag
  even though one thread writes it while many threads read it concurrently.
- **`log_mutex`**: serializes all `printf` calls across every coder thread
  and the monitor thread (see "Log serialization" above).
- **Thread-safe communication between coders and the monitor**: coders and
  the monitor never communicate directly — they only interact through the
  shared `t_simulation` structure (passed by pointer, never copied), each
  field of which is either read-only after initialization (`config`,
  pointers to dongles) or protected by its own mutex (`stop`, dongle state,
  log output). This avoids the need for any global variable while keeping
  every shared access race-free.

Example of a race condition avoided: without `stop_mutex`, a coder thread
reading `sim->stop` while the monitor thread writes `sim->stop = 1` at the
exact same instant would be undefined behaviour on some architectures. Wrapping
both the read (`should_stop`) and the write (`set_stop`) in the same mutex
guarantees the coder always observes either the value before or the value
after the write, never a partially-updated state.
