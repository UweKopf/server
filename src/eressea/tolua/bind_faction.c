/* vi: set ts=2:
+-------------------+
|                   |  Enno Rehling <enno@eressea.de>
| Eressea PBEM host |  Christian Schlittchen <corwin@amber.kn-bremen.de>
| (c) 1998 - 2008   |  Katja Zedel <katze@felidae.kn-bremen.de>
|                   |  Henning Peters <faroul@beyond.kn-bremen.de>
+-------------------+

This program may not be used, modified or distributed
without prior permission by the authors of Eressea.
*/

#include <config.h>

#include <kernel/unit.h>
#include <kernel/faction.h>

#include <lua.h>
#include <tolua.h>

int tolua_factionlist_next(lua_State *tolua_S)
{
  faction** faction_ptr = (faction **)lua_touserdata(tolua_S, lua_upvalueindex(1));
  faction * f = *faction_ptr;
  if (f != NULL) {
    tolua_pushusertype(tolua_S, (void*)f, "faction");
    *faction_ptr = f->next;
    return 1;
  }
  else return 0;  /* no more values to return */
}

int tolua_factionlist_iter(lua_State *tolua_S)
{
  faction_list** faction_ptr = (faction_list **)lua_touserdata(tolua_S, lua_upvalueindex(1));
  faction_list* flist = *faction_ptr;
  if (flist != NULL) {
    tolua_pushusertype(tolua_S, (void*)flist->data, "faction");
    *faction_ptr = flist->next;
    return 1;
  }
  else return 0;  /* no more values to return */
}

static int tolua_faction_get_units(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  unit ** unit_ptr = (unit**)lua_newuserdata(tolua_S, sizeof(unit *));

  luaL_getmetatable(tolua_S, "unit");
  lua_setmetatable(tolua_S, -2);

  *unit_ptr = self->units;

  lua_pushcclosure(tolua_S, tolua_unitlist_nextf, 1);
  return 1;
}

static int
tolua_faction_get_maxheroes(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)maxheroes(self));
  return 1;
}

static int
tolua_faction_get_heroes(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)countheroes(self));
  return 1;
}

static int
tolua_faction_get_score(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->score);
  return 1;
}

static int
tolua_faction_get_id(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->no);
  return 1;
}

static int
tolua_faction_get_age(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->age);
  return 1;
}

static int
tolua_faction_get_flags(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->flags);
  return 1;
}

static int
tolua_faction_get_options(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->options);
  return 1;
}

static int
tolua_faction_get_lastturn(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  tolua_pushnumber(tolua_S, (lua_Number)self->lastorders);
  return 1;
}

static int
tolua_faction_renumber(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  int no = (int)tolua_tonumber(tolua_S, 2, 0);

  renumber_faction(self, no);
  return 0;
}

static int
tolua_faction_get_policy(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  faction * other = (faction *)tolua_tousertype(tolua_S, 2, 0);
  const char * policy = tolua_tostring(tolua_S, 3, 0);

  int result = 0, mode;
  for (mode=0;helpmodes[mode].name!=NULL;++mode) {
    if (strcmp(policy, helpmodes[mode].name)==0) {
      result = get_alliance(self, other) & mode;
      break;
    }
  }

  tolua_pushnumber(tolua_S, (lua_Number)result);
  return 1;
}


static int
tolua_faction_set_policy(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);
  faction * other = (faction *)tolua_tousertype(tolua_S, 2, 0);
  const char * policy = tolua_tostring(tolua_S, 3, 0);
  int value = tolua_toboolean(tolua_S, 4, 0);

  int mode;
  for (mode=0;helpmodes[mode].name!=NULL;++mode) {
    if (strcmp(policy, helpmodes[mode].name)==0) {
      if (value) {
        set_alliance(self, other, get_alliance(self, other) | helpmodes[mode].status);
      } else {
        set_alliance(self, other, get_alliance(self, other) & ~helpmodes[mode].status);
      }
      break;
    }
  }

  return 0;
}

static int
tolua_faction_get_origin(lua_State* tolua_S)
{
  faction * self = (faction *)tolua_tousertype(tolua_S, 1, 0);

  ursprung * origin = self->ursprung;
  int x, y;
  while (origin!=NULL && origin->id!=0) {
    origin = origin->next;
  }
  if (origin) {
    x = origin->x;
    y = origin->y;
  } else {
    x = 0;
    y = 0;
  }

  tolua_pushnumber(tolua_S, (lua_Number)x);
  tolua_pushnumber(tolua_S, (lua_Number)y);
  return 2;
}

static int
tolua_faction_create(lua_State* tolua_S)
{
  const char * email = tolua_tostring(tolua_S, 1, 0);
  const char * racename = tolua_tostring(tolua_S, 2, 0);
  const char * lang = tolua_tostring(tolua_S, 3, 0);
  struct locale * loc = find_locale(lang);
  faction * f = NULL;
  const struct race * frace = findrace(racename, default_locale);
  if (frace==NULL) frace = findrace(racename, find_locale("de"));
  if (frace==NULL) frace = findrace(racename, find_locale("en"));
  if (frace!=NULL) {
    f = addfaction(email, NULL, frace, loc, 0);
  }
  tolua_pushusertype(tolua_S, f, "faction");
  return 1;
}

void
tolua_faction_open(lua_State* tolua_S)
{
  /* register user types */
  tolua_usertype(tolua_S, "faction");
  tolua_usertype(tolua_S, "faction_list");

  tolua_module(tolua_S, NULL, 0);
  tolua_beginmodule(tolua_S, NULL);
  {
    tolua_cclass(tolua_S, "faction", "faction", "", NULL);
    tolua_beginmodule(tolua_S, "faction");
    {
      tolua_variable(tolua_S, "name", tolua_faction_get_name, tolua_faction_set_name);
      tolua_variable(tolua_S, "units", tolua_faction_get_units, NULL);
      tolua_variable(tolua_S, "heroes", tolua_faction_get_heroes, NULL);
      tolua_variable(tolua_S, "maxheroes", tolua_faction_get_maxheroes, NULL);
      tolua_variable(tolua_S, "password", tolua_faction_get_password, tolua_faction_set_password);
      tolua_variable(tolua_S, "email", tolua_faction_get_email, tolua_faction_set_email);
      tolua_variable(tolua_S, "locale", tolua_faction_get_locale, tolua_faction_set_locale);
      tolua_variable(tolua_S, "race", tolua_faction_get_race, tolua_faction_set_race);
      tolua_variable(tolua_S, "alliance", tolua_faction_get_alliance, tolua_faction_set_alliance);
      tolua_variable(tolua_S, "score", tolua_faction_get_score, NULL);
      tolua_variable(tolua_S, "id", tolua_faction_get_id, NULL);
      tolua_variable(tolua_S, "age", tolua_faction_get_age, NULL);
      tolua_variable(tolua_S, "options", tolua_faction_get_options, NULL);
      tolua_variable(tolua_S, "flags", tolua_faction_get_flags, NULL);
      tolua_variable(tolua_S, "lastturn", tolua_faction_get_lastturn, NULL);
 
      tolua_function(tolua_S, "set_policy", tolua_faction_set_policy);
      tolua_function(tolua_S, "get_policy", tolua_faction_get_policy);
      tolua_function(tolua_S, "get_origin", tolua_faction_get_origin);

      tolua_function(tolua_S, "renumber", tolua_faction_renumber);
      tolua_function(tolua_S, "create", tolua_faction_create);
    }
    tolua_endmodule(tolua_S);
  }
  tolua_endmodule(tolua_S);
}
