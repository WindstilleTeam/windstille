/* ArkOS glibc compatibility for newer-toolchain static libs. */
#include <stdlib.h>

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
