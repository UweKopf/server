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
#include "giveitem.h"
#include <kernel/config.h>

/* kernel includes */
#include <kernel/building.h>
#include <kernel/region.h>
#include <kernel/unit.h>
#include <kernel/item.h>

/* util includes */
#include <util/attrib.h>
#include <util/base36.h>
#include <util/resolve.h>
#include <util/goodies.h>

#include <storage.h>

/* libc includes */
#include <stdlib.h>
#include <string.h>

typedef struct give_data {
  struct building *building;
  struct item *items;
} give_data;

static void
a_writegive(const attrib * a, const void *owner, struct storage *store)
{
  give_data *gdata = (give_data *) a->data.v;
  item *itm;
  write_building_reference(gdata->building, store);
  for (itm = gdata->items; itm; itm = itm->next) {
    WRITE_TOK(store, resourcename(itm->type->rtype, 0));
    WRITE_INT(store, itm->number);
  }
  WRITE_TOK(store, "end");
}

static int a_readgive(attrib * a, void *owner, struct storage *store)
{
  give_data *gdata = (give_data *) a->data.v;
  variant var;
  char zText[32];

  READ_INT(store, &var.i);
  if (var.i > 0) {
    gdata->building = findbuilding(var.i);
    if (gdata->building == NULL) {
      ur_add(var, &gdata->building, resolve_building);
    }
  } else {
    gdata->building = NULL;
  }
  for (;;) {
    int i;
    READ_TOK(store, zText, sizeof(zText));
    if (!strcmp("end", zText))
      break;
    READ_INT(store, &i);
    if (i == 0)
      i_add(&gdata->items, i_new(it_find(zText), i));
  }
  return AT_READ_OK;
}

static void a_initgive(struct attrib *a)
{
  a->data.v = calloc(sizeof(give_data), 1);
}

static void a_finalizegive(struct attrib *a)
{
  free(a->data.v);
}

static int a_giveitem(attrib * a)
{
  give_data *gdata = (give_data *) a->data.v;
  unit *u;

  if (gdata->building == NULL || gdata->items == NULL)
    return 0;
  u = building_owner(gdata->building);
  if (u == NULL)
    return 1;
  while (gdata->items) {
    item *itm = gdata->items;
    i_change(&u->items, itm->type, itm->number);
    i_free(i_remove(&gdata->items, itm));
  }
  return 0;
}

attrib_type at_giveitem = {
  "giveitem",
  a_initgive, a_finalizegive,
  a_giveitem,
  a_writegive, a_readgive
};

attrib *make_giveitem(struct building * b, struct item * ip)
{
  attrib *a = a_new(&at_giveitem);
  give_data *gd = (give_data *) a->data.v;
  gd->building = b;
  gd->items = ip;
  return a;
}

void init_giveitem(void)
{
  at_register(&at_giveitem);
}
