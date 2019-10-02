#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "sys_hook.h"

static uint64_t
get_cr0(void)
{
    uint64_t ret;

    __asm__ volatile (
        "movq %%cr0, %[ret]"
        : [ret] "=r" (ret)
    );
    
    return ret;
}

static void
set_cr0(uint64_t cr0)
{
    __asm__ volatile (
        "movq %[cr0], %%cr0"
        :
        : [cr0] "r" (cr0)
    );
}

struct sys_hook *
sys_hook_init(uintptr_t k32, uintptr_t k64)
{
    struct sys_hook *sh;

    sh = kmalloc(sizeof (struct sys_hook), GFP_KERNEL);
    if (IS_ERR(sh)) {
        printk(KERN_INFO "not enough memory for hooks\n");
        return NULL;
    }

    sh->x86_sct = (unsigned int *)k32;
    sh->x64_sct = (unsigned long long *)k64;

    sh->head = sh->tail = NULL;

    return sh;
}

bool_t
sys_hook_add64(struct sys_hook *hook, unsigned int syscall_id, void *func)
{
    struct sys_hook_ent *ent;

    ent = kmalloc(sizeof (struct sys_hook_ent), GFP_KERNEL);
    if (IS_ERR(ent)) {
        printk(KERN_INFO "not enough memory for sys hook\n");
        return FALSE;
    }

    /* Create our new hook entry */
    ent->next = NULL;
    ent->syscall_id = syscall_id;
    ent->original = hook->x64_sct[syscall_id];
    ent->hooked = (uintptr_t)func;
    ent->type = SHT_X64;

    /* Overwrite the entry in the syscall table */
    set_cr0(get_cr0() & ~CR0_WRITE_PROTECT);
    hook->x64_sct[syscall_id] = (unsigned long long)ent->hooked;
    set_cr0(get_cr0() | CR0_WRITE_PROTECT);

    /* Update the hook list */
    if (hook->head == NULL)
        hook->head = hook->tail = ent;
    else {
        hook->tail->next = ent;
        hook->tail = ent;
    }

    return TRUE;
}

bool_t
sys_hook_del64(struct sys_hook *hook, unsigned int syscall_id)
{
    return TRUE;
}

uintptr_t
sys_hook_get_orig64(struct sys_hook *hook, unsigned int syscall_id)
{
    struct sys_hook_ent *curr;

    for (curr = hook->head; curr != NULL; curr = curr->next) {
        if (curr->type == SHT_X64 && curr->syscall_id == syscall_id)
            return curr->original;
    }

    
    return 0;
}

void
sys_hook_free(struct sys_hook *hook)
{
    struct sys_hook_ent *curr, *tmp;

    if (hook == NULL)
        return;

    set_cr0(get_cr0() & ~CR0_WRITE_PROTECT);

    for (curr = hook->head; curr != NULL;) {
        switch (curr->type) {
            case SHT_X64:
                hook->x64_sct[curr->syscall_id] = (unsigned long long)curr->original;
                break;
            case SHT_X86:
                hook->x86_sct[curr->syscall_id] = (unsigned int)curr->original;
                break;
            default:
                printk(KERN_EMERG "possible memory corruption in syscall hooks - invalid hook state\n");
                break;
        }

        tmp = curr->next;
        kfree(curr);
        curr = tmp;
    }

    set_cr0(get_cr0() | CR0_WRITE_PROTECT);
    kfree(hook);
}
