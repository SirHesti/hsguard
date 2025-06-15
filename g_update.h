/// @file g_update.h

int     Execute_update(void);
char   *update_getDir_or_basis_orDie(void);
sqlid_t update_getdirnum(char *dirname, int flags);
int     update_new(char *dirname, int deep);
char  **update_delete(char **filelst, sqlid_t loc_id);
int     update_insert_makeCRC32 (char *local_name, sqlid_t loc_id, char *fname, size_t fsize, time_t fmtime);
int     update_file (char *local_name, struct entrie_s *dbent, size_t fsize, time_t fmtime);
int     update_excllst_find(char *findme);
void   *update_Placelst_free(void *data);
int     update_dirsDB(struct place_s *place, int deep);
void    update_mover(int shownumber);

enum {
    F_CREATE_DIR    = (1<< 1),
    F_CREATE_CACHED = (1<< 2),
    F_CREATE_IGNORE = (1<<15)
};

