/* vi: set ts=2:
 *
 *	Eressea PB(E)M host Copyright (C) 1998-2000
 *      Christian Schlittchen (corwin@amber.kn-bremen.de)
 *      Katja Zedel (katze@felidae.kn-bremen.de)
 *      Henning Peters (faroul@beyond.kn-bremen.de)
 *      Enno Rehling (enno@eressea-pbem.de)
 *      Ingo Wilken (Ingo.Wilken@informatik.uni-oldenburg.de)
 *
 *  based on:
 *
 * Atlantis v1.0  13 September 1993 Copyright 1993 by Russell Wallace
 * Atlantis v1.7                    Copyright 1996 by Alex Schr�der
 *
 * This program may not be used, modified or distributed without
 * prior permission by the authors of Eressea.
 * This program may not be sold or used commercially without prior written
 * permission from the authors.
 */

#ifndef ERESSEA_H
#define ERESSEA_H

/*
 * Features enabled: 
 * If you are lacking the settings.h, create a new file common/settings.h,
 * and write #include <settings-eressea.h> (or whatever settings you want
 * your game to use) in there.
 * !!! DO NOT COMMIT THE SETTINGS.H FILE TO CVS !!!
 */
#include <settings.h>

/* Features currently in development (change makefile please): */
/* #define MSG_LEVELS -- msg-levels active */

/* basic types used in the eressea "kernel" */
typedef unsigned char order_t;
typedef char terrain_t;
typedef char direction_t;
typedef int race_t;
typedef int magic_t;
typedef short skill_t;
typedef short typ_t;
typedef int herb_t;
typedef int potion_t;
typedef int luxury_t;
typedef int weapon_t;
typedef int item_t;
typedef int resource_t;
typedef int spellid_t;

struct plane;
struct spell;
struct region;
struct race;
struct ship;
struct building;
struct faction;
struct party;
struct unit;
struct item;
/* item */
struct strlist;
struct resource_type;
struct item_type;
struct potion_type;
struct luxury_type;
struct herb_type;
struct weapon_type;
/* types */
struct ship_type;
struct building_type;

/* util includes */
struct xml_stack;

#include <cvector.h>
#include <umlaut.h>
#include <language.h>
#include <lists.h>
#include <vmap.h>
#include <vset.h>
#include <attrib.h>

#define AT_PERSISTENT

/* eressea-defined attribute-type flags */
#define ATF_CURSE  ATF_USER_DEFINED

#define OLD_FAMILIAR_MOD /* conversion required */
/* feature-dis/en-able */
#undef WEATHER        /* Kein Wetter-Modul */
#define NEW_DRIVE     /* Neuer Algorithmus Transportiere/Fahre */
#define PARTIAL_STUDY /* Wenn nicht genug Silber vorhanden, wird ein Talent anteilig gelernt */
#define HUNGER_REDUCES_SKILL /* Hunger reduziert den Talentwert auf die H�lfte */
#define DAEMON_HUNGER /* D�monen hungern, statt mit 10% in ihre sph�re zur�ckzukehren */
#define NEW_RECEIPIES /* Vereinfachte, besser verteilte Kr�uterzutaten f�r Tr�nke */
#define NEW_TAVERN
#define GOBLINKILL
#undef HELFE_WAHRNEHMUNG

#define USE_FIREWALL 1
#undef COMPATIBILITY

#define MONSTER_FACTION 0 /* Die Partei, in der die Monster sind. */

#define FUZZY_BASE36 /* Fuzzy- Behandlung von Geb�ude- und Schiffsnummern */
#define FULL_BASE36

#define RACES_VERSION 91
#define MIN_VERSION RACES_VERSION
#define MAGIEGEBIET_VERSION 92
#define FATTRIBS_VERSION 94
#define ATNORD_VERSION 95
#define NEWMAGIC 100
#define MEMSAVE_VERSION 101
#define BORDER_VERSION 102
#define ATNAME_VERSION 103
#define ATTRIBFIX_VERSION 104
#define BORDERID_VERSION 105
#define NEWROAD_VERSION 106
#define GUARD_VERSION 107
#define TIMEOUT_VERSION 108
#define GUARDFIX_VERSION 109
#define NEW_FACTIONID_VERSION 110
#define ACTIONFIX1_VERSION 111
#define SHIPTYPE_VERSION 112
#define GROUPS_VERSION 113
#define MSGLEVEL_VERSION 114
#define DISABLE_ROADFIX 114
#define FACTIONFLAGS_VERSION 114 /* speichern von struct faction::flags */
#define KARMA_VERSION 114
#define FULL_BASE36_VERSION 115
#define TYPES_VERSION 117
#define ITEMTYPE_VERSION 190
#define NOFOREST_VERSION 191
#define REGIONAGE_VERSION 192
#define CURSE_NO_VERSION 193
#define EFFSTEALTH_VERSION 194
#define MAGE_ATTRIB_VERSION 195
#define GLOBAL_ATTRIB_VERSION 196
#define BASE36IDS_VERSION 197
#define NEWSOURCE_VERSION 197
#define NEWSTATUS_VERSION 198
#define NEWNAMES_VERSION 199
#define LOCALE_VERSION 300
#define GROUPATTRIB_VERSION 301
#define NEWRESOURCE_VERSION 303
#define GROWTREE_VERSION 305
#define RANDOMIZED_RESOURCES_VERSION 306 /* should be the same, but doesn't work */
#define NEWRACE_VERSION 307
#define INTERIM_VERSION 309
#define NEWSKILL_VERSION 309
#define WATCHERS_VERSION 310
#define OVERRIDE_VERSION 311
#define CURSETYPE_VERSION 312
#define ALLIANCES_VERSION 313
 
