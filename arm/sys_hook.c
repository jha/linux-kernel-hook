#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "sys_hook.h"

struct sys_hook *
sys_hook_init(uintptr_t k_arm)
{
    struct sys_hook *sh;

    sh = kmalloc(sizeof (struct sys_hook), GFP_KERNEL);
    if (IS_ERR(sh)) {
        printk(KERN_INFO "not enough memory for hooks\n");
        return NULL;
    }

    sh->arm_sct = (unsigned int *)k_arm;

    sh->head = sh->tail = NULL;

    return sh;
}

bool_t
sys_hook_add(struct sys_hook *hook, unsigned int syscall_id, void *func)
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
    ent->original = hook->arm_sct[syscall_id];
    ent->hooked = (uintptr_t)func;
    ent->type = SHT_ARM;

    /* Overwrite the entry in the syscall table */
    hook->arm_sct[syscall_id] = (unsigned int)ent->hooked;

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
sys_hook_del(struct sys_hook *hook, unsigned int syscall_id)
{
    return TRUE;
}

uintptr_t
sys_hook_get_orig(struct sys_hook *hook, unsigned int syscall_id)
{
    struct sys_hook_ent *curr;

    for (curr = hook->head; curr != NULL; curr = curr->next) {
        if (curr->type == SHT_ARM && curr->syscall_id == syscall_id)
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

    for (curr = hook->head; curr != NULL;) {
        switch (curr->type) {
            case SHT_ARM:
                hook->arm_sct[curr->syscall_id] = (unsigned int)curr->original;
                break;
            default:
                printk(KERN_EMERG "possible memory corruption in syscall hooks - invalid hook state\n");
                break;
        }

        tmp = curr->next;
        kfree(curr);
        curr = tmp;
    }

    kfree(hook);
}
