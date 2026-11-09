# Instructor notes

## APG

`variables.yml` declares `buf_size`, `flag_task2`, `flag_task3`. The Answers Storage service
generates fresh values per sandbox instance; they're exposed as Ansible variables during
provisioning and referenced in `training.json` via `answer_variable_name` on Tasks 2 and 4.

## Scoring (120 pts)

| Level | Flag | Pts |
|---|---|---|
| Recon | static | 5 |
| Task 1: Crash | static, verified | 10 |
| Task 2: Print memory (stack + heap) | APG | 25 |
| Task 3: Modify memory (3.A/3.B/3.C) | static, verified | 25 |
| Task 4: Inject code | APG | 35 |
| Task 5: 64-bit zero-byte addresses | static, conceptual | 10 |
| Task 6: Fix the vulnerability | static | 10 |