#define UGROUPS_VERSION 400 /* nicht aktivieren, nicht fertig */
#define RELEASE_VERSION ALLIANCES_VERSION

/*
#if RELEASE_VERSION >= UGROUPS_VERSION
#define USE_UGROUPS
#endif
*/

#if RESOURCE_CONVERSION
extern void init_resourcefix(void);
extern void read_iron(struct region * r, int iron);
extern void read_laen(struct region * r, int laen);
#endif

#define ECHECK_VERSION "4.01"

/* changes from->to: 72->73: struct unit::lock entfernt.
 * 73->74: struct unit::flags eingef�hrt.
 * 74->75: parteitarnung als flag.
 * 75->76: #ifdef NEW_HP: hp
 * 76->77: ship->damage
 * 77->78: neue Message-Option "Orkvermehrung" (MAX_MSG +1)
 * 78->79: showdata nicht mehr speichern
 * 79->HEX_VERSION: hex
 * 80->82: ATTRIB_VERSION
 * 90: Ebenen
 * 92: Magiegebiet-Auswahl f->magiegebiet
 * 94: f->attribs wird gespeichert
 * 100: NEWMAGIC, neue Message-Option "Zauber" (MAX_MSG +1)
 * 108: Speichern von Timeouts
 * 193: curse bekommen id aus struct unit-nummernraum
 */

/* NEWMAIL benutzt ein spezielles Skript zum Erstellen der Reportmails */
#define MAIL				"eresseamail"

#define DEFAULT_LOCALE "de"

#define MAXPEASANTS_PER_AREA    10

#if !defined(NEW_ITEMS)
# define NEW_ITEMS
#endif

/* So lange kann die Partei nicht angegriffen werden */
#undef STUDY_IF_NOT_WORKING

/** Attackierende Einheiten k�nnen keine langen Befehle ausf�hren */

#define DISALLOW_ATTACK_AND_WORK

/** Ausbreitung und Vermehrung */

#define MAXDEMAND      25
#define DMRISE         10
#define DMRISEHAFEN    20

/* Vermehrungsrate Bauern in 1/10000.
 * Evt. Berechnungsfehler, reale Vermehrungsraten scheinen h�her. */
#if REDUCED_PEASANTGROWTH == 1
#define PEASANTGROWTH		10
#define BATTLE_KILLS_PEASANTS 20
#define PEASANTLUCK			10
#else
#define PEASANTGROWTH   100
#define BATTLE_KILLS_PEASANTS 100
#define PEASANTLUCK			1
#endif

/* Vermehrung trotz 90% Auslastung */
#define PEASANTFORCE   75

#define PEASANTSWANDER_WEIGHT 5
#define PEASANTSGREED_WEIGHT  5

#define STARVATION_SURVIVAL  10

/* Pferdevermehrung */
#define HORSEGROWTH 4
/* Wanderungschance pro Pferd */
#define HORSEMOVE   3

/* Vermehrungschance pro Baum */
#define FORESTGROWTH 10000					/* In Millionstel */
#define TREESIZE     (MAXPEASANTS_PER_AREA-2)

/* Eisen in Bergregionen bei Erschaffung */
#define IRONSTART     250
/* Zuwachs Eisen in Bergregionen */
#define IRONPERTURN    25
#define MAXLAENPERTURN  6
#define NEW_LAEN	0
/* Eisen in Gletschern */
#define GLIRONSTART    20
/* Zuwachs Eisen in Gletschern */
#define GLIRONPERTURN   3
/* Maximales Eisen in Gletschern */
#define MAXGLIRON      80

/* Verrottchance f�r Kr�uter */
#define HERBROTCHANCE 5
#define HERBS_ROT

#define STUDYCOST     200

/* Geb�udegr��e = Minimalbelagerer */
#define SIEGEFACTOR     2

/** Magic */
#define MAXMAGICIANS    3
#define MAXALCHEMISTS   3

/** Plagues **/
/* Seuchenwahrscheinlichkeit (siehe plagues()) */
#define SEUCHE         10
/* % Betroffene */
#define SEUCHENOPFER   20
extern void plagues(struct region * r, boolean ismagic);

