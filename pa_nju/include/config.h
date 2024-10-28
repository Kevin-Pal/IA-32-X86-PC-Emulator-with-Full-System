#ifndef __PA_CONFIG_H__
#define __PA_CONFIG_H__

// #define NEMU_REF_INSTR

// PA 3
#define CACHE_ENABLED        // cache is enabled
#define IA32_SEG             // protect mode enable
#define IA32_PAGE            // virtual memory management is now complete
//#define TLB_ENABLED
//#define CACHE_TIMER         // cache timer is enabled (to time the execution time of with and without cache)

/*
note: if you want to enable cache timer, you need to add the following code in nemu/src/main.c. Howerver, if you change the code in nemu/src/main.c, you will not be able to use the command make submit_pa-3-1 to submit your code, because the integrity of the code will be checked. You have to restore the code in nemu/src/main.c to the original code before you submit your code.

add this at the beginning of the file nemu/src/main.c:
```c
#ifdef CACHE_TIMER
extern uint64_t cache_time;
extern uint64_t cache_hit;
extern uint64_t cache_miss;
#endif
```

add this at the end of the function `main` (before `return 0;`) in the file nemu/src/main.c:
```c
#ifdef CACHE_TIMER

#ifdef CACHE_ENABLED
    printf("Cache is enabled\n");
    printf("Cache time: %lld\n", cache_time);
    printf("Cache hit: %lld, Cache miss: %lld, Cache hit rate: %.2f\n", cache_hit, cache_miss, (double)cache_hit / (cache_hit + cache_miss));
#else
    printf("Cache is disabled\n");
    printf("without Cache time: %lld\n", cache_time);
#endif

#endif
```
*/


// PA 4
#define IA32_INTR            // tells NEMU and Kernel that we are ready for handling exceptions(traps only) and interrupts
#define HAS_DEVICE_TIMER
#define HAS_DEVICE_SERIAL    // the disturbing 'nemu trap output:' tag will be removed
#define HAS_DEVICE_IDE       // the loader will load the elf from the hard disk instead of mem disk
#define HAS_DEVICE_KEYBOARD  // keyboard input
#define HAS_DEVICE_VGA       // we then have display
//#define HAS_DEVICE_AUDIO     // audio, this is experimental, need to enable when submit pa-4-3

#define PA_ONLINE

#endif
