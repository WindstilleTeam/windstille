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

    constexpr uint32_t EXTRA_VERBOSE = 1<<1;
    constexpr uint32_t ONLY_ONE = 1<<1;

    argparser::OptionGroup opts;
    opts.program(argv[0]);

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

    //opts.option('c', "complex", Argument<int,int,int>("X:Y:Z"), "Blabla");

    std::optional<std::string> stringvar;
    opts.option('z', {}, Argument("ARG"), "Blabla").store(stringvar);

    opts.group("Options with arguments:");
    opts.option('f', "file", Argument<std::filesystem::path>("FILE"), "Do File", Flags().mutual_exclusion(ONLY_ONE))
      .then([](auto const& path)
      {
        std::cout << "Got path: " << path << std::endl;
      });
    opts.option('n', "number", Argument<int>("FILE"), "Number", Flags().mutual_exclusion(ONLY_ONE))
      .then([](int number) {
        std::cout << "Got int: " << number << std::endl;
      });

    opts.option('t', "text", Argument("FILE"), "Number").then([](std::string_view text) {
      std::cout << "Got text: " << text << std::endl;
    });

    opts.group("Extra Options:", Flags().visibility(EXTRA_VERBOSE));
    opts.option('X', "xeno", "Xeno Help", Flags().visibility(EXTRA_VERBOSE));
    opts.option('Y', "yavi", "Yavi Help", Flags().visibility(EXTRA_VERBOSE));
    opts.option('Z', "zulu", "Zulu Help", Flags().visibility(EXTRA_VERBOSE));

    int var = 0;
    opts.group("Store Test:");
    opts.option('s', "store", "Help text").store(var, 5);
    opts.option('S', "no-store", "Help text").store(var, -10);
    //group.option("-v", "-version", Argument<int>("FILE"), "Help text").append(&var);
    //group.option("-v", "-version", Argument<int>("FILE"), "Help text").then([](ParseContext& ctx, int value){
    //ctx.raise_exception("illegal option");
    //});

    //group.add(Option("-v", "--version").argument(Argument::NONE).help("Help text").store(&var, false));
    //Argument<std::tuple<int, int, std::string>>();
    //Argument<std::vector<int>()

    opts.group("Commands:");
    auto& install_cmd = opts.command("install", "Install stuff");
    auto& install_opts = install_cmd.get_options();
    install_opts.text("Install stuff");

    install_opts.group("Options:");
    install_opts.option('h', "help", "Help text").then([&]{ argparser::print_help(opts, install_cmd); });
    install_opts.option('n', "number", "Blabla");
    install_opts.option('d', "device", "Blabla");

    install_opts.group("Positional arguments:");
    install_opts.positional(Argument("PACKAGE"), "Package to install").then([](std::string_view text){
      std::cout << "Installing package: " << text << "\n";
    });

    auto& search_cmd = opts.command("search", "Search stuff");
    auto& search_opts = search_cmd.get_options();
    search_opts.option('h', "help", "Help text").then([&]{ argparser::print_help(opts, search_cmd); });
    search_opts.option('n', "number", "Blabla");
    search_opts.option('d', "device", "Blabla");

    search_opts.group("Positonal Arguments:");
    search_opts.positional(Argument("FLUB"), "File to load", Flags().required());
    search_opts.positional(Argument("BLOB"), "Output file");

    search_opts.group("Rest Arguments:");
    search_opts.rest(Argument<std::filesystem::path>("FILE"), "Files to do stuff with");

    /*
    opts.newline();
    opts.group("Positional Arguments:");
    opts.positional(Argument("FLUB"), "File to load").then([](std::string_view text){
      std::cout << "flub: " << text << std::endl;
    });
    opts.positional(Argument<std::filesystem::path>("BLOB"), "Output file").then([](std::filesystem::path const& text){
      std::cout << "blob: " << text << std::endl;
    });
    opts.positional(Argument<int>("NUMBER"), "a number file").then([](int number){
      std::cout << "number: " << number << std::endl;
    });
    */

    opts.newline();
    opts.text("Copyright, author email and all that stuff");

    std::cout << "----------------------------------------------" << std::endl;
    argparser::parse(opts, argc, argv);

    std::cout << "Store: " << var << std::endl;
    std::cout << "Verbose: " << verbose << std::endl;
    std::cout << "StringVar: " << (stringvar ? *stringvar : "<not set>") << std::endl;

    return 0;
  }
  catch(const std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return 1;
  }
}

/* EOF */
