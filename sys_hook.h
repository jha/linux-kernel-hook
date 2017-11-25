#pragma once

#include "types.h"

struct sys_hook {
    unsigned int *x86_sct;
    unsigned long long *x64_sct;
    struct sys_hook_ent *head, *tail;
};

enum sys_hook_type {
    SHT_X86,
    SHT_X64,
};

struct sys_hook_ent {
    struct sys_hook_ent *next;
    unsigned int syscall_id;
    uintptr_t original, hooked;
    enum sys_hook_type type;
};

#define CR0_WRITE_PROTECT   (1 << 16)

struct sys_hook *
sys_hook_init(uintptr_t k32, uintptr_t k64);

bool_t
sys_hook_add64(struct sys_hook *, unsigned int syscall_id, void *func);

bool_t
sys_hook_del64(struct sys_hook *, unsigned int syscall_id);

uintptr_t
sys_hook_get_orig64(struct sys_hook *, unsigned int syscall_id);

void
sys_hook_free(struct sys_hook *hook);
