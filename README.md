# Format String Foothold (CyberRangeCZ Platform)

A single-VM sandbox for [CyberRangeCZ Platform](https://docs.platform.cyberrange.cz/), adapting
SEED Labs' *Format String Vulnerability Lab* (W. Du) into a graded, per-student exercise.

## What changed from the earlier (TCP, two-host, 64-bit) version of this project

This project was originally built around a different SEED lab PDF (*Format String Attack
Lab*: TCP, two hosts, 64-bit, `BUF_SIZE`). It's now rebuilt around *Format String Vulnerability
Lab* instead: UDP, one host, 32-bit, `DUMMY_SIZE`, and a root daemon the student escalates to
locally. See ATTRIBUTION.txt for exactly what's reconstructed from the new handout and what
changed.

## Game levels summary

- Recon
- Task 2: understanding the stack layout (conceptual)
- Task 3: crash the program
- Task 4: print out the server's memory (stack + heap)
- Task 5: modify the server's memory (any value, a specific value, a large value via %hn)
- Task 6-7: inject code and get a root shell
- Task 8: fix the vulnerability

(The handout's own "Task 1" is just the vulnerable program itself - folded into the
Introduction here, not a separate graded level.)

## Topology summary

| Host   | Image            | Flavor         |
|--------|------------------|----------------|
| victim | debian-12-x86_64 | standard.small |

One host + router. The student logs into `victim` and attacks the root daemon running on the
same machine, over `127.0.0.1:9090/udp` - matching the handout's own "acceptable to use one VM"
note.

## Why every sandbox is different

APG (Automatic Problem Generation) — see `variables.yml`. Each sandbox gets its own
`dummy_size` (shifts the stack layout - stronger than the handout's own suggestion of one
value per class) and two random flag tokens (`flag_task2`, embedded as the `secret` string;
`flag_task3`, dropped to `/root/flag_task3.txt`, reachable only via Task 6-7's code execution).

## Status

`server.c` is reconstructed from the handout PDF's Listing 1 (no Labsetup.zip was available
for this specific lab) - see `ATTRIBUTION.txt` for the handful of necessary additions (a
missing header, a stub for an undefined function, a debug marker). I compiled and ran this
file (minus its one 32-bit-only inline-asm line, which this environment couldn't assemble -
see `INSTRUCTOR_NOTES.md`) to confirm the rest of it works, including a buffering issue this
testing surfaced: the daemon never exits, so its stdout needs to be run unbuffered
(`stdbuf -o0`, already wired into the systemd unit) or `journalctl` won't show anything for a
long time.

**Not included:** a working Task 6-7 exploit (shellcode + offsets for this specific target).
`exploit.py` / `build_string.py` are present in `provisioning/roles/victim/files/attack-code/`
- they were carried over from this project's earlier TCP/64-bit version, where they came from
that lab's own `Labsetup.zip`, and will need rework for this one (different protocol,
architecture, and stack layout). See `INSTRUCTOR_NOTES.md`.
