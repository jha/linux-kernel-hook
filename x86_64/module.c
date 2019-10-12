#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include "sys_hook.h"
#include "hooks.h"

struct sys_hook *lkh_sys_hook;

static uintptr_t
hex_addr_to_pointer(const char *str)
{
    uintptr_t sum;

    /* Go through str char by char and accumulate into sum */
    for (sum = 0; *str != '\0'; str++) {
        sum *= 16;
        if (*str >= '0' && *str <= '9')
            sum += (*str - '0');
        else if (*str >= 'a' && *str <= 'f')
            sum += (*str - 'a') + 10;
        else if (*str >= 'A' && *str <= 'F')
            sum += (*str - 'A') + 10;
        else
            return 0;
    }

    return sum;
}

/* Module parameter macros */
static char *kbase32 = NULL, *kbase64 = NULL;
module_param(kbase32, charp, 0);
MODULE_PARM_DESC(kbase32, "Base address of the x86 syscall table, in hex");
module_param(kbase64, charp, 0);
MODULE_PARM_DESC(kbase64, "Base address of the x64 syscall table, in hex");

static int __init
module_entry(void)
{
    uintptr_t k32, k64;

    printk(KERN_INFO "lkh initializing...\n");
    
    /* Validate that we got parameters */
    if (kbase32 == NULL || kbase32[0] == '\0') {
        printk(KERN_INFO "failed to get x86 syscall table\n");
        return 1;
    } else if (kbase64 == NULL || kbase64[0] == '\0') {
        printk(KERN_INFO "failed to get x64 syscall table\n");
        return 1;
    }

    /* Validate that we got valid syscall base addresses */
    if ((k32 = hex_addr_to_pointer(kbase32)) == 0) {
        printk(KERN_INFO "invalid x86 syscall address %p\n", (void *)k32);
        return 1;
    } else if ((k64 = hex_addr_to_pointer(kbase64)) == 0) {
        printk(KERN_INFO "invalid x64 syscall address %p\n", (void *)k64);
        return 1;
    }

    if ((lkh_sys_hook = sys_hook_init(k32, k64)) == NULL) {
        printk(KERN_INFO "failed to initialize sys_hook\n");
        return 1;
    }

    sys_hook_add64(lkh_sys_hook, __NR_mkdir, (void *)mkdir_hook);

    printk(KERN_INFO "lkh loaded\n");
    return 0;
}

static void __exit
module_cleanup(void)
{
    sys_hook_free(lkh_sys_hook);
    printk(KERN_INFO "lkh has finished\n");
}

/* Declare the entry and exit points of our module */
module_init(module_entry);
module_exit(module_cleanup);

/* Shut up kernel warnings about tainted kernels due to non-free software */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("github:jha");
MODULE_DESCRIPTION("Linux Kernel Hook");
