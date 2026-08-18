/* ArkOS libstdc++ shims for GCC 15-built objects linked against older libstdc++. */
#include <cstdlib>
#include <new>

namespace std {
void __throw_bad_array_new_length()
{
  abort();
}
}
