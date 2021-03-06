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
#include <kernel/config.h>
#include "ship.h"

/* kernel includes */
#include "build.h"
#include "unit.h"
#include "item.h"
#include "race.h"
#include "region.h"
#include "skill.h"

/* util includes */
#include <util/attrib.h>
#include <util/base36.h>
#include <util/bsdstring.h>
#include <util/event.h>
#include <util/language.h>
#include <util/lists.h>
#include <util/umlaut.h>
#include <quicklist.h>
#include <util/xml.h>

#include <storage.h>

/* libc includes */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

quicklist *shiptypes = NULL;

static local_names *snames;

const ship_type *findshiptype(const char *name, const struct locale *lang)
{
  local_names *sn = snames;
  variant var;

  while (sn) {
    if (sn->lang == lang)
      break;
    sn = sn->next;
  }
  if (!sn) {
    quicklist *ql;
    int qi;

    sn = (local_names *) calloc(sizeof(local_names), 1);
    sn->next = snames;
    sn->lang = lang;

    for (qi = 0, ql = shiptypes; ql; ql_advance(&ql, &qi, 1)) {
      ship_type *stype = (ship_type *) ql_get(ql, qi);
      variant var2;
      const char *n = locale_string(lang, stype->name[0]);
      var2.v = (void *)stype;
      addtoken(&sn->names, n, var2);
    }
    snames = sn;
  }
  if (findtoken(sn->names, name, &var) == E_TOK_NOMATCH)
    return NULL;
  return (const ship_type *)var.v;
}

const ship_type *st_find(const char *name)
{
  quicklist *ql;
  int qi;

  for (qi = 0, ql = shiptypes; ql; ql_advance(&ql, &qi, 1)) {
    ship_type *stype = (ship_type *) ql_get(ql, qi);
    if (strcmp(stype->name[0], name) == 0) {
      return stype;
    }
  }
  return NULL;
}

void st_register(const ship_type * type)
{
  ql_push(&shiptypes, (void *)type);
}

#define SMAXHASH 7919
ship *shiphash[SMAXHASH];
void shash(ship * s)
{
  ship *old = shiphash[s->no % SMAXHASH];

  shiphash[s->no % SMAXHASH] = s;
  s->nexthash = old;
}

void sunhash(ship * s)
{
  ship **show;

  for (show = &shiphash[s->no % SMAXHASH]; *show; show = &(*show)->nexthash) {
    if ((*show)->no == s->no)
      break;
  }
  if (*show) {
    assert(*show == s);
    *show = (*show)->nexthash;
    s->nexthash = 0;
  }
}

static ship *sfindhash(int i)
{
  ship *old;

  for (old = shiphash[i % SMAXHASH]; old; old = old->nexthash)
    if (old->no == i)
      return old;
  return 0;
}

struct ship *findship(int i)
{
  return sfindhash(i);
}

struct ship *findshipr(const region * r, int n)
{
  ship *sh;

  for (sh = r->ships; sh; sh = sh->next) {
    if (sh->no == n) {
      assert(sh->region == r);
      return sh;
    }
  }
  return 0;
}

void damage_ship(ship * sh, double percent)
{
  double damage =
    DAMAGE_SCALE * sh->type->damage * percent * sh->size + sh->damage;
  sh->damage = (int)damage;
}

/* Alte Schiffstypen: */
static ship *deleted_ships;

ship *new_ship(const ship_type * stype, region * r, const struct locale *lang)
{
  static char buffer[32];
  ship *sh = (ship *) calloc(1, sizeof(ship));
  const char *sname = 0;

  assert(stype);
  sh->no = newcontainerid();
  sh->coast = NODIRECTION;
  sh->type = stype;
  sh->region = r;

  sname = LOC(lang, stype->name[0]);
  if (!sname) {
    sname = LOC(lang, parameters[P_SHIP]);
    if (!sname) {
      sname = parameters[P_SHIP];
    }
  }
  assert(sname);
  slprintf(buffer, sizeof(buffer), "%s %s", sname, shipid(sh));
  sh->name = _strdup(buffer);
  shash(sh);
  if (r) {
    addlist(&r->ships, sh);
  }
  return sh;
}