/** Healing **/
/* Wahrscheinlichkeit Heilung */
#define HEILCHANCE     25
/* Heilkosten */
#define HEILKOSTEN     30

/** Monster */

/* Chance f�r Monsterangriff */
#define MONSTERATTACK  40

/** Geb�ude */
/* Chance f�r Einsturz bei unversorgtem Gebaeude */
#define EINSTURZCHANCE     40
/* Soviele % �berleben den Einsturz */
#define EINSTURZUEBERLEBEN 50

/** Geographie. Nicht im laufenden Spiel �ndern!!! */
#define BLOCKSIZE           9

/* Nur Bewaffnete bewachen */
#define WACH_WAFF
#undef TROLLSAVE

/* Spezialtrank f�r Insekten */
#define INSECT_POTION

/* Schiffsbesch�digungen */
#define SHIPDAMAGE

/* regionen im Report der Parteien werden nur einmal berechnet: */
#define FAST_REGION

/* Magiesystem */
#define NIGHT_EYE_TALENT        5

/* gibt es ein attribut at_salary */
#undef AFFECT_SALARY
/* ist noch nicht fertig! */

#define ARENA_PLANE
#define MUSEUM_PLANE
#undef UNDERWORLD_PLANE

#define ORCIFICATION

/* Bewegungsweiten: */
#define BP_WALKING 4
#define BP_RIDING  6
#define BP_UNICORN 9
#define BP_DRAGON  4

#define BP_NORMAL 3
#define BP_ROAD   2


/**
 * Hier endet der Teil von eressea.h, der die defines f�r die
 * Spielwelt Eressea enth�lt, und beginnen die allgemeinen Routinen
 */

#define ENCCHANCE           10	/* %-Chance f�r einmalige Zufallsbegegnung */
#define SPACE_REPLACEMENT   '~'
#define ESCAPE_CHAR         '\\'

#define DISPLAYSIZE         8191	/* max. L�nge einer Beschreibung, ohne trailing 0 */
#define NAMESIZE            127	/* max. L�nge eines Namens, ohne trailing 0 */
#define IDSIZE              15	/* max. L�nge einer no (als String), ohne trailing 0 */
#define KEYWORDSIZE         15	/* max. L�nge eines Keyword, ohne trailing 0 */
#define OBJECTIDSIZE        (NAMESIZE+5+IDSIZE)	/* max. L�nge der Strings, die
						 * von struct unitname, regionid, etc. zur�ckgegeben werden. ohne die 0 */
#define CMDSIZE             (DISPLAYSIZE*2+1)
#define STARTMONEY          5000

#define PRODUCEEXP          10
#define MAINTENANCE         10
#define TAVERN_MAINTENANCE  14
/* Man gibt in einer Taverne mehr Geld aus! */

#define BAGCAPACITY		20000	/* soviel pa�t in einen Bag of Holding */
#define STRENGTHCAPACITY	50000	/* zus�tzliche Tragkraft beim Kraftzauber */

typedef struct ursprung {
	struct ursprung *next;
	int id;
	int x, y;
} ursprung;

/* ----------------- Befehle ----------------------------------- */

typedef int keyword_t;
enum {
	K_KOMMENTAR,
	K_BANNER,
	K_WORK,
	K_ATTACK,
	K_BIETE,
	K_STEAL,
	K_BESIEGE,
	K_NAME,
	K_USE,
	K_DISPLAY,
	K_ENTER,
	K_GUARD,
	K_MAIL,
	K_END,
	K_DRIVE,
	K_NUMBER,
	K_FOLLOW,
	K_RESEARCH,
	K_GIVE,
	K_ALLY,
	K_STATUS,
	K_COMBAT,
	K_BUY,
	K_CONTACT,
	K_TEACH,
	K_STUDY,
	K_LIEFERE,
	K_MAKE,
	K_MOVE,
	K_PASSWORD,
	K_DUMMY,
	K_RECRUIT,
	K_RESERVE,
	K_ROUTE,
	K_SABOTAGE,
	K_SEND,
	K_SPY,
	K_QUIT,
	K_SETSTEALTH,
	K_TRANSPORT,
	K_TAX,
	K_ENTERTAIN,
	K_SELL,
	K_LEAVE,
	K_FORGET,
	K_CAST,
	K_RESHOW,
	K_DESTROY,
	K_ZUECHTE,
	K_DEFAULT,
	K_REPORT,
	K_URSPRUNG,
	K_EMAIL,
	K_VOTE,
	K_MAGIEGEBIET,
	K_PIRACY,
	K_RESTART,
	K_GROUP,
	K_SACRIFICE,
	K_PRAY,
	K_SORT,
	K_SETJIHAD,
	K_GM,          /* perform GM commands */
	K_INFO,        /* set player-info */
#ifdef USE_UGROUPS
	K_JOINUGROUP,
	K_LEAVEUGROUP,
#endif
	K_PREFIX,
	K_SYNONYM,
	K_PFLANZE,
	K_WEREWOLF,
	K_XE,
	K_ALLIANCE,
	MAXKEYWORDS,
	NOKEYWORD = (keyword_t) - 1
};

