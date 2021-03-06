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
#include "language.h"
#include "language_struct.h"

#include "log.h"
#include "goodies.h"
#include "umlaut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/** importing **/

locale *default_locale;
locale *locales;

unsigned int locale_index(const locale * lang)
{
  assert(lang);
  return lang->index;
}

locale *find_locale(const char *name)
{
  unsigned int hkey = hashstring(name);
  locale *l = locales;
  while (l && l->hashkey != hkey)
    l = l->next;
  return l;
}

static unsigned int nextlocaleindex = 0;

locale *make_locale(const char *name)
{
  unsigned int hkey = hashstring(name);
  locale *l = (locale *) calloc(sizeof(locale), 1);
  locale **lp = &locales;

  if (!locales) {
    nextlocaleindex = 0;
  }

  while (*lp && (*lp)->hashkey != hkey)
    lp = &(*lp)->next;
  if (*lp) {
    return *lp;
  }

  l->hashkey = hkey;
  l->name = _strdup(name);
  l->next = NULL;
  l->index = nextlocaleindex++;
  assert(nextlocaleindex <= MAXLOCALES);
  *lp = l;
  if (default_locale == NULL)
    default_locale = l;
  return l;
}

/** creates a list of locales
 * This function takes a comma-delimited list of locale-names and creates
 * the locales using the make_locale function (useful for ini-files).
 * For maximum performance, locales should be created in order of popularity.
 */
void make_locales(const char *str)
{
  const char *tok = str;
  while (*tok) {
    char zText[32];
    while (*tok && *tok != ',')
      ++tok;
    strncpy(zText, str, tok - str);
    zText[tok - str] = 0;
    make_locale(zText);
    if (*tok) {
      str = ++tok;
    }
  }
}

const char *locale_getstring(const locale * lang, const char *key)
{
  unsigned int hkey = hashstring(key);
  unsigned int id = hkey & (SMAXHASH - 1);
  const struct locale_str *find;

  assert(lang);
  if (key == NULL || *key == 0)
    return NULL;
  find = lang->strings[id];
  while (find) {
    if (find->hashkey == hkey) {
      if (find->nexthash == NULL) {
        /* if this is the only entry with this hash, fine. */
        assert(strcmp(key, find->key) == 0);
        return find->str;
      }
      if (strcmp(key, find->key) == 0) {
        return find->str;
      }
    }
    find = find->nexthash;
  }
  return NULL;
}

const char *locale_string(const locale * lang, const char *key)
{
  assert(lang);

  if (key != NULL) {
    unsigned int hkey = hashstring(key);
    unsigned int id = hkey & (SMAXHASH - 1);
    struct locale_str *find;

    if (*key == 0)
      return NULL;
    find = lang->strings[id];
    while (find) {
      if (find->hashkey == hkey) {
        if (find->nexthash == NULL) {
          /* if this is the only entry with this hash, fine. */
          assert(strcmp(key, find->key) == 0);
          break;
        }
        if (strcmp(key, find->key) == 0)
          break;
      }
      find = find->nexthash;
    }
    if (!find) {
      log_warning("missing translation for \"%s\" in locale %s\n", key, lang->name);
      if (lang->fallback) {
        return locale_string(lang->fallback, key);
      }
      return 0;
    }
    return find->str;
  }
  return NULL;
}

void locale_setstring(locale * lang, const char *key, const char *value)
{
  unsigned int hkey = hashstring(key);
  unsigned int id = hkey & (SMAXHASH - 1);
  struct locale_str *find;
  if (!lang) {
    lang = default_locale;
  }
  assert(lang);
  find = lang->strings[id];
  while (find) {
    if (find->hashkey == hkey && strcmp(key, find->key) == 0)
      break;
    find = find->nexthash;
  }
  if (!find) {
    find = calloc(1, sizeof(struct locale_str));
    find->nexthash = lang->strings[id];
    lang->strings[id] = find;
    find->hashkey = hkey;
    find->key = _strdup(key);
    find->str = _strdup(value);
  } else {
    if (strcmp(find->str, value) != 0) {
      log_error("duplicate translation '%s' for key %s\n", value, key);
    }
    assert(!strcmp(find->str, value) || !"duplicate string for key");
  }
}

const char *locale_name(const locale * lang)
{
  assert(lang);
  return lang->name;
}

char *mkname_buf(const char *space, const char *name, char *buffer)
{
  if (space && *space) {
    sprintf(buffer, "%s::%s", space, name);
  } else {
    strcpy(buffer, name);
  }
  return buffer;
}

const char *mkname(const char *space, const char *name)
{
  static char zBuffer[128];     /* STATIC_RESULT: used for return, not across calls */
  return mkname_buf(space, name, zBuffer);
}

locale *nextlocale(const struct locale * lang)
{
  return lang->next;
}

typedef struct lstr {
  void * tokens[UT_MAX];
} lstr;

static lstr lstrs[MAXLOCALES];

void ** get_translations(const struct locale *lang, int index)
{
  assert(lang);
  assert(lang->index < MAXLOCALES
    || "you have to increase MAXLOCALES and recompile");
  if (lang->index < MAXLOCALES) {
    return lstrs[lang->index].tokens + index;
  }
  return lstrs[0].tokens + index;
}

void free_locales(void)
{
  while (locales) {
    int i;
    locale * next = locales->next;

    for (i=0; i!=SMAXHASH; ++i) {
      while (locales->strings[i]) {
        struct locale_str * strings = locales->strings[i];
        free(strings->key);
        free(strings->str);
        locales->strings[i] = strings->nexthash;
        free(strings);
      }
    }
    free(locales);
    locales = next;
  }
}