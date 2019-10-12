#pragma once

#include <linux/syscalls.h>
#include "types.h"

typedef asmlinkage int (*sys_mkdir_t)(const char *, int);

asmlinkage int
mkdir_hook(const char *, int);
