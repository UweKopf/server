#ifndef ERESSEA_TESTS_H
#define ERESSEA_TESTS_H
#ifdef __cplusplus
extern "C" {
#endif
  #include <CuTest.h>
  
  void test_cleanup(void);

  struct terrain_type * test_create_terrain(const char * name, unsigned int flags);
  struct race *test_create_race(const char *name);
  struct region *test_create_region(int x, int y,
    const struct terrain_type *terrain);
  struct faction *test_create_faction(const struct race *rc);
  struct unit *test_create_unit(struct faction *f, struct region *r);
  void test_create_world(void);
  struct building * test_create_building(struct region * r, const struct building_type * btype);
  struct ship * test_create_ship(struct region * r, const struct ship_type * stype);
  struct item_type * test_create_itemtype(const char ** names);
  struct ship_type *test_create_shiptype(const char **names);
  struct building_type *test_create_buildingtype(const char *name);

  int RunAllTests(void);

#ifdef __cplusplus
}
#endif
#endif
