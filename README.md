# Linux Kernel Hook

Syscall hooks for x86_64 and arm32.

# Usage
```
cd <arch>
make
cd ..
./load.sh
```

The example hook for mkdir prints the argument of the created folder in the kernel logs.  
  
To unload the module: `rmmod lkh`