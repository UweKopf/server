/* vi: set ts=2:
 +-------------------+  
 |                   |  Christian Schlittchen <corwin@amber.kn-bremen.de>
 | Eressea PBEM host |  Enno Rehling <enno@eressea.de>
 | (c) 1998 - 2004   |  Katja Zedel <katze@felidae.kn-bremen.de>
 |                   |
 +-------------------+  

 This program may not be used, modified or distributed
 without prior permission by the authors of Eressea.
*/

#ifndef KRNL_ORDER_H
#define KRNL_ORDER_H
#ifdef __cplusplus
extern "C" {
#endif

/* Encapsulation of an order
 *
 * This structure contains one order given by a unit. These used to be
 * stored in string lists, but by storing them in order-structures,
 * it is possible to use reference-counting on them, reduce string copies,
 * and reduce overall memory usage by sharing strings between orders (not
 * implemented yet) saving approx. 50% of all string-related memory.
 */

  struct order_data;

  typedef struct order {
    struct order *next;
    /* do not access this data: */
    struct order_data *data;
    int _persistent:1;
  } order;

/* constructor */
  extern order *create_order(keyword_t kwd, const struct locale *lang,
    const char *params, ...);
  extern order *parse_order(const char *s, const struct locale *lang);
  extern void replace_order(order ** dst, order * orig, const order * src);

/* reference counted copies of orders: */
  extern order *copy_order(const order * ord);
  extern void free_order(order * ord);
  extern void free_orders(order ** olist);

  extern void push_order(struct order **olist, struct order *ord);

/* access functions for orders */
  extern keyword_t get_keyword(const order * ord);
  extern void set_order(order ** destp, order * src);
  extern char *getcommand(const order * ord);
  extern bool is_persistent(const order * ord);
  extern bool is_exclusive(const order * ord);
  extern bool is_repeated(const order * ord);
  extern bool is_long(const order * ord);

  extern char *write_order(const order * ord, char *buffer, size_t size);
  extern void init_tokens(const struct order *ord);     /* initialize token parsing */

#ifdef __cplusplus
}
#endif
#endif
