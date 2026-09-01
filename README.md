# never_cpp
Valid modern C.  Hostile to every C++ dialect, including C++26.

A single translation unit, themed as a tiny lab mixer: flexible telemetry packet in, restrict-qualified complex buffers mixed, generic type tag out. It is valid C23 (Clang 18: clang -std=c23 -c never_cpp.c succeeds). Clang++ C++26 dies after 20 errors and hits -ferror-limit.
Skipped on purpose so the file stays legal C: K&R definitions, implicit int, implicit function declarations, int foo(); foo(42) (gone in C23), and C2y named break (would need -std=c2y).

What each cluster is there to break:
| In the file | Why C++ dies |
| --- | --- |
| `int samples; int samples;` | no tentative definitions |
| `const int gain;` | `const` must be initialized |
| `extern void black_hole;` | no `void` objects |
| `struct template { int class; int new; … }` | reserved words |
| `struct Inner` used at file scope | nested class scope |
| `enum Bool` + `typedef int Bool` | one namespace for tags and typedefs |
| `char data[]` | no flexible array members |
| `int (*decayed)[] = &fixed` | no “compatible types” |
| `char four[4] = "abcd"` | room required for `'\0'` |
| `e_hi - e_lo`, `e_lo <= 3.7` | C++26 removed mixed enum arithmetic |
| `double _Complex`, `I`, `creal` | C complex, not `std::complex` |
| `_BitInt(8)`, `0xFFuwb` | not in C++26 |
| `_Atomic`, `_Noreturn`, `_Alignas` mid-specifiers | C specifiers |
| `typeof` / `typeof_unqual` | C++ has `decltype` |
| `_Generic` | no such operator |
| `restrict`, `[static 1]`, `[const]` | C99 parameter sugar |
| `struct Note { … }` in a parameter | types may not be defined there |
| `scratch[n]`, `buf[n]` | no VLAs |
| `malloc(...)` into `T *` | no implicit `void *` conversion |
| `char *title = "MIX"` | string literals are `const` |
| `int *mut_level = &level` | discarded `const` |
| `(double _Complex[2]){ … }` | no compound literals |
| `.x` before `.y`, `[2] =`, `.s.i` | designated-init subset |
| `goto after` over `int warmed = …` | jump across initialization |
| `lhs == rhs` | C++26 removed array comparison |

First errors C++26 actually reports: redefinition of samples, uninitialized gain, void variable, struct template exploding into a template-parse, Bool clash, incomplete Inner, overlong string, illegal enum math, unknown I / uwb / typeof / restrict, type defined in a parameter — then the compiler gives up. That is the point.
