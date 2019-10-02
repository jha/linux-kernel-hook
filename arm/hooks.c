#include <linux/module.h>
#include "hooks.h"
#include "sys_hook.h"

extern struct sys_hook *lkh_sys_hook;

asmlinkage int
mkdir_hook(const char *path, int mode)
{
    sys_mkdir_t sys_mkdir;
    printk(KERN_INFO "mk_dir hook triggered %s\n", path);
    
    sys_mkdir = (sys_mkdir_t)sys_hook_get_orig(lkh_sys_hook, __NR_mkdir);

    return sys_mkdir(path, mode);
}
