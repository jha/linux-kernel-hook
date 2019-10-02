#pragma once

#include "types.h"

struct sys_hook {
    unsigned int *arm_sct;
    struct sys_hook_ent *head, *tail;
};

enum sys_hook_type {
    SHT_ARM,
};

struct sys_hook_ent {
    struct sys_hook_ent *next;
    unsigned int syscall_id;
    uintptr_t original, hooked;
    enum sys_hook_type type;
};

struct sys_hook *
sys_hook_init(uintptr_t k_arm);

bool_t
sys_hook_add(struct sys_hook *, unsigned int syscall_id, void *func);

bool_t
sys_hook_del(struct sys_hook *, unsigned int syscall_id);

uintptr_t
sys_hook_get_orig(struct sys_hook *, unsigned int syscall_id);

void
sys_hook_free(struct sys_hook *hook);
