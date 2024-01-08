#include <stdlib.h>
#include <stdio.h>

#include "perf_common.h"

StandardArgs parse_args(int argc, const char *argv[], const StandardArgs res_init) {
 StandardArgs retv = res_init;
 int argi = 1;
 while (argi < argc) {
  if (argv[argi][0]!='-') {
   retv.error |= true;
   return retv;
  }
  int val = 0;
  int arginc = 1;
  char c1 = argv[argi][1];
  char c2 = c1?argv[argi][2]:c1;
  char c3 = c2?argv[argi][3]:c2;
  if (c1 != 'h') { // option -h is exceptional
   val = argi+1<argc?atoi(argv[argi + 1]):0;
   if (!(argi+1<argc)) retv.error |= true;
   arginc = 2;
  }
  switch (c1) {
   case 'n':
    retv.loops = val;
    if (c2) retv.error |= true;
    break;
   case 'h':
    retv.help = true;
    if (c2) retv.error |= true;
    break;
   case 'l':
    if (c2==0) {
     retv.levels = val;
    } else {
     if (c2=='a') {
      retv.lmask_a = val;
     } else if (c2=='b') {
      retv.lmask_b = val;
     } else {
      retv.error |= true;
     }
     if (c3) retv.error |= true;
    }
    break;
   case 'd':
    if (c2=='a') {
     retv.diff_a = val;
    } else if (c2=='b') {
     retv.diff_b = val;
    } else {
     retv.error |= true;
    }
    if (c3) retv.error |= true;
    break;
   case 'f':
    retv.fmask = val;
    if (c2) retv.error |= true;
    break;
   case 'F':
    retv.fexmask = val;
    if (c2) retv.error |= true;
    break;
   default:
    retv.error |= true;
  }
  argi+=arginc;
 }
 return retv;
}

void print_help(const char *prgname, unsigned int argmask) {
 static const char *OPTSEP=" ";
 fprintf(stderr, "Usage: %s",prgname);
 if (argmask&ARGMASK_LOOPS) fprintf(stderr, "%s[-n <loops>]", OPTSEP);
 if (argmask&ARGMASK_LEVELS) fprintf(stderr, "%s[-l <operand levels>]", OPTSEP);
 if (argmask&ARGMASK_LMASKA) fprintf(stderr, "%s[-la <level mask of operand a>]", OPTSEP);
 if (argmask&ARGMASK_LMASKB) fprintf(stderr, "%s[-lb <level mask of operand b>]", OPTSEP);
 if (argmask&ARGMASK_DIFFA) fprintf(stderr, "%s[-da <step of operand a>]", OPTSEP);
 if (argmask&ARGMASK_DIFFB) fprintf(stderr, "%s[-db <step of operand b>]", OPTSEP);
 if (argmask&ARGMASK_FMASK) fprintf(stderr, "%s[-f <mask of included function checks>]", OPTSEP);
 if (argmask&ARGMASK_FEXMASK) fprintf(stderr, "%s[-F <mask of excluded function checks>]", OPTSEP);
 fprintf(stderr, "%s[-h]\n", OPTSEP);
}

void print_help_all(const char *prgname, unsigned int bits, unsigned int argmask, unsigned int fun_n, const char *funname[]) {
 print_help(prgname, argmask);
 fprintf(stderr, "where function check bitmask bits mean\n");
 for (unsigned int i=0; i<fun_n; ++i) {
  fprintf(stderr, " %6u: biguint%u_%s()\n", 1U<<i, bits, funname[i]);
 }
}

static inline int check_args_(const char *prgname, StandardArgs *args,
   unsigned int bits,
   const StandardConstraints *max,
   unsigned int fun_n, const char *funname[],
   unsigned int argmask) {
 if (args->help) {
  print_help_all(prgname, bits, argmask, fun_n, funname);
  return 0;
 }
 if (args->error) {
  print_help_all(prgname, bits, argmask, fun_n, funname);
  return 1;
 }
 if (max->levels < args->levels) {
  fprintf(stderr, "The maximal number of levels is %u.\n", max->levels);
  args->levels = max->levels;
 }
 if (args->levels < 1) {
  fprintf(stderr, "The minimal number of levels is 1.\n");
  args->levels = 1;
 }
 if (max->loops < args->loops) {
  fprintf(stderr, "The maximal number of looops is %u.\n", max->loops);
  args->loops = max->loops;
 }
 return 0;
}

int fun2_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init, const StandardConstraints *max,
        unsigned int fun_n, const char *funname[],
        void (*internal_loop)(unsigned int ai, unsigned int bi, unsigned int funidx, const StandardArgs *args)) {
 StandardArgs args = parse_args(argc, argv, args_init);
 int check_res = check_args_(argv[0], &args, bits, max, fun_n, funname, -1);
 if (check_res) return check_res;

 for (unsigned int ai = 0; ai<args.levels; ++ai) {
  if ((args.lmask_a & (1<<ai))==0) continue;
  for (unsigned int bi = 0; bi<args.levels; ++bi) {
   if ((args.lmask_b & (1<<bi))==0) continue;
   fprintf(stderr, "*** Operand levels: a #%u, b #%u ***\n", ai, bi);

   for (unsigned int fi = 0; fi < fun_n; ++fi) {
    if ((args.fmask & (1<<fi)) && !(args.fexmask & (1<<fi))) {
     internal_loop(ai, bi, fi, &args);
    }
   }

  }
 }
  return 0;
}

int fun1_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init, const StandardConstraints *max,
        unsigned int fun_n, const char *funname[],
        void (*internal_loop)(unsigned int ai, unsigned int funidx, const StandardArgs *args)) {
 StandardArgs args = parse_args(argc, argv, args_init);
 int check_res = check_args_(argv[0], &args, bits, max, fun_n, funname, ARGMASK_ALL - ARGMASK_LMASKB - ARGMASK_DIFFB);
 if (check_res) return check_res;

 for (unsigned int ai = 0; ai<args.levels; ++ai) {
  if ((args.lmask_a & (1<<ai))==0) continue;
  fprintf(stderr, "*** Operand level: a #%u ***\n", ai);

  for (unsigned int fi = 0; fi < fun_n; ++fi) {
   if ((args.fmask & (1<<fi)) && !(args.fexmask & (1<<fi))) {
    internal_loop(ai, fi, &args);
   }
  }
 }
 return 0;
}
