// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005-2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2018-2020 Ingo Ruhnke <grumbel@gmail.com>
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

#include "tree.hpp"

BasicType BasicType::VOID("void");
BasicType BasicType::BOOL("bool");
BasicType BasicType::CHAR("char");
BasicType BasicType::SHORT("short");
BasicType BasicType::INT("int");
BasicType BasicType::LONG("long");
BasicType BasicType::FLOAT("float");
BasicType BasicType::DOUBLE("double");

StringType* StringType::_instance = nullptr;
HSQUIRRELVMType* HSQUIRRELVMType::_instance = nullptr;
SQIntegerType* SQIntegerType::_instance = nullptr;
