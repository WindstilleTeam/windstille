// R36S / ArkOS exception smoke test.
// Build with the ArkOS sysroot wrappers; run on device (or qemu-user).
// Exit 0 if throw/catch works; non-zero if unwind aborts or catch fails.

#include <cstdio>
#include <exception>
#include <stdexcept>
#include <string>

static int g_step = 0;

static void throw_runtime()
{
  g_step = 1;
  throw std::runtime_error("r36s-exception-test");
}

static void throw_int()
{
  g_step = 2;
  throw 42;
}

int main()
{
  std::printf("r36s-exception-test: start\n");
  std::fflush(stdout);

  try {
    throw_runtime();
    std::printf("FAIL: throw_runtime did not throw\n");
    return 2;
  } catch (std::runtime_error const& e) {
    std::printf("ok: caught runtime_error: %s (step=%d)\n", e.what(), g_step);
  } catch (...) {
    std::printf("FAIL: runtime_error caught by catch-all\n");
    return 3;
  }

  try {
    throw_int();
    std::printf("FAIL: throw_int did not throw\n");
    return 4;
  } catch (int v) {
    std::printf("ok: caught int %d (step=%d)\n", v, g_step);
  } catch (...) {
    std::printf("FAIL: int caught by catch-all\n");
    return 5;
  }

  try {
    throw std::string("string-ex");
  } catch (std::string const& s) {
    std::printf("ok: caught std::string: %s\n", s.c_str());
  } catch (...) {
    std::printf("FAIL: std::string not caught\n");
    return 6;
  }

  // Nested rethrow
  try {
    try {
      throw std::logic_error("nested");
    } catch (...) {
      std::printf("ok: inner catch-all, rethrow\n");
      throw;
    }
  } catch (std::logic_error const& e) {
    std::printf("ok: outer caught logic_error: %s\n", e.what());
  } catch (...) {
    std::printf("FAIL: nested rethrow\n");
    return 7;
  }

  std::printf("r36s-exception-test: ALL PASSED\n");
  std::fflush(stdout);
  return 0;
}