extern const char *keywords[MAXKEYWORDS];

/* ------------------ Status von Einheiten --------------------- */

typedef int status_t;
enum {
	ST_AGGRO,
	ST_FIGHT,
	ST_BEHIND,
	ST_CHICKEN,
	ST_AVOID,
	ST_FLEE
};

/* ----------------- Parameter --------------------------------- */

typedef int param_t;
enum {
	P_LOCALE,
	P_ANY,
	P_PEASANT,
	P_BUILDING,
	P_UNIT,
	P_PRIVAT,
	P_BEHIND,
	P_CONTROL,
	P_HERBS,
	P_NOT,
	P_NEXT,
	P_FACTION,
	P_GAMENAME,
	P_PERSON,
	P_REGION,
	P_SHIP,
	P_SILVER,
	P_ROAD,
	P_TEMP,
	P_FLEE,
	P_GEBAEUDE,
	P_GIB,
	P_KAEMPFE,
	P_GUARD,
	P_ZAUBER,
	P_PAUSE,
	P_VORNE,
	P_AGGRO,
	P_CHICKEN,
	P_LEVEL,
	P_HELP,
	P_FOREIGN,
	P_AURA,
	P_FOR,
	P_AID,
	P_MERCY,
	P_AFTER,
	P_BEFORE,
	P_NUMBER,
	P_ITEMS,
	P_POTIONS,
	P_GROUP,
	P_FACTIONSTEALTH,
	P_TREES,
	P_XEPOTION,
	P_XEBALLOON,
	P_XELAEN,
	MAXPARAMS,
	NOPARAM = (param_t) - 1
};

typedef enum {					/* Fehler und Meldungen im Report */
	MSG_BATTLE,
	MSG_EVENT,
	MSG_MOVE,
	MSG_INCOME,
	MSG_COMMERCE,
	MSG_PRODUCE,
	MSG_ORCVERMEHRUNG,
	MSG_MESSAGE,
	MSG_COMMENT,
	MSG_MAGIC,
	MAX_MSG
} msg_t;

enum {					/* Message-Level */
	ML_IMPORTANT,				/* Sachen, die IMO erscheinen _muessen_ */
	ML_DEBUG,
	ML_MISTAKE,
	ML_WARN,
	ML_INFO,
	ML_MAX
};

extern const char *gr_prefix[3];
extern const char *parameters[MAXPARAMS];

/* --------------- Reports Typen ------------------------------- */

extern const char *report_options[MAX_MSG];

extern const char *message_levels[ML_MAX];

enum {
	O_REPORT,				/* 1 */
	O_COMPUTER,			/* 2 */
	O_ZUGVORLAGE,		/* 4 */
	O_SILBERPOOL,		/* 8 */
	O_STATISTICS,		/* 16 */
	O_DEBUG,				/* 32 */
	O_COMPRESS,			/* 64 */
	O_NEWS,					/* 128 */
	O_MATERIALPOOL,	/* 256 */
	O_ADRESSEN,			/* 512 */
	O_BZIP2,				/* 1024 - crkurz compatible flag */
	O_SCORE,				/* 2048 - punkte anzeigen? */
	O_SHOWSKCHANGE,	/* 4096 - Skillver�nderungen anzeigen? */
	MAXOPTIONS
};

#define want(i) (1<<i)
#define Pow(i) (1<<i)

extern const char *options[MAXOPTIONS];

/* ------------------ Talente ---------------------------------- */

enum {
	SK_ALCHEMY,
	SK_CROSSBOW,
	SK_MINING,
	SK_LONGBOW,
	SK_BUILDING,
	SK_TRADE,
	SK_LUMBERJACK,
	SK_CATAPULT,
	SK_HERBALISM,
	SK_MAGIC,
	SK_HORSE_TRAINING,			/* 10 */
	SK_RIDING,
	SK_ARMORER,
	SK_SHIPBUILDING,
	SK_SWORD,
	SK_SAILING,
	SK_SPEAR,
	SK_SPY,
	SK_QUARRYING,
	SK_ROAD_BUILDING,
	SK_TACTICS,					/* 20 */
	SK_STEALTH,
	SK_ENTERTAINMENT,
	SK_WEAPONSMITH,
	SK_CARTMAKER,
	SK_OBSERVATION,
	SK_TAXING,
	SK_AUSDAUER,
	SK_WEAPONLESS,
	MAXSKILLS,
	NOSKILL = (skill_t) -1
};

/* ------------- Typ von Einheiten ----------------------------- */

enum {
	RC_DWARF,		/* 0 - Zwerg */
	RC_ELF,
	RC_ORC,
	RC_GOBLIN,
	RC_HUMAN,
	RC_TROLL,
	RC_DAEMON,
	RC_INSECT,
	RC_HALFLING,
	RC_CAT,
	RC_AQUARIAN,
	RC_URUK,
	RC_SNOTLING,

