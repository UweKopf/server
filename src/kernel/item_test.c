#include <platform.h>

#include <kernel/types.h>
#include <kernel/item.h>
#include <kernel/pool.h>
#include <kernel/unit.h>
#include <util/language.h>
#include <util/functions.h>

#include <CuTest.h>
#include <tests.h>

static void test_uchange(CuTest * tc, unit * u, const resource_type * rtype) {
  int n;
  change_resource(u, rtype, 4);
  n = get_resource(u, rtype);
  CuAssertPtrNotNull(tc, rtype->uchange);
  CuAssertIntEquals(tc, n, rtype->uchange(u, rtype, 0));
  CuAssertIntEquals(tc, n-3, rtype->uchange(u, rtype, -3));
  CuAssertIntEquals(tc, n-3, get_resource(u, rtype));
  CuAssertIntEquals(tc, 0, rtype->uchange(u, rtype, -n));
}

void test_change_item(CuTest * tc)
{
  unit * u;

  test_cleanup();
  register_resources();
  init_resources();
  test_create_world();

  u = test_create_unit(0, 0);
  test_uchange(tc, u, olditemtype[I_IRON]->rtype);
}

void test_change_person(CuTest * tc)
{
  unit * u;

  test_cleanup();

  register_resources();
  init_resources();
  test_create_world();

  u = test_create_unit(0, 0);
  test_uchange(tc, u, r_unit);
}

void test_resource_type(CuTest * tc)
{
  struct item_type *itype;
  const char *names[2] = { 0 , 0 };

  test_cleanup();

  CuAssertPtrEquals(tc, 0, rt_find("herpderp"));

  names[0] = names[1] = "herpderp";
  test_create_itemtype(names);

  names[0] = names[1] = "herp";
  itype = test_create_itemtype(names);

  names[0] = names[1] = "herpes";
  test_create_itemtype(names);

  CuAssertPtrEquals(tc, itype, it_find("herp"));
  CuAssertPtrEquals(tc, itype->rtype, rt_find("herp"));
}

void test_finditemtype(CuTest * tc)
{
  const item_type *itype, *iresult;
  struct locale * lang;

  test_cleanup();
  test_create_world();

  lang = find_locale("de");
  locale_setstring(lang, "horse", "Pferd");
  itype = it_find("horse");
  iresult = finditemtype("Pferd", lang);
  CuAssertPtrNotNull(tc, iresult);
  CuAssertPtrEquals(tc, (void*)itype, (void*)iresult);
}

void test_findresourcetype(CuTest * tc)
{
  const resource_type *rtype, *rresult;
  struct locale * lang;

  test_cleanup();
  test_create_world();

  lang = find_locale("de");
  locale_setstring(lang, "horse", "Pferd");
  locale_setstring(lang, "peasant", "Bauer");

  rtype = rt_find("horse");
  rresult = findresourcetype("Pferd", lang);
  CuAssertPtrNotNull(tc, rresult);
  CuAssertPtrEquals(tc, (void*)rtype, (void*)rresult);

  CuAssertPtrNotNull(tc, findresourcetype("Bauer", lang));
}

CuSuite *get_item_suite(void)
{
  CuSuite *suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_change_item);
  SUITE_ADD_TEST(suite, test_change_person);
  SUITE_ADD_TEST(suite, test_resource_type);
  SUITE_ADD_TEST(suite, test_finditemtype);
  SUITE_ADD_TEST(suite, test_findresourcetype);
  return suite;
}
