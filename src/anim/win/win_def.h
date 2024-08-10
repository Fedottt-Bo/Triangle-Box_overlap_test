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

/* "win_def.h" - Windows.h file wrapper */

#ifndef __win_def_h__
#define __win_def_h__

/* Prepare settings defines */
#define WIN32_LEAN_AND_MEAN
#undef _MSC_EXTENSIONS
#define _WIN32_WINNT 0x0601

/* Windows include */
#include <Windows.h>

/* Get rid of Windows defines that overlaps common functions/methods/etc. */
#undef VOID
#undef TRUE
#undef FALSE
#undef min
#undef max
#undef CreateWindow
#undef CreateSemaphore
#undef PostMessage
#undef interface

#endif /* __win_def_h__ */

/* END OF 'win_def.h' FILE */