	RC_UNDEAD,		/* 12 - Untoter */
	RC_ILLUSION,
	RC_FIREDRAGON,
	RC_DRAGON,
	RC_WYRM,
	RC_TREEMAN,

	RC_BIRTHDAYDRAGON,	/* 18 - Katzendrache*/
	RC_DRACOID,

	RC_SPECIAL,
	RC_SPELL,

	RC_IRONGOLEM,		/* 22 - Eisengolem */
	RC_STONEGOLEM,

	RC_SHADOW,
	RC_SHADOWLORD,

	RC_IRONKEEPER,	/* 26 - Bergw�chter */
	RC_ALP,

	RC_TOAD,		/* 28 - Kr�te */
	RC_HIRNTOETER,

	RC_PEASANT,
	RC_WOLF, /* 31 */

	RC_HOUSECAT,
	RC_TUNNELWORM,
	RC_EAGLE,
	RC_RAT,
	RC_PSEUDODRAGON,
	RC_NYMPH,
	RC_UNICORN,
	RC_WARG,
	RC_WRAITH,
	RC_IMP, /* 41 */
	RC_DREAMCAT,
	RC_FEY,
	RC_OWL,
	RC_HELLCAT,
	RC_TIGER,
	RC_DOLPHIN,
	RC_OCEANTURTLE,
	RC_KRAKEN,
	RC_SEASERPENT,
	RC_SHADOWKNIGHT, /* 51 */

	RC_CENTAUR,

	RC_SKELETON,
	RC_SKELETON_LORD,
	RC_ZOMBIE,
	RC_ZOMBIE_LORD,
	RC_GHOUL,
	RC_GHOUL_LORD,

	RC_MUS_SPIRIT,   /* 60 */
	RC_GNOME,        /* 61 */
	RC_TEMPLATE,     /* 62 */
	RC_CLONE,        /* 63 */
	
	RC_SHADOWDRAGON, /* 64 */
	RC_SHADOWBAT,
	RC_NIGHTMARE,
	RC_VAMPUNICORN,

	MAXRACES,
	NORACE = (race_t) - 1
};


/* ---------- Regionen ----------------------------------------- */

/* Richtungen */
enum {
	D_NORTHWEST,
	D_NORTHEAST,
	D_EAST,
	D_SOUTHEAST,
	D_SOUTHWEST,
	D_WEST,
	MAXDIRECTIONS,
	D_PAUSE,
	NODIRECTION = (direction_t) - 1
};

/* Jahreszeiten, siehe auch res/timestrings */
enum {
	SEASON_WINTER,
	SEASON_SPRING,
	SEASON_SUMMER,
	SEASON_AUTUMN
};

/* Siegbedingungen */

#define VICTORY_NONE   0
#define VICTORY_MURDER 1

/* ------------------------------------------------------------- */

#define MAXSPEED   21

int shipcapacity(const struct ship * sh);
int shipspeed(struct ship * sh, const struct unit * u);

/* MAXSPEED setzt die groesse fuer den Array, der die Kuesten Beschreibungen
 * enthaelt. MAXSPEED muss 2x +3 so gross wie die groesste Geschw. sein
 * (Zauberspruch) */

/* ------------------------------------------------------------- */

/* ------------------------------------------------------------- */
/* TODO
 * werden diese hier eigendlich noch f�r irgendwas gebraucht?*/


/* Attribute: auf Einheiten */

/* Klassen */

enum {
	A_KL_GEGENSTAND,
	A_KL_EIGENSCHAFT
};
/* Eigenschaften (in i[0]) */

#define ZEIGE_PARTEI	1		/* Zeigt s[0] in eigenem Report an */
#define ZEIGE_ANDEREN 2			/* Zeigt s[1] in fremden Reports an */
#define TALENTMOD			4	/* Gegenstand ver�ndert Talent
						 * i[2] um i[3] */

/* A_KL_EIGENSCHAFT */

enum {
	A_TY_NOT_PARTEI_GETARNT,	/* In i[0] die Pseudopartei?! */
	A_TY_NOT_FLAGS,		/* Verschiedene Eigenschaften in i[0] */
	A_TY_EX_TYPNAME,		/* Was soll als Typname erscheinen? s[0] =
				 * Singular, s[1] = Plural */
	A_TY_EX_ZIELREGION,
	A_TY_EX_BAUERNBLUT		/* In i[0] die Zahl der max. gesch�tzten
				 * D�monen. */
};

/* A_TY_GESEHEN:    fuer cansee() in i[0] steht drin, welche Partei uns
 * sah, in i[1], wie gut (0,1 oder 2) */

/* Auf Regionen */

/* Klassen */

enum {
	A_KL_MERKMAL,
	A_KL_ZAUBER
};

/* A_KL_MERKMAL */

