/* ArkOS glibc compatibility for newer-toolchain static libs. */
#include <stdlib.h>

/* Prevent the linker from dropping these when linking static archives. */
long int __isoc23_strtol(const char *nptr, char **endptr, int base)
  __attribute__((visibility("default")));
unsigned long int __isoc23_strtoul(const char *nptr, char **endptr, int base)
  __attribute__((visibility("default")));
long long int __isoc23_strtoll(const char *nptr, char **endptr, int base)
  __attribute__((visibility("default")));
unsigned long long int __isoc23_strtoull(const char *nptr, char **endptr, int base)
  __attribute__((visibility("default")));
void __cxa_call_terminate(void *eh) __attribute__((visibility("default")));
void _dl_find_object(void) __attribute__((weak, visibility("default")));

long int __isoc23_strtol(const char *nptr, char **endptr, int base)
{ return strtol(nptr, endptr, base); }
unsigned long int __isoc23_strtoul(const char *nptr, char **endptr, int base)
{ return strtoul(nptr, endptr, base); }
long long int __isoc23_strtoll(const char *nptr, char **endptr, int base)
{ return strtoll(nptr, endptr, base); }
unsigned long long int __isoc23_strtoull(const char *nptr, char **endptr, int base)
{ return strtoull(nptr, endptr, base); }

void __cxa_call_terminate(void *eh)
{
  (void)eh;
  abort();
}

/* libgcc_eh from GCC 15 may reference this; ArkOS ld.so has no export. */
void _dl_find_object(void) { }
