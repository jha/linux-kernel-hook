# Linux Kernel Hook
This is a fork from https://github.com/jha/linux-kernel-hook . I added an arm32 version of the module, as a personal curiosity.  
There is one directory per architecture (x86_64 and arm32). At first I put macros everywhere to us the same sources for both arch but
I finally find it more readable with a folder for each arch (even though it could be factorized).

# Usage
```
cd <arch>
make
cd ..
./load.sh
```

The example hook for mkdir prints the argument of the created folder in the kernel logs.  
  
To unload the module: `rmmod lkh`