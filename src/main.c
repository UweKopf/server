/*
Copyright (c) 1998-2010, Enno Rehling <enno@eressea.de>
                         Katja Zedel <katze@felidae.kn-bremen.de
                         Christian Schlittchen <corwin@amber.kn-bremen.de>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**/

#include <platform.h>
#include <util/log.h>

#include <kernel/config.h>
#include <kernel/types.h>
#include <kernel/save.h>
#include <kernel/version.h>
#include "eressea.h"
#include "gmtool.h"

#include "bindings.h"
#include "races/races.h"
#include "spells/spells.h"
#include "spells/borders.h"

#include <lua.h>
#include <assert.h>
#include <locale.h>
#include <wctype.h>
#include <iniparser.h>

static const char *logfile= "eressea.log";
static const char *luafile = 0;
static const char *entry_point = NULL;
static const char *inifile = "eressea.ini";
static int memdebug = 0;

static void parse_config(const char *filename)
{
  dictionary *d = iniparser_new(filename);
  if (d) {
    load_inifile(d);
    log_debug("reading from configuration file %s\n", filename);

    memdebug = iniparser_getint(d, "eressea:memcheck", memdebug);
    entry_point = iniparser_getstring(d, "eressea:run", entry_point);
    luafile = iniparser_getstring(d, "eressea:load", luafile);

    /* only one value in the [editor] section */
    force_color = iniparser_getint(d, "editor:color", force_color);

    /* excerpt from [config] (the rest is used in bindings.c) */
    game_name = iniparser_getstring(d, "config:game", game_name);
  } else {
    log_warning("could not open configuration file %s\n", filename);
  }
  global.inifile = d;
}

static int usage(const char *prog, const char *arg)
{
  if (arg) {
    fprintf(stderr, "unknown argument: %s\n\n", arg);
  }
  fprintf(stderr, "Usage: %s [options]\n"
    "-t <turn>        : read this datafile, not the most current one\n"
    "-q               : be quite (same as -v 0)\n"
    "-v <level>       : verbosity level\n"
    "-C               : run in interactive mode\n"
    "--color          : force curses to use colors even when not detected\n", prog);
  return -1;
}

static int get_arg(int argc, char **argv, size_t len, int index, const char **result, const char *def) {
  if (argv[index][len]) {
    *result = argv[index]+len;
    return index;
  }
  if (index+1 < argc) {
    *result = argv[index+1];
    return index+1;
  }
  *result = def;
  return index;
}

static int parse_args(int argc, char **argv, int *exitcode)
{
  int i;

  for (i = 1; i != argc; ++i) {
    if (argv[i][0] != '-') {
      return usage(argv[0], argv[i]);
    } else if (argv[i][1] == '-') {     /* long format */
      if (strcmp(argv[i] + 2, "version") == 0) {
        printf("\n%s PBEM host\n"
          "Copyright (C) 1996-2005 C. Schlittchen, K. Zedel, E. Rehling, H. Peters.\n\n"
          "Compilation: " __DATE__ " at " __TIME__ "\nVersion: %d\n\n",
          global.gamename, RELEASE_VERSION);
      } else if (strcmp(argv[i] + 2, "color") == 0) {
        /* force the editor to have colors */
        force_color = 1;
      } else if (strcmp(argv[i] + 2, "help") == 0) {
        return usage(argv[0], NULL);
      } else {
        return usage(argv[0], argv[i]);
      }
    } else {
      const char *arg;
      switch (argv[i][1]) {
      case 'C':
        entry_point = NULL;
        break;
      case 'l':
        i = get_arg(argc, argv, 2, i, &logfile, 0);
        break;
      case 'e':
        i = get_arg(argc, argv, 2, i, &entry_point, 0);
        break;
      case 't':
        i = get_arg(argc, argv, 2, i, &arg, 0);
        turn = atoi(arg);
        break;
      case 'q':
        verbosity = 0;
        break;
      case 'r':
        entry_point = "run_turn";
        i = get_arg(argc, argv, 2, i, &arg, 0);
        turn = atoi(arg);
        break;
      case 'v':
        i = get_arg(argc, argv, 2, i, &arg, 0);
        verbosity = arg ? atoi(arg) : 0xff;
        break;
      case 'h':
        usage(argv[0], NULL);
        return 1;
      default:
        *exitcode = -1;
        usage(argv[0], argv[i]);
        return 1;
      }
    }
  }
  
  switch (verbosity) {
  case 0:
    log_stderr = 0;
    break;
  case 1:
    log_stderr = LOG_CPERROR;
    break;
  case 2:
    log_stderr = LOG_CPERROR|LOG_CPWARNING;
    break;
  case 3:
    log_stderr = LOG_CPERROR|LOG_CPWARNING|LOG_CPDEBUG;
    break;
  default:
    log_stderr = LOG_CPERROR|LOG_CPWARNING|LOG_CPDEBUG|LOG_CPINFO;
    break;
  }

  return 0;
}

#if defined(HAVE_SIGACTION) && defined(HAVE_EXECINFO)
#include <execinfo.h>
#include <signal.h>

static void report_segfault(int signo, siginfo_t * sinf, void *arg)
{
  void *btrace[50];
  size_t size;
  int fd = fileno(stderr);

  fflush(stdout);
  fputs("\n\nProgram received SIGSEGV, backtrace follows.\n", stderr);
  size = backtrace(btrace, 50);
  backtrace_symbols_fd(btrace, size, fd);
  abort();
}

static int setup_signal_handler(void)
{
  struct sigaction act;

  act.sa_flags = SA_ONESHOT | SA_SIGINFO;
  act.sa_sigaction = report_segfault;
  sigfillset(&act.sa_mask);
  return sigaction(SIGSEGV, &act, NULL);
}
#else
static int setup_signal_handler(void)
{
  return 0;
}
#endif

#undef CRTDBG
#ifdef CRTDBG
#include <crtdbg.h>
void init_crtdbg(void)
{
#if (defined(_MSC_VER))
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  if (memdebug == 1) {
    flags |= _CRTDBG_CHECK_ALWAYS_DF;   /* expensive */
  } else if (memdebug == 2) {
    flags = (flags & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;
  } else if (memdebug == 3) {
    flags = (flags & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_128_DF;
  } else if (memdebug == 4) {
    flags = (flags & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_1024_DF;
  }
  _CrtSetDbgFlag(flags);
#endif
}
#endif

void locale_init(void)
{
  setlocale(LC_CTYPE, "");
  setlocale(LC_NUMERIC, "C");
  if (towlower(0xC4) != 0xE4) { /* &Auml; => &auml; */
    log_error("Umlaut conversion is not working properly. Wrong locale? LANG=%s\n",
        getenv("LANG"));
  }
}

extern void bind_monsters(struct lua_State *L);

int main(int argc, char **argv)
{
  int err, result = 0;
  lua_State *L;

  setup_signal_handler();
  parse_config(inifile);
  log_open(logfile);

  err = parse_args(argc, argv, &result);
  if (err) {
    return result;
  }

  locale_init();

#ifdef CRTDBG
  init_crtdbg();
#endif

  L = lua_init();
  game_init();
  register_races();
  register_borders();
  register_spells();
  bind_monsters(L);

  err = eressea_run(L, luafile, entry_point);
  if (err) {
    log_error("server execution failed with code %d\n", err);
    return err;
  }
#ifdef MSPACES
  malloc_stats();
#endif

  game_done();
  lua_done(L);
  log_close();
  if (global.inifile) {
    iniparser_free(global.inifile);
  }
  return 0;
}
