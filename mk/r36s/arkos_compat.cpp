/* ArkOS libstdc++ shims for GCC 15-built objects linked against older libstdc++. */
#include <cstdlib>
#include <new>
#include <charconv>
#include <cstdio>
#include <cstring>

namespace std {

void __throw_bad_array_new_length()
{
  abort();
}

/* Floating to_chars used by std::format; approximate via snprintf. */
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
