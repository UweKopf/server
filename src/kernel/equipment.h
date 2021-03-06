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

#ifndef H_KRNL_EQUIPMENT_H
#define H_KRNL_EQUIPMENT_H
#ifdef __cplusplus
extern "C" {
#endif
  struct spell;

  typedef struct itemdata {
    const struct item_type *itype;
    char *value;
    struct itemdata *next;
  } itemdata;

  typedef struct subsetitem {
    struct equipment *set;
    float chance;
  } subsetitem;

  typedef struct subset {
    float chance;
    subsetitem *sets;
  } subset;

  typedef struct equipment {
    char *name;
    struct itemdata *items;
    char *skills[MAXSKILLS];
    struct spellbook *spellbook;
    struct subset *subsets;
    struct equipment *next;
    void (*callback) (const struct equipment *, struct unit *);
  } equipment;

  extern struct equipment *create_equipment(const char *eqname);
  extern struct equipment *get_equipment(const char *eqname);

  extern void equipment_setitem(struct equipment *eq,
    const struct item_type *itype, const char *value);
  extern void equipment_setskill(struct equipment *eq, skill_t sk,
    const char *value);
  extern void equipment_addspell(struct equipment *eq, struct spell *sp, int level);
  extern void equipment_setcallback(struct equipment *eq,
    void (*callback) (const struct equipment *, struct unit *));

  extern void equip_unit(struct unit *u, const struct equipment *eq);
#define EQUIP_SKILLS  (1<<1)
#define EQUIP_SPELLS  (1<<2)
#define EQUIP_ITEMS   (1<<3)
#define EQUIP_SPECIAL (1<<4)
#define EQUIP_ALL     (0xFF)
  extern void equip_unit_mask(struct unit *u, const struct equipment *eq,
    int mask);
  extern void equip_items(struct item **items, const struct equipment *eq);

#ifdef __cplusplus
}
#endif
#endif