enum {
	A_TY_MITHRIL,
	A_TY_ADAMANTIUM,
	A_TY_DIRECTION		/* In s[0] describe_txt, in s[1] das command,
											 in i[0]-i[2] die Koordinaten des Ziels. */
};

/* Auf Schiffen */

/* Klassen */

enum {
	A_KL_MERKMAL_SCHIFF
};

/* A_KL_MERKMAL */

enum {
	A_TY_EX_KUESTE		/* In i[0] steht die Richtung */
};

/* Auf Borders */

/* Klassen */

enum {
	A_KL_BLOCK
};

enum {
	A_TY_LOCK
};

#define DONT_HELP      0
#define HELP_MONEY     1			/* Mitversorgen von Einheiten */
#define HELP_FIGHT     2			/* Bei Verteidigung mithelfen */
#define HELP_OBSERVE   4			/* Bei Wahrnehmung mithelfen */
#define HELP_GIVE      8			/* Dinge annehmen ohne KONTAKTIERE */
#define HELP_GUARD    16			/* Laesst Steuern eintreiben etc. */
#define HELP_FSTEALTH 32			/* Laesst Steuern eintreiben etc. */
#define HELP_ALL    63-HELP_OBSERVE		/* Alle "positiven" HELPs zusammen */
/* HELP_OBSERVE deaktiviert */
/* ------------------------------------------------------------- */
/* Prototypen */


#define ALLIED_TAX     1
#define ALLIED_NOBLOCK 2
#define ALLIED_HELP    4

extern vmap region_map;

#define i2b(i) ((boolean)((i)?(true):(false)))

typedef struct ally {
	struct ally *next;
	struct faction *faction;
	int status;
} ally;

#define FF_NEWID (1<<0) /* Die Partei hat bereits einmal ihre no gewechselt */

void remove_empty_units_in_region(struct region *r);
void remove_empty_units(void);
void remove_empty_factions(boolean writedropouts);

typedef struct strlist {
	struct strlist *next;
	char * s;
} strlist;

extern int max_unique_id;

#define UFL_DEBUG         (1<<0)
#define UFL_ISNEW         (1<<1)	/* 2 */
#define FL_LONGACTION     (1<<2)	/* 4 */
#define FL_OWNER          (1<<3)	/* 8 */
#define FL_PARTEITARNUNG  (1<<4)	/* 16 */
#define FL_DISBELIEVES    (1<<5)	/* 32 */
#define FL_WARMTH         (1<<6)	/* 64 */
#define FL_MOVED          (1<<8)

#define FL_FOLLOWING      (1<<9)
#define FL_FOLLOWED       (1<<10)

#define FL_HUNGER         (1<<11) /* kann im Folgemonat keinen langen Befehl
                                     au�er ARBEITE ausf�hren */

#define FL_SIEGE          (1<<12) /* speedup: belagert eine burg, siehe attribut */
#define FL_TARGET         (1<<13) /* speedup: hat ein target, siehe attribut */

#define FL_POTIONS        (1<<15) /* speedup: hat ein oder mehr tr�nke, statt potionptr */

/* warning: von 512/1024 gewechslet, wegen konflikt mit NEW_FOLLOW */
#define FL_LOCKED         (1<<16) /* Einheit kann keine Personen aufnehmen oder weggeben, nicht rekrutieren. */

#define FL_DH             (1<<18) /* ehemals f->dh, u->dh, r->dh, etc... */
#define FL_STORM          (1<<19) /* Kapit�n war in einem Sturm */
#define FL_TRADER         (1<<20) /* H�ndler, pseudolang */

/* alle vierstelligen zahlen: */
#define MAX_UNIT_NR (36*36*36*36-1)
#define MAX_CONTAINER_NR (36*36*36*36-1)

#define FL_NOAIDF         (1<<21) /* Hilfsflag Kampf */
#define FL_NOAID          (1<<22) /* Einheit hat Noaid-Status */

#define FL_MARK           (1<<23) /* f�r markierende algorithmen, die das hinterher auch wieder
																		 l�schen m�ssen! (Ist daf�r nicht eigentlich FL_DH gedacht?) */
#define FL_NOIDLEOUT      (1<<24) /* Partei stirbt nicht an NMRs */
#define FL_TAKEALL        (1<<25) /* Einheit nimmt alle Gegenst�nde an */
#define FL_UNNAMED        (1<<26) /* Partei/Einheit/Geb�ude/Schiff ist unbenannt */

#define FFL_RESTART       (1<<2)
#define FFL_OVERRIDE      (1<<27) /* Override-Passwort wurde benutzt */

#define UFL_WERE					(1<<28)

/* Flags, die gespeichert werden sollen: */
#define UFL_SAVEMASK (FL_NOAID | FL_OWNER | FL_PARTEITARNUNG | FL_LOCKED | FL_HUNGER | FL_NOIDLEOUT | FL_TAKEALL | FL_UNNAMED)

