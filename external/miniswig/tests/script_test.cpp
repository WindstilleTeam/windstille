// miniswig - Language binding generator for Squirrel
// Copyright (C) 2023 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>

#include <squirrel.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>

#include <squirrel/squirrel_error.hpp>

#include "util.hpp"
#include "example_wrap.hpp"

namespace {

void my_printfunc(HSQUIRRELVM vm, SQChar const* fmt, ...)
{
  printf("from squirrel: ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void my_errorfunc(HSQUIRRELVM vm, SQChar const* fmt, ...)
{
  fprintf(stderr, "from squirrel: ");

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void my_debug_hook(HSQUIRRELVM vm, SQInteger event_type, SQChar const* sourcename, SQInteger line, SQChar const* funcname)
{
  std::cout << "  sq ";
  switch (event_type)
  {
    case 'l':
      std::cout << "line: ";
      break;

    case 'c':
      std::cout << "call: ";
      break;

    case 'r':
      std::cout << "return: ";
      break;

    default:
      assert(false && "never reached");
  }

  std::cout << ": " << (sourcename ? sourcename : "<null>")
            << ":" << line
            << ":" << (funcname ? funcname : "<null>") << "\n";
}

void my_compiler_error(HSQUIRRELVM vm, SQChar const* desc, SQChar const* source, SQInteger line, SQInteger column)
{
  std::cerr
    << "Compile error:\n"
    << "  Description: " << (desc ? desc : "<null>") << "\n"
    << "  Source: " << (source ? source : "<null>") << "\n"
    << "  Line: " << line << " Column: " << column << std::endl;
}

struct Options
{
  bool debug = false;
  std::string filename;
};

Options parse_args(int argc, char** argv)
{
  Options opts;

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-') {
      if (std::strcmp("--debug", argv[i]) == 0) {
        opts.debug = true;
      } else {
        throw std::runtime_error("unknown option: " + std::string(argv[i]));
      }
    } else {
      if (!opts.filename.empty()) {
        throw std::runtime_error("only one FILENAME argument allowed");
      }
      opts.filename = argv[i];
    }
  }

  if (opts.filename.empty()) {
    throw std::runtime_error("FILENAME argument missing");
  }

  return opts;
}

} // namespace

int main(int argc, char** argv)
{
  Options opts = parse_args(argc, argv);

  std::cout << "main()\n";

  try {
    // create Squirrel VM
    HSQUIRRELVM vm = sq_open(1024);
    if (vm == nullptr) {
      throw std::runtime_error("Couldn't initialize squirrel vm");
    }

    sq_setprintfunc(vm, my_printfunc, my_errorfunc);
    sq_setnativedebughook(vm, my_debug_hook);
    sq_enabledebuginfo(vm, opts.debug);

    sq_setcompilererrorhandler(vm, my_compiler_error);

    std::cout << "-- start\n";

    sq_pushroottable(vm);
    std::cout << "-- register standard functions\n";
    sqstd_register_bloblib(vm);
    sqstd_register_iolib(vm);
    sqstd_register_mathlib(vm);
    sqstd_register_stringlib(vm);
    sqstd_register_systemlib(vm);

    std::cout << "-- register custom functions\n";
    example::register_example_wrapper(vm);
    sq_pop(vm, 1);

    // run tests
    std::string script = load_file(opts.filename);

    std::cout << "-- compiling script\n";
    if (SQ_FAILED(sq_compilebuffer(vm, script.c_str(), script.length(),
                                   opts.filename.c_str(), SQTrue))) {
      throw SquirrelError(vm, "Couldn't compile script");
    }

    std::cout << "-- executing script\n";
    sq_pushroottable(vm);

    if (SQ_FAILED(sq_call(vm, 1, SQTrue, SQTrue))) {
      throw SquirrelError(vm, "Problem while executing script");
    }

    while (sq_getvmstate(vm) == SQ_VMSTATE_SUSPENDED)
    {
      std::cout << "-- vmstate: ";
      switch (sq_getvmstate(vm))
      {
        case SQ_VMSTATE_IDLE:
          std::cout << "idle";
          break;
        case SQ_VMSTATE_RUNNING:
          std::cout << "running";
          break;
        case SQ_VMSTATE_SUSPENDED:
          std::cout << "suspended";
          break;
        default:
          std::cout << "unknown";
      }
      std::cout << "\n";

      if (SQ_FAILED(sq_wakeupvm(vm,
                                SQFalse /* resumedret */,
                                SQFalse /* retval */,
                                SQTrue  /* raiseerror */,
                                SQFalse /* throwerror */)))
      {
        throw SquirrelError(vm, "Problem while resuming script");
      }
    }

    print_squirrel_stack(vm);

    // compiled script, roottable
    assert(sq_gettop(vm) == 2);

    std::cout << "-- closing\n";
    sq_close(vm);

    std::cout << "-- end\n";

    return EXIT_SUCCESS;
  } catch (std::exception const& err) {
    std::cerr << "error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}

/* EOF */
