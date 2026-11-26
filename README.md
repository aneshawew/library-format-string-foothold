# Format String Attack Lab (CyberRangeCZ Platform)

A two-host (`attacker` + `victim`) adaptation of SEED Labs' *Format String Attack Lab*
(W. Du), built to match the handout to the letter: six tasks, both a 32-bit and a
64-bit target, no invented extras.

## Topology

| Host     | Image | Flavor         |
|----------|-------|----------------|
| attacker | seed  | m1.medium      |
| victim   | seed  | m1.medium      |

Router + one switch between them, same pattern as other CyberRangeCZ libraries.

## What's provisioned vs. what the student does

Provisioning (`provisioning/roles/*/tasks/main.yml`) only **drops files**:

- `attacker` gets `~/attack-code/{build_string.py,exploit.py}` - the handout's own
  scaffolding, unmodified.
- `victim` gets `~/server-code/{Makefile,server.c,format.c}`, plus a `/root/flag_reverse_shell.txt`
  and a sudoers entry scoped to exactly `server32`/`server64` (see below).

There is **no systemd service and no automated `make`/run step**. The student builds
and starts the servers themselves on `victim` - exactly like the handout's own
`make && make install` workflow, just without Docker. This is spelled out as the
second `ACCESS_LEVEL` in `training.json`.

## Why sudo is scoped, not blanket

The handout's server runs "with the root privilege" (Section 2.2), and Tasks 4-5 are
about getting a root shell via the vulnerability - that's the entire point. If the
student's own login on `victim` had ordinary root access, they could just
`sudo cat /root/flag_reverse_shell.txt` and skip the exploit. So `/etc/sudoers.d/seed-server-code`
grants `NOPASSWD` for exactly `/home/seed/server-code/server32` and `server64`, nothing
else - enough to start the root daemon, not enough to read root's files directly.

## Source fidelity

`format.c`, `server.c`, and the `Makefile` are the files you provided, changed only
where necessary and noted inline:

- `format.c.j2`: only the `secret` string is templated (APG `flag_task4`). Nothing
  else touched.
- `server.c.j2`: `PROGRAM` and `PORT` were hardcoded `#define`s; they're now
  `#ifndef`-guarded with the same default values, so the Makefile can build two
  configured instances of the same file. No other line changed.
- `Makefile.j2`: `L` is templated (APG `buf_size`, matching the handout's own
  suggested 10-400 range); the `TARGET` list and two new rules build `server32`
  (execs `format-32`, port 9090 default) and `server64` (execs `format-64`, port
  9092); the Docker `install` rule was dropped since there's no container here to
  copy binaries into.

See `ATTRIBUTION.txt` for the exact license and copyright notice.

## Ports

- `server32` → `format-32` → TCP/9090
- `server64` → `format-64` → TCP/9092

## Scoring (120 pts)

| Level | Flag | Pts |
|---|---|---|
| Task 1: Crash | static | 10 |
| Task 2: Print memory (stack + heap) | APG (`flag_task4`) | 25 |
| Task 3: Modify memory (3.A/3.B/3.C) | static | 25 |
| Task 4: Inject code + reverse shell (32-bit) | APG (`flag_reverse_shell`) | 35 |
| Task 5: Attack the 64-bit server | static, conceptual | 15 |
| Task 6: Fix the vulnerability | static | 10 |