#define fval(u, i) ((u)->flags & (i))
#define fset(u, i) ((u)->flags |= (i))
#define freset(u, i) ((u)->flags &= ~(i))

typedef struct request {
	struct request * next;
	struct unit *unit;
	int qty;
	int no;
	union {
		boolean goblin; /* stealing */
		const struct luxury_type * ltype; /* trading */
	} type;
} request;

int turn;

/* parteinummern */
extern int *used_faction_ids;
extern int no_used_faction_ids;
extern void register_faction_id(int id);
extern void init_used_faction_ids(void);
extern boolean faction_id_is_unused(int);

/* leuchtturm */
extern boolean check_leuchtturm(struct region * r, struct faction * f);
extern void update_lighthouse(struct building * lh);

/* skills */
extern int max_skill(const struct faction * f, skill_t sk);
extern int count_skill(struct faction * f, skill_t sk);
extern int count_all_money(const struct region * r);

/* direction, geography */
extern const char *directions[];
extern direction_t finddirection(const char *s, const struct locale *);

extern int findoption(const char *s, const struct locale * lang);

/* shared character-buffer */
#define BUFSIZE 32765
extern char buf[BUFSIZE + 1];

/* special units */
struct unit *make_undead_unit(struct region * r, struct faction * f, int n, const struct race * race);

extern struct region *regions;
extern struct faction *factions;

strlist *makestrlist(const char *s);
void addstrlist(strlist ** SP, const char *s);

int armedmen(const struct unit * u);

void scat(const char *s);
void icat(int n);

int atoip(const char *s);
int geti(void);

extern int findstr(const char **v, const char *s, unsigned char n);

extern char *igetstrtoken(const char *s);
extern char *getstrtoken(void);

extern skill_t findskill(const char *s, const struct locale * lang);

extern keyword_t findkeyword(const char *s, const struct locale * lang);
extern keyword_t igetkeyword(const char *s, const struct locale * lang);
extern keyword_t getkeyword(const struct locale * lang);

extern param_t findparam(const char *s, const struct locale * lang);
extern param_t igetparam(const char *s, const struct locale * lang);
extern param_t getparam(const struct locale * lang);

#define BASE36_VERSION 93
extern int atoi36(const char * s);
#define getid() ((global.data_version<BASE36_VERSION)?geti():atoi36(getstrtoken()))
#define getstruct unitid() getid()
#define unitid(x) itoa36((x)->no)

#define getshipid() getid()
#define getfactionid() getid()

#define buildingid(x) itoa36((x)->no)
#define shipid(x) itoa36((x)->no)
#define factionid(x) itoa36((x)->no)
#define curseid(x) itoa36((x)->no)

extern boolean cansee(const struct faction * f, const struct region * r, const struct unit * u, int modifier);
boolean cansee_durchgezogen(const struct faction * f, const struct region * r, const struct unit * u, int modifier);
boolean seefaction(const struct faction * f, const struct region * r, const struct unit * u, int modifier);
extern int effskill(const struct unit * u, skill_t sk);

int lovar(int n);

int distribute(int old, int new_value, int n);

int newunitid(void);
int forbiddenid(int id);
int newcontainerid(void);

extern struct unit *createunit(struct region * r, struct faction * f, int number, const struct race * rc);
extern struct unit *create_unit(struct region * r1, struct faction * f, int number, const struct race * rc, int id, const char * dname, struct unit *creator);
extern void create_unitid(struct unit *u, int id);
extern boolean getunitpeasants;
struct unit *getunitg(const struct region * r, const struct faction * f);
struct unit *getunit(const struct region * r, const struct faction * f);
int read_unitid(const struct faction * f, const struct region * r);

int isallied(const struct plane * pl, const struct faction * f, const struct faction * f2, int mode);
int allied(const struct unit * u, const struct faction * f, int mode);

struct faction *findfaction(int n);
struct faction *findfaction_unique_id(int unique_id);
struct faction *getfaction(void);

struct region *findregion(int x, int y);
struct unit *findunitg(int n, const struct region * hint);
struct unit *findunit(int n);

struct unit *findunitr(const struct region * r, int n);
struct region *findunitregion(const struct unit * su);

char *estring(const char *s);
char *cstring(const char *s);
const char *regionid(const struct region * r);
const char *unitname(const struct unit * u);
char *xunitid(const struct unit * u);

struct building *largestbuilding(const struct region * r, boolean img);

extern boolean teure_talente(const struct unit * u);
int count_all(const struct faction * f);
int count_maxmigrants(const struct faction * f);
const struct race * findrace(const char *, const struct locale *);

int effstealth(const struct unit * u);
int eff_stealth(const struct unit * u, const struct region * r);
void scale_number(struct unit * u, int n);
int unit_max_hp(const struct unit * u);
int ispresent(const struct faction * f, const struct region * r);

char * set_string(char **s, const char *neu);

