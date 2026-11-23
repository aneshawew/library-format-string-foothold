# Instructor notes

## APG

`variables.yml` declares `dummy_size`, `flag_task2`, `flag_task3`. The Answers Storage service
generates fresh values per sandbox instance; they're exposed as Ansible variables during
provisioning and referenced in `training.json` via `answer_variable_name` on Tasks 4 and 6-7.

## What's real vs. what's still needed

`server.c` is reconstructed from the handout PDF's Listing 1 - no `Labsetup.zip` was supplied
for this particular lab variant, so this is built from the printed code, not a downloaded file.
Four small additions were needed to make it compile and work as a long-running service (see
`ATTRIBUTION.txt` for the exact list: a missing `#include <stdint.h>`, a stub for an undefined
`helper()`, the per-sandbox APG values, and a fixed `SERVER-READY` marker).

I could not fully compile or run the real 32-bit binary in my own environment (no `gcc-multilib`
available there) - I verified the logic by compiling a 64-bit stand-in with the one 32-bit-only
inline-asm line (`movl %%ebp, ...`) stubbed out, confirmed the UDP recvfrom/bind/loop logic
works, and separately confirmed the buffering fix (`stdbuf -o0`, below) against that same
stand-in. The 32-bit compile itself (`gcc -m32 -z execstack ...`) was not run end-to-end -
`gcc-multilib`/`libc6-dev-i386` should be ordinary `apt` packages on a real Debian 12 image, but
confirm this on your first provisioned sandbox before a graded run.

## The SERVER-READY marker and the buffering fix

Two additions not in the handout, both infrastructure rather than exploit content:

- The daemon never exits (infinite `recvfrom()` loop) and a crash takes down the whole process
  (no `fork()`, unlike some other format-string labs) - so "did it crash" is really "did
  systemd restart it." `printf("SERVER-READY\n")` right after `bind()` gives a fixed string
  that appears once at boot and again after any crash-triggered restart, with no address
  dependency - that's Task 3's flag.
- Testing surfaced that this same never-exiting-process behavior means stdout sits fully
  buffered by default; `journalctl` wouldn't show the per-request banner lines until the
  buffer happened to fill (potentially never, under light testing load). `ExecStart=/usr/bin/
  stdbuf -o0 ...` in the systemd unit fixes this - verified in testing.

## Why the daemon runs as root

The handout's own design has a root daemon, and the entire point of Tasks 6-8 is escalating to
it - using anything else would remove the exercise's core objective. This is a single,
per-student VM rather than shared multi-tenant infrastructure, so a student exploiting their
own daemon to get root is root on their own sandbox only - the tradeoff an earlier, two-host
version of this project made (a restricted service account instead of root) doesn't apply the
same way here, since there's no second host or other student's environment at risk.

## Scoring (120 pts)

| Level | Flag | Pts |
|---|---|---|
| Recon | static | 5 |
| Task 2: Stack layout | static, conceptual | 5 |
| Task 3: Crash | static, verified | 10 |
| Task 4: Print memory (stack + heap) | APG | 25 |
| Task 5: Modify memory (5.A/5.B/5.C) | static | 25 |
| Task 6-7: Inject code + root shell | APG | 40 |
| Task 8: Fix the vulnerability | static | 10 |
