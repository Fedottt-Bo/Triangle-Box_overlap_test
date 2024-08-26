/* Copyright 2024 Fedor Borodulin

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* "def.h" - Global definitions file */

#ifndef __def_h__
#define __def_h__

/* Storage types */
#include <array>
#include <string>
#include <list>
#include <set>
#include <map>
#include <span>

/* Time */
#include <chrono>
namespace std
{
  using namespace chrono;
}

/* Erorrs handling */
#include <stdexcept>

/* Miltithreading */
#include <thread>
#include <semaphore>
#include <mutex>

/* Utility */
#include <format>
#include <iostream>
#include <concepts>
#include <type_traits>
#include <locale>
#include <codecvt>

/* Filesystem */
#include <filesystem>
#include <fstream>

/* Additional defines for convenient filesystem usage */
namespace std
{
  namespace fs = filesystem;

  namespace filesystem
  {
    using path_string_view = basic_string_view<path::string_type::value_type>;
  }
}

#endif /* __def_h__ */

/* END OF 'def.h' FILE */