/* unterschied zu getkeyword(void) ist lediglich, da� in *str dynamisch der
 * neue string hineingeschrieben wird (speicher wird reserviert, falls *str zu
 * klein). enno. */

int check_option(struct faction * f, int option);

/* Anzahl Personen in einer Einheit festlegen. NUR (!) mit dieser Routine,
 * sonst gro�es Ungl�ck. Durch asserts an ein paar Stellen abgesichert. */
void verify_data(void);

void stripfaction(struct faction * f);
void stripunit(struct unit * u);
void freestrlist(strlist * s);

int change_hitpoints(struct unit *u, int value);

int weight(const struct unit * u);
void changeblockchaos(void);

struct region *firstregion(struct faction * f);
struct region *lastregion(struct faction * f);

void inituhash(void);
void uhash(struct unit * u);
void uunhash(struct unit * u);
struct unit *ufindhash(int i);

void fhash(struct faction * f);
void funhash(struct faction * f);

#ifndef NDEBUG
const char *strcheck(const char *s, size_t maxlen);

#else
#define strcheck(s, ml) (s)
#endif

const char * findorder(const struct unit * u, const char * cmd);

#define attacked(u) (fval(u, FL_LONGACTION))
boolean idle(struct faction * f);
boolean unit_has_cursed_item(struct unit *u);
struct region * rconnect(const struct region *, direction_t dir);

/* simple garbage collection: */
void * gc_add(void * p);
void gc_done(void);
void mistake(const struct unit * u, const char *cmd, const char *text, int mtype);
void addmessage(struct region * r, struct faction * f, const char *s, msg_t mtype, int level);
void cmistake(const struct unit * u, const char *cmd, int mno, int mtype);

/* grammatik-flags: */
#define GF_NONE 0
	/* singular, ohne was dran */
#define GF_PLURAL 1
	/* Angaben in Mehrzahl */
#define GF_ARTICLE 8
	/* der, die, eine */
#define GF_SPECIFIC 16
	/* der, die, das vs. ein, eine */
#define GF_DETAILED 32
	/* mehr Informationen. z.b. stra�e zu 50% */


extern struct attrib_type at_guard;
extern struct attrib_type at_lighthouse;
extern struct attrib_type at_creator;

#define GUARD_NONE 0
#define GUARD_TAX 1
	/* Verhindert Steuereintreiben */
#define GUARD_MINING 2
	/* Verhindert Bergbau */
#define GUARD_TREES 4
	/* Verhindert Waldarbeiten */
#define GUARD_TRAVELTHRU 8
	/* Blockiert Durchreisende */
#define GUARD_LANDING 16
	/* Verhindert Ausstieg + Weiterreise */
#define GUARD_CREWS 32
	/* Verhindert Unterhaltung auf Schiffen */
#define GUARD_RECRUIT 64
  /* Verhindert Rekrutieren */
#define GUARD_PRODUCE 128
	/* Verhindert Abbau von Resourcen mit RTF_LIMITED */
#define GUARD_ALL 0xFFFF

extern void setguard(struct unit * u, unsigned int flags);
	/* setzt die guard-flags der Einheit */
extern unsigned int getguard(const struct unit * u);
	/* liest die guard-flags der Einheit */
extern void guard(struct unit * u, unsigned int mask);
	/* Einheit setzt "BEWACHE", rassenspezifzisch.
	 * 'mask' kann einzelne flags zus�tzlich und-maskieren.
	 */

typedef struct local_names {
	struct local_names * next;
	const struct locale * lang;
	struct tnode names;
} local_names;

extern int maxworkingpeasants(const struct region * r);
extern void hunger(struct unit * u, int moneyneeded);
extern int lifestyle(const struct unit*);
extern int besieged(const struct unit * u);

extern int wage(const struct region *r, const struct unit *u, boolean img);
extern int fwage(const struct region *r, const struct faction *f, boolean img);
extern struct region * movewhere(struct region * r, const struct unit *u);
extern boolean move_blocked(const struct unit * u, const struct region *r, direction_t dir);
extern void add_income(struct unit * u, int type, int want, int qty);

extern int month(int offset);
extern const char * basepath(void);
extern const char * resourcepath(void);
extern void kernel_init(void);
extern void kernel_done(void);

extern void reorder_owners(struct region * r);

extern const char *locales[];

#define FIRST_TURN 184

#include <log.h>

#ifdef _MSC_VER
#include <stdafx.h>
#endif
#include "terrain.h" /* f�r (bald) alte MAXTERRAINS */

/** compatibility: **/
extern race_t old_race(const struct race *);
extern const struct race * new_race[];

/* globale settings des Spieles */
typedef struct settings {
	const char    *gamename;
	const char    *welcomepath;
	unsigned int   maxunits;
	struct attrib *attribs;
	unsigned int   data_version;
	boolean disabled[MAXKEYWORDS];
} settings;
extern settings global;

extern FILE * updatelog;
extern int produceexp(struct unit * u, skill_t sk, int n);

#endif
