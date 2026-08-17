// ArgParser
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <filesystem>
#include <iostream>
#include <optional>

#include <argpp/argparser.hpp>

int main(int argc, char** argv)
{
  try
  {
    int verbose = 0;

    using argparser::Argument;
    using argparser::Flags;
    argparser::OptionGroup opts;

    opts.program(argv[0]);
    opts.usage("Custom Usage String");
    opts.usage("Custom Usage String 2");
    opts.usage("Custom Usage String 3");
    opts.text("Short description of what it does");
    opts.newline();
    opts.text("Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. "
                  "Lengthy description of what the program does. ");

    opts.group("Options:");
    opts.option('v', "version", "Version", Flags().mutual_exclusion(1u));
    opts.option('V', "verbose", "Version", Flags().mutual_exclusion(1u)).increment(verbose);
    opts.option('h', "help", "Help text", Flags().mutual_exclusion(1u)).then([&]{ argparser::print_help(opts, std::cout, 1u); });
    opts.option('H', "help-extra", "Help text").then([&]{ argparser::print_help(opts, std::cout, ~0u); });
    opts.alias("hilfe", opts.lookup_long_option("help"));
    opts.option({}, "long-only", Argument("ARG"), "Blabla");

    opts.group("Rest Arguments:");
    auto& rest_item = opts.rest_options(Argument<std::filesystem::path>("FILE"), "Files to do stuff with");
    rest_item.then([](std::filesystem::path const& path){
      std::cout << "Path: " << path << std::endl;
    });
    auto& rest_opts = rest_item.get_options();
    rest_opts.group("Rest Argument Options:");
    rest_opts.option('h', "help", "Help text...").then([]{
      std::cout << "  help" << std::endl;
    });

    opts.newline();
    opts.text("Copyright, author email and all that stuff");

    std::cout << "----------------------------------------------" << std::endl;
    argparser::parse(opts, argc, argv);

    std::cout << "Verbose: " << verbose << std::endl;

    return 0;
  }
  catch(const std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return 1;
  }
}

/* EOF */
