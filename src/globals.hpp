// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005-2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
// Copyright (C) 2023 Vankata453 <78196474+Vankata453@users.noreply.github.com>
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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "tree.hpp"
#include <iostream>

// parsing
extern CompilationUnit* unit;
extern bool search_down;
extern Namespace* search_namespace;
extern Namespace* current_namespace;
extern std::string last_docucomment;
// the first file indicated by # 1 "..."
// (this is what the C preprocessor outputs so that you know which was the
// original file before preprocessing
extern std::string original_file;
// the filename where the current fragment came from (before it was included by
// the preprocessor)
extern std::string current_file;
// get line number inside the current_file
int getCurrentLine();

// config/output
extern std::istream* input;
extern std::string inputfile;
extern std::string modulename;
extern std::string selected_namespace;
extern std::string inputhpp_include;
extern std::string outputhpp_include;

#endif
