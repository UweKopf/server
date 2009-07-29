#include <config.h>
#include <kernel/eressea.h>
#include <kernel/faction.h>
#include <util/unicode.h>
#include <sqlite3.h>
#include <md5.h>

faction * get_faction_by_id(int uid)
{
  faction * f;
  for (f=factions;f;f=f->next) {
    if (f->subscription==uid) {
      return f;
    }
  }
  return NULL;
}

int
db_update_factions(sqlite3 * db)
{
  int game_id = 6;
//  const char * sql = "INSERT OR REPLACE INTO email (md5, email) VALUES(?, ?)";
  const char * sql_select = 
      "SELECT faction.id, faction.email_id, faction.no, email.email FROM email, faction"
      " WHERE email.id=faction.email_id AND faction.game_id=?";
  const char * sql_insert_email = 
      "INSERT OR IGNORE email (email,md5) VALUES (?,?)";
  const char * sql_select_email =
      "SELECT id FROM email WHERE md5=?";
  const char * sql_update_faction = 
      "UPDATE faction SET email_id=?, lastturn=?, no=? WHERE id=?";

  faction * f;
  sqlite3_stmt *stmt_insert_email;
  sqlite3_stmt *stmt_select_email;
  sqlite3_stmt *stmt_update_faction;
  sqlite3_stmt *stmt_select;
  int res;
  
  res = sqlite3_prepare_v2(db, sql_select_email, strlen(sql_select_email)+1, &stmt_select_email, NULL);
  if (res!=SQLITE_OK) return res;
  res = sqlite3_prepare_v2(db, sql_insert_email, strlen(sql_insert_email)+1, &stmt_insert_email, NULL);
  if (res!=SQLITE_OK) return res;
  res = sqlite3_prepare_v2(db, sql_select, strlen(sql_select)+1, &stmt_select, NULL);
  if (res!=SQLITE_OK) return res;
  res = sqlite3_prepare_v2(db, sql_update_faction, strlen(sql_update_faction)+1, &stmt_update_faction, NULL);
  if (res!=SQLITE_OK) return res;

  res = sqlite3_bind_int(stmt_select, 1, game_id);
  for (;;) {
    sqlite3_uint64 idfaction, idemail;
    char email[64];
    int length, no;
    
    res = sqlite3_step(stmt_select);
    if (res!=SQLITE_ROW) break;
    idfaction = sqlite3_column_int64(stmt_select, 0);
    idemail = sqlite3_column_int64(stmt_select, 1);
    no = sqlite3_column_int(stmt_select, 2);
    length = sqlite3_column_bytes(stmt_select, 3);
    assert(length<sizeof(email));
    memcpy(email, sqlite3_column_text(stmt_select, 3), length);
    f = get_faction_by_id((int)idfaction);
    if (f==NULL) {
      // update status?
    } else {
      if (strcmp(f->email, email)!=0) {
        char lower[64];
        unicode_utf8_tolower(lower, sizeof(lower), f->email);
        if (strcmp(email, lower)!=0) {
          char md5hash[33];
          md5_state_t ms;
          md5_byte_t digest[16];
          int i;

          md5_init(&ms);
          md5_append(&ms, (md5_byte_t*)lower, strlen(lower));
          md5_finish(&ms, digest);
          for (i=0;i!=16;++i) sprintf(md5hash+2*i, "%.02x", digest[i]);

          idemail = 0;
          res = sqlite3_bind_text(stmt_insert_email, 1, lower, -1, SQLITE_TRANSIENT);
          res = sqlite3_bind_text(stmt_insert_email, 2, md5hash, -1, SQLITE_TRANSIENT);
          res = sqlite3_step(stmt_insert_email);
          if (res==SQLITE_OK) {
            idemail = sqlite3_last_insert_rowid(db);
          } else {
            res = sqlite3_bind_text(stmt_select_email, 1, md5hash, -1, SQLITE_TRANSIENT);
            res = sqlite3_step(stmt_select_email);
            if (res==SQLITE_ROW) {
              idemail = sqlite3_column_int64(stmt_select_email, 0);
            }
          }
          // INSERT OR IGNORE email
          // if (OK) idemail = lat_rowid
          // else SELECT email.id
          // UPDATE faction SET email_id=?, lastturn=?, no=?
        }
      }
      res = sqlite3_bind_int64(stmt_update_faction, 1, idemail);
      res = sqlite3_bind_int(stmt_update_faction, 2, turn);
      res = sqlite3_bind_int(stmt_update_faction, 3, f->no);
      res = sqlite3_bind_int64(stmt_update_faction, 4, idfaction);
      res = sqlite3_step(stmt_update_faction);
    }
  }
  sqlite3_finalize(stmt_select);
/*  
  for (f=factions;f;f=f->next) {
    char email[64];
    char hash[33];
    md5_state_t ms;
    md5_byte_t digest[16];
    int i;
    sqlite3_int64 rowid;
    
    unicode_utf8_tolower(email, sizeof(email), f->email);
    
    md5_init(&ms);
    md5_append(&ms, (md5_byte_t*)email, strlen(email));
    md5_finish(&ms, digest);
    for (i=0;i!=16;++i) sprintf(hash+2*i, "%.02x", digest[i]);
    
    res = sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_TRANSIENT);
    res = sqlite3_bind_text(stmt, 2, email, -1, SQLITE_TRANSIENT);
    do {
      res = sqlite3_step(stmt);
    } while (res!=SQLITE_DONE);
    rowid = sqlite3_last_insert_rowid(db);
    // TODO: use rowid to update faction    
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);
 */
  return SQLITE_OK;
}
