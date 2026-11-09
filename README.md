# Format String Foothold (CyberRangeCZ Platform)

A two-host sandbox for [CyberRangeCZ Platform](https://docs.platform.cyberrange.cz/), adapting
SEED Labs' *Format String Attack Lab* (W. Du) into a graded, per-student exercise. Built to
mirror the structure of `library-junior-hacker` and `seed-bof-lab`.

## Game levels summary

- recon
- Task 1: crash the program
- Task 2: print out the server's memory (stack + heap)
- Task 3: modify the server's memory (small change, specific value, large value via %hn/%hhn)
- Task 4: inject malicious code
- Task 5: the 64-bit zero-byte-address problem
- Task 6: fix the vulnerability

## Topology summary

| Host     | Image             | Flavor         |
|----------|-------------------|----------------|
| attacker | kali              | standard.small |
| server   | debian-12-x86_64  | standard.small |

Two hosts + router. Students only log into `attacker`; `server` is exploited over the network
on TCP/9090, same as the handout.

## Status

`format.c` and `server.c` are SEED Labs' own files from `Labsetup.zip`, lightly adapted — see
`ATTRIBUTION.txt` for exactly what changed and why (APG variables, and enabling an
optional stdout-redirect block the stock file ships commented out, needed because attacker
and server are separate hosts here). All six tasks' content and flags were verified by
actually compiling and running this pair; see the QA notes in `INSTRUCTOR_NOTES.md`.

**Not included:** `attack-code/exploit.py`, `attack-code/build_string.py`, and their shellcode,
from `Labsetup.zip`. Task 4 describes what the exploit needs to do and points at the handout's
relevant sections, but doesn't ship a working payload. See `INSTRUCTOR_NOTES.md`.

**Important runtime detail:** the server sends its output back on a *second* TCP connection
(port 9091), not on the same socket you send input through — `training.json`'s Introduction
level explains this, but you'll want to know it before testing the sandbox yourself too.

## Why every sandbox is different

APG (Automatic Problem Generation) — see `variables.yml`. Each sandbox gets its own `buf_size`
(shifts the stack layout, same role the lab's own instructor-tunable `L`/`BUF_SIZE` plays) and
two random flag tokens (`flag_task2`, embedded as the `secret` string; `flag_task3`, dropped to
a file only reachable via Task 4's code execution).

## License

* Code (Ansible, templates) - MIT License.
* Game design / level text - CC BY 4.0, adapted from SEED Labs' *Format String Attack Lab*
  (Copyright © 2018-2020 Wenliang Du). See `ATTRIBUTION.txt`.