void remove_ship(ship ** slist, ship * sh)
{
  region *r = sh->region;
  unit *u = r->units;

  handle_event(sh->attribs, "destroy", sh);
  while (u) {
    if (u->ship == sh) {
      leave_ship(u);
    }
    u = u->next;
  }
  sunhash(sh);
  while (*slist && *slist != sh)
    slist = &(*slist)->next;
  assert(*slist);
  *slist = sh->next;
  sh->next = deleted_ships;
  deleted_ships = sh;
  sh->region = NULL;
}

void free_ship(ship * s)
{
  while (s->attribs)
    a_remove(&s->attribs, s->attribs);
  free(s->name);
  free(s->display);
  free(s);
}

void free_ships(void)
{
  while (deleted_ships) {
    ship *s = deleted_ships;
    deleted_ships = s->next;
  }
}

const char *write_shipname(const ship * sh, char *ibuf, size_t size)
{
  slprintf(ibuf, size, "%s (%s)", sh->name, itoa36(sh->no));
  return ibuf;
}

const char *shipname(const ship * sh)
{
  typedef char name[OBJECTIDSIZE + 1];
  static name idbuf[8];
  static int nextbuf = 0;
  char *ibuf = idbuf[(++nextbuf) % 8];
  return write_shipname(sh, ibuf, sizeof(name));
}

int shipcapacity(const ship * sh)
{
  int i = sh->type->cargo;

  /* sonst ist construction:: size nicht ship_type::maxsize */
  assert(!sh->type->construction
    || sh->type->construction->improvement == NULL);

  if (sh->type->construction && sh->size != sh->type->construction->maxsize)
    return 0;

#ifdef SHIPDAMAGE
  if (sh->damage) {
    i = (int)ceil(i * (1.0 - sh->damage / sh->size / (double)DAMAGE_SCALE));
  }
#endif
  return i;
}

void getshipweight(const ship * sh, int *sweight, int *scabins)
{
  unit *u;

  *sweight = 0;
  *scabins = 0;

  for (u = sh->region->units; u; u = u->next) {
    if (u->ship == sh) {
      *sweight += weight(u);
      if (sh->type->cabins) {
        int pweight = u->number * u_race(u)->weight;
        /* weight goes into number of cabins, not cargo */
        *scabins += pweight;
        *sweight -= pweight;
      }
    }
  }
}

void ship_set_owner(unit * u) {
  assert(u && u->ship);
  u->ship->_owner = u;
}

static unit * ship_owner_ex(const ship * sh, const struct faction * last_owner)
{
  unit *u, *heir = 0;

  /* Eigentümer tot oder kein Eigentümer vorhanden. Erste lebende Einheit
    * nehmen. */
  for (u = sh->region->units; u; u = u->next) {
    if (u->ship == sh) {
      if (u->number > 0) {
        if (heir && last_owner && heir->faction!=last_owner && u->faction==last_owner) {
          heir = u;
          break; /* we found someone from the same faction who is not dead. let's take this guy */
        }
        else if (!heir) {
          heir = u; /* you'll do in an emergency */
        }
      }
    }
  }
  return heir;
}

void ship_update_owner(ship * sh) {
  unit * owner = sh->_owner;
  sh->_owner = ship_owner_ex(sh, owner?owner->faction:0);
}

unit *ship_owner(const ship * sh)
{
  unit *owner = sh->_owner;
  if (!owner || (owner->ship!=sh || owner->number<=0)) {
    unit * heir = ship_owner_ex(sh, owner?owner->faction:0);
    return (heir && heir->number>0) ? heir : 0;
  }
  return owner;
}

void write_ship_reference(const struct ship *sh, struct storage *store)
{
  WRITE_INT(store, (sh && sh->region) ? sh->no : 0);
}

void ship_setname(ship * self, const char *name)
{
  free(self->name);
  if (name)
    self->name = _strdup(name);
  else
    self->name = NULL;
}

const char *ship_getname(const ship * self)
{
  return self->name;
}
