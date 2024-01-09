#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "perf_common.h"

static inline bool set_aorb(char sel, unsigned int *store, unsigned int val) {
 if (sel<'a' || 'b'<sel) return true;
 store[sel-'a']=val;
 return false;
}

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
     retv.error|=set_aorb(c2, retv.lmask, val);
     if (c3) retv.error |= true;
    }
    break;
   case 'd':
    retv.error|=set_aorb(c2, retv.diff, val);
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

static inline void print_exec_time(clock_t t_begin, clock_t t_end, const char *op, int cnt, unsigned int bits) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d BigUInt%u %s operations ===\n", cnt, bits, op);
 fprintf(stdout, " Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}

static inline void print_exec_summary(clock_t t_begin, clock_t t_end, const char *op, unsigned int bits, int cnt, const UInt *val, int valnum) {
 print_exec_time(t_begin, t_end, op, cnt, bits);
 for (unsigned int i=0; i<valnum; ++i) {
  fprintf(stdout, "  Check value #%u: %"PRIuint"\n", i+1, val[i]);
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
        LoopFunction2 internal_loop,
        unsigned int chkval_n
) {
 StandardArgs args = parse_args(argc, argv, args_init);
 int check_res = check_args_(argv[0], &args, bits, max, fun_n, funname, -1);
 if (check_res) return check_res;

 for (unsigned int ai = 0; ai<args.levels; ++ai) {
  if ((args.lmask[0] & (1<<ai))==0) continue;
  for (unsigned int bi = 0; bi<args.levels; ++bi) {
   if ((args.lmask[1] & (1<<bi))==0) continue;
   fprintf(stdout, "*** Operand levels: a #%u, b #%u ***\n", ai, bi);

   for (unsigned int fi = 0; fi < fun_n; ++fi) {
    if ((args.fmask & (1 << fi)) && !(args.fexmask & (1 << fi))) {
     UInt chkval[]={0,0};
     clock_t t0, t1;

     t0 = clock();
     unsigned int loops = internal_loop(ai, bi, fi, &args, chkval);
     t1 = clock();
     print_exec_summary(t0, t1, funname[fi], bits, loops, chkval, chkval_n);
    }
   }

  }
 }
  return 0;
}

int fun1_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init, const StandardConstraints *max,
        unsigned int fun_n, const char *funname[],
        LoopFunction1 internal_loop,
        unsigned int chkval_n
) {
 StandardArgs args = parse_args(argc, argv, args_init);
 int check_res = check_args_(argv[0], &args, bits, max, fun_n, funname, ARGMASK_ALL - ARGMASK_LMASKB - ARGMASK_DIFFB);
 if (check_res) return check_res;

 for (unsigned int ai = 0; ai<args.levels; ++ai) {
  if ((args.lmask[0] & (1<<ai))==0) continue;
  fprintf(stdout, "*** Operand level: a #%u ***\n", ai);

  for (unsigned int fi = 0; fi < fun_n; ++fi) {
   if ((args.fmask & (1 << fi)) && !(args.fexmask & (1 << fi))) {
    UInt chkval[]={0,0};
    clock_t t0, t1;

    t0 = clock();
    unsigned int loops = internal_loop(ai, fi, &args, chkval);
    t1 = clock();
    print_exec_summary(t0, t1, funname[fi], bits, loops, chkval, chkval_n);
   }
  }
 }
 return 0;
}
