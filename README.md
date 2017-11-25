# Linux Kernel Hook
This is just a small project to hook syscalls in an x86_64 Linux kernel. I've tested it on `4.9.0-3-amd64`. It's mostly just a project for me to explore writing kernel modules.

Most of the other kernel hooks I've seen on the public internet use an outdated mechanism to grab the syscall table - the oldest tutorials relied on `sys_call_table` being exported as a public symbol, and slightly newer ones had a brute-forcing approach where they would try to find the syscall table in between two different symbols. This one doesn't do anything that fancy - the `load.sh` script just greps `/proc/kallsyms` for the syscall table addresses.

## Usage
It comes built in with a `mkdir` hook that just proxies the syscall over to the original syscall. `module.c:69` is responsible for adding the hook, and the code at `hooks.c:8` is the actual hooked function which does the proxying
```bash
make && sh ./load.sh
```
