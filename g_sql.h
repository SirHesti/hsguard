/// @file h_sql.h

#ifdef  __cplusplus
extern "C" {
#endif

typedef sqlite3_int64 sqlid_t;

struct entrie_s{
    sqlid_t ID;
    sqlid_t loc_id;
    char    fname[NAME_MAX+1];
    size_t  fsize;
    time_t  fmtime;
    crc32_t fcrc32;
    time_t  lastchk;
    long    checkcnt;
    time_t  founded;
};

struct entrie_small_s{
	sqlid_t ID;
	char    fname[NAME_MAX+1];
};

struct place_s{
    sqlid_t ID;
    sqlid_t parent_id;
    char    *location;
    time_t  founded;
};

char *open_valid_sqlite3(void);
int sql_create_database (void);
//int sql_getCount(int From, char *extraInfo);

int getnumber(char **in_out);
time_t sql_parse_date(char *datestring);

#define sql_ShowError(rc,sql_error,sql_query) sql_error=sql_ShowError_w( rc, sql_error, sql_query, __FILE__, __LINE__ );
char *sql_ShowError_w (int rc, char *error_exec, char *sql, char *file, int line );
int     sql_entrie_list   (sqlid_t loc_id);
sqlid_t sql_entrie_insert (sqlid_t loc_id, char *fname, size_t fsize, time_t fmtime, crc32_t crc32);
int     sql_entrie_update (sqlid_t id,                  size_t fsize, time_t fmtime, crc32_t st_crc32);
int     sql_entrie_delete (sqlid_t id);
int     sql_entrie_count  (sqlid_t loc_id);
int     sql_entrie_getFullList(sqlid_t loc_id);
sqlid_t sql_entrie_getID   (sqlid_t loc_id, char *fname, struct entrie_s *ent);
sqlid_t sql_entrie_getByID (sqlid_t id, struct entrie_s *ent);
int     sql_entrie_listByCRC(crc32_t crc);
int     sql_entrie_listLikeFname(char *fname);
int     sql_entrie_getFullbyCrcSizeNotID(sqlid_t id, size_t fsize, crc32_t fcrc32);

sqlid_t sql_location_getID (char *location);
sqlid_t sql_location_insert(char *location, sqlid_t pid);
char   *sql_location_getDirname(sqlid_t loc_id);
int     sql_location_delete(sqlid_t id);
int     sql_location_getDir(char *lname, struct place_s *place);
//int     sql_location_getSubdirs(char *lname, void **data);
int     sql_location_getSubdirs(sqlid_t id, void **data);
int     sql_location_getByID(sqlid_t id, struct place_s *place);

//int sql_callback_null       (void *N, int number_of_lines, char **column_value, char **column_name);
int sql_cb_GetSingleData    (void *N, int number_of_lines, char **column_value, char **column_name);
int sql_cb_GetEntrie        (void *N, int number_of_lines, char **column_value, char **column_name);
int sql_cb_BuildEntryLst    (void *N, int number_of_lines, char **column_value, char **column_name);
int sql_cb_GetLocation      (void *N, int number_of_lines, char **column_value, char **column_name);
int sql_cb_GetEntrieNodes   (void *N, int number_of_lines, char **column_value, char **column_name);

/* ** neu ** */
int sql_testdbGetAll(char *location, int maxids, int rdm);
int sql_cb_GetTestDBID(void *N, int number_of_lines, char **column_value, char **column_name);

int sql_location_index_drop(void);
int sql_location_index_create(void);



#ifdef  __cplusplus
}
#endif

