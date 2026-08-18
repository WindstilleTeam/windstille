/* ArkOS libstdc++ / libgcc shims for GCC 15-built objects vs older sysroot. */
#include <cstdlib>
#include <cstdio>
#include <new>
#include <charconv>

extern "C" void __cxa_call_terminate(void *eh) __attribute__((visibility("default")));
extern "C" void __cxa_call_terminate(void *eh)
{
  (void)eh;
  std::abort();
}

namespace std {

void __throw_bad_array_new_length()
{
  std::abort();
}

to_chars_result to_chars(char* first, char* last, float value)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%g", static_cast<double>(value));
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}
to_chars_result to_chars(char* first, char* last, double value)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%g", value);
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}
to_chars_result to_chars(char* first, char* last, long double value)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%Lg", value);
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}
to_chars_result to_chars(char* first, char* last, float value, chars_format)
{ return to_chars(first, last, value); }
to_chars_result to_chars(char* first, char* last, double value, chars_format)
{ return to_chars(first, last, value); }
to_chars_result to_chars(char* first, char* last, long double value, chars_format)
{ return to_chars(first, last, value); }
to_chars_result to_chars(char* first, char* last, float value, chars_format, int precision)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%.*g", precision, static_cast<double>(value));
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}
to_chars_result to_chars(char* first, char* last, double value, chars_format, int precision)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%.*g", precision, value);
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}
to_chars_result to_chars(char* first, char* last, long double value, chars_format, int precision)
{
  int n = std::snprintf(first, static_cast<size_t>(last - first), "%.*Lg", precision, value);
  if (n < 0 || n >= last - first) return {last, errc::value_too_large};
  return {first + n, errc{}};
}

} // namespace std
