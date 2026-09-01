/* never_cpp.c
 *
 * Valid modern C.  Hostile to every C++ dialect, including C++26.
 *
 *   clang -std=c23 -Wall -Wextra -c never_cpp.c     # OK
 *   clang++ -std=c++26 -c never_cpp.c               # scrapyard
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <complex.h>
#include <stdbool.h>

/* tentative definitions — one object in C, redefinition in C++ */
int samples;
int samples;

/* uninitialized file-scope const */
const int gain;

/* extern void object */
extern void black_hole;

/* storage-class on a type declaration: ignored in C */
static struct MixerState;

/* C++ keywords as identifiers */
struct template {
    int class, new, delete, virtual, namespace, operator;
    int concept, requires, co_await, contract_assert;
    int typename, this, try, catch, throw;
    int explicit, mutable, friend, private, public, protected;
    struct template *next;
};

/* nested tag leaks to file scope; tag vs typedef are different namespaces */
struct Outer {
    struct Inner { int bins; } inner;
};
enum Bool { FALSE, TRUE };
typedef int Bool;
struct Inner leaked_inner;

/* flexible array member + alignment specifier among other specifiers */
struct Packet {
    int len;
    unsigned _Alignas(8) int stamp;
    char data[];
};

/* compatible types: int[3] ~ int[] */
int fixed[3];
int (*decayed)[] = &fixed;

/* string that does not leave room for '\0' */
char four[4] = "abcd";

/* mixing distinct enumerations — ill-formed in C++26 */
enum E1 { e_lo };
enum E2 { e_hi };
int enum_mix = e_hi - e_lo;
int enum_float_cmp = e_lo <= 3.7;

/* native complex, not std::complex */
double _Complex tone = 1.0 + 2.0 * I;

/* C23 _BitInt + suffix */
unsigned _BitInt(8) saturation = 0xFFuwb;

/* C11 _Atomic */
_Atomic int tick;

/* C23 typeof */
typeof(tone) spare_tone;
typeof_unqual(const int *) raw_ptr_t_demo;

/* C11 _Generic */
#define TYPE_TAG(x) _Generic((x),                 \
        int: "int",                               \
        double: "double",                         \
        double _Complex: "complex",               \
        default: "other")

/* restrict + [static N] + [const] array parameters */
static void
mix_into(double _Complex dst[restrict static 1],
         const double _Complex src[restrict const static 1],
         int n)
{
    for (int i = 0; i < n; ++i)
        dst[i] += src[i] * tone;
}

/* type defined in a parameter list */
static void
annotate(struct Note { int hz; char tag[4]; } note)
{
    (void)note;
}

/* C _Noreturn specifier */
_Noreturn static void
bail(const char *msg)
{
    fputs(msg, stderr);
    abort();
}

/* VLA parameter + block-scope VLA */
static int
energy(int n, const double _Complex buf[n])
{
    double acc = 0.0;
    double scratch[n];
    for (int i = 0; i < n; ++i) {
        scratch[i] = creal(buf[i]) * creal(buf[i])
                   + cimag(buf[i]) * cimag(buf[i]);
        acc += scratch[i];
    }
    return (int)acc;
}

int
main(void)
{
    /* implicit void * → T * */
    struct Packet *pkt = malloc(sizeof *pkt + 16);
    if (!pkt)
        bail("oom\n");
    pkt->len = 16;
    pkt->stamp = 1;

    /* discarded const on a string literal and on a pointed-to object */
    char *title = "MIX";
    const int level = 20;
    int *mut_level = &level;

    /* compound literals */
    mix_into((double _Complex[2]){ 0, 0 },
             (const double _Complex[2]){ tone, 1.0 + I },
             2);

    /* designated initializers C++ still rejects */
    struct A { int y, x; };
    struct A swapped = { .x = 9, .y = 3 };      /* out of order */
    int hist[4] = { [2] = 7, [0] = 1 };         /* array designators */
    struct B { struct { int i; } s; int k; };
    struct B nest = { .s.i = 4, .k = 5 };       /* nested designator */

    /* goto across an initialized declaration */
    if (pkt->len == 0)
        goto after;
    int warmed = *mut_level + swapped.x + hist[2] + nest.k;
    after:

    /* array comparison: pointers in C, removed in C++26 */
    int lhs[3] = { 1, 2, 3 };
    int rhs[3] = { 1, 2, 3 };
    int same = (lhs == rhs);

    struct template node = {
        .class = 1, .new = 2, .virtual = 3, .namespace = 4,
        .operator = 5, .concept = 6, .requires = 7,
        .co_await = 8, .contract_assert = 9, .this = 10,
        .next = NULL
    };
    leaked_inner.bins = node.class;

    int n = 4;
    double _Complex *buf = malloc((size_t)n * sizeof *buf);
    if (!buf)
        bail("oom\n");
    for (int i = 0; i < n; ++i)
        buf[i] = tone * (double)(i + 1);

    int e = energy(n, buf);
    Bool flag = (Bool)(e > 0);

    (void)flag; (void)warmed; (void)same; (void)title; (void)four;
    (void)gain; (void)samples; (void)decayed; (void)saturation;
    (void)spare_tone; (void)enum_mix; (void)enum_float_cmp;
    (void)&black_hole; (void)&annotate; (void)tick;

    printf("tag=%s energy=%d inner=%d\n",
           TYPE_TAG(tone), e, leaked_inner.bins);

    free(buf);
    free(pkt);
    return 0;
}
