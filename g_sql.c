#include "hsguard.h"

/**
 @brief start the the recode
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param startpoint

 SQLite3 Fehler ausgeben und Speicher (sql_ErrMsg) freigeben
 falls nicht NULL sql ausgeben

___[ Revision ]______________________________________________________________

 ** 02.03.18 HS Create als ShowSQLError für dir2file
 ** 27.05.18 HS verschoben in global.c
 ** 25.04.25 HS wieder aktiviert h_sql hsguard
 ** 26.04.25 HS file+line
___________________________________________________________________________*/

char *sql_ShowError_w ( int rc, char *error_exec, char *sql, char *file, int line )
{
    char *sql_msg;
    sql_msg=strprintf("%s (%i) %s", "Db Error", rc, error_exec);
    if (sql)
    {
        stradd(sql_msg," -> ");
        stradd(sql_msg,sql);
    }
    stradd(sql_msg," [" );
    stradd(sql_msg, file );
    stradd(sql_msg,"] " );
    stradd(sql_msg, strnum (line) );
    lmsg (mFAIL, sql_msg);
    free(sql_msg);
    sqlite3_free(error_exec);
    return NULL;
}

/**
 @brief Nächste Zahl holen
 @return zahl, die im string veräuft
 @param in_out pointer der start und ende markiert

 @code
0123456789012345678
2025-12-13 15:12:44
^    ^
 @endcode

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Create
___________________________________________________________________________*/

int getnumber(char **in_out)
{
    int li;								// ergebnis
    char *in;							// modifizierbarer pointer
    in = *in_out;						// zuweisen aus parametern

    li=0;
    if (!in) return li;					// wenn NULL, dann ist auch der ergebnis NULL
    for(;in;in++)						// solange "in" durlaufen
    {
        if (!isdigit(*in)) break;		// bis es keine Zahl mehr ist
        li=(li*10)+(*in-'0');			// wenn doch noch zahl dann ergnis fortschreiben
    }
    for(;in;in++)						// jetzt suchen bis
    {
        if (*in=='\0')					// wenn stringende, dann
        {
            in=NULL;					// NULL ist NULL
            break;						// schleifende, dann gibt es die NULL zurück
        }
        if (isdigit(*in)) break;		// wenn die nächste zahl beginnt, dann ist auch ende
    }
    *in_out=in;							// rückgabe speichern
    return li;							// Wert zurückgeben
}

/**
 @brief Zeit von einem sqlstring zurückgeben
 @return Zeit von mktime
 @param datestring SQL.String in der Datenbank

 Zeit aus dem SQL.String wie "2025-12-13 15:12:44" nach time_t übersetzen

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Create
___________________________________________________________________________*/

time_t sql_parse_date(char *datestring)
{
    time_t z;
    int y,m,d;
    char *pnt;
    pnt = datestring;					// Modifizierbaren duplicat anlegen
//    printf ("*** %s ***\n", datestring);
    y = getnumber(&pnt)-1900; // zb: 123 = 2023 start ist erst 1970   123-70=53
    m = getnumber(&pnt);
    d = getnumber(&pnt);
    z = ydays(y+1900, m-1, d)-1; // 0-364/365 1.1 muss tag 0 sein wg. *86400
    z = ((z + ((y-70)*365)
            + ((y-69) /4)
            - ((y-1)  /100)
            + ((y+399)/400)
        )*86400);
    z=z + (getnumber(&pnt) * 3600 );
    z=z + (getnumber(&pnt) * 60   );
    z=z + (getnumber(&pnt)        );
    return z;
}

/**
 @brief dummy callback
 @return 0
 @param N               NotUsed VoidPointer
 @param number_of_lines Anzahl der Ergebnisse
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Create
___________________________________________________________________________*/

//int sql_callback_null(void *N, int number_of_lines, char **column_value, char **column_name)
//{
//    return 0;
//}

/**
 @brief get single data callback
 @return 0
 @param N               NotUsed VoidPointer
 @param number_of_lines Anzahl der Ergebnisse
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 25.12.16 HS Create
 ** 26.04.25 HS stolen from global.c(hsvc) and renamed
___________________________________________________________________________*/

int sql_cb_GetSingleData(void *N, int number_of_lines, char **column_value, char **column_name)
{
    sql_Data=free0(sql_Data);
    sql_Data=strdup_ex(&*column_value[0]);
    return 0;
}

/**
 @brief Lese single data Location callback
 @return 0
 @param N               Pointer zu struct place_s
 @param number_of_lines Anzahl der Ergebnisse (3)
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Create
 ___________________________________________________________________________*/

int sql_cb_GetLocation(void *N, int number_of_lines, char **column_value, char **column_name)
{
    struct place_s* E;
    E=(struct place_s*)N;

    E->ID       = atoll                 (&*column_value[0]);
    E->parent_id= atoll                 (&*column_value[1]);
    E->location = strDeEsc  (strNotNULL (&*column_value[2]));
    E->founded  = sql_parse_date        (&*column_value[3]);

    sql_stored=1;
	return 0;
}

/**
 @brief Lese multi data Location callback
 @return 0
 @param N               NotUsed VoidPointer
 @param number_of_lines Anzahl der Ergebnisse (3)
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

 sql_datalst wird mit Node_Add und einem place_s gefüllt

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Create
 ___________________________________________________________________________*/

int sql_cb_GetListLocation(void *N, int number_of_lines, char **column_value, char **column_name)
{
    struct place_s* E;
    E = malloc0(sizeof(struct place_s));

    E->ID       = atoll                 (&*column_value[0]);
    E->parent_id= atoll                 (&*column_value[1]);
    E->location = strDeEsc  (strNotNULL (&*column_value[2]));
    E->founded  = sql_parse_date        (&*column_value[3]);

    sql_datalst=Node_Add(sql_datalst,E,Node_Add_Append);
	return 0;
}

/**
 @brief get a multifield data callback
 @return 0
 @param N               NotUsed VoidPointer
 @param number_of_lines Anzahl der Ergebnisse
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 25.12.16 HS Create
 ** 26.04.25 HS stolen from global.c(hsvc) and renamed
___________________________________________________________________________*/

//int sql_cb_GetMultiData(void *N, int number_of_lines, char **column_value, char **column_name)
//{
    //char *data;
    //data = strdup(&*column_value[0]);
    //HSMultiDataLST = Node_Add(HSMultiDataLST, data, Node_Add_Append);
//    return 0;
//}

/**
 @brief  Speichert die aktulle ID des Eintrags der Liste von sql_testdbGetAll
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param startpoint

___[ Revision ]______________________________________________________________

 ** 15.05.25 HS Initial
___________________________________________________________________________*/

int sql_cb_GetTestDBID(void *N, int number_of_lines, char **column_value, char **column_name)
{
    long_table_insert(atoll(&*column_value[0]));
    return 0;
}

/**
 @brief get a multifield data callback for entrie (passed parameter in N)
 @return 0
 @param N               Pointer from struct entries_s;
 @param number_of_lines Anzahl der Ergebnisse ( das sind 9 )
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 25.12.16 HS Create
 ** 26.04.25 HS stolen from global.c(hsvc) and renamed
___________________________________________________________________________*/

int sql_cb_GetEntrie        (void *N, int number_of_lines, char **column_value, char **column_name)
{
    struct entrie_s* E;
    E=(struct entrie_s*)N;
    char *deescaped;

    memset(E,0,sizeof(struct entrie_s));
    sql_stored++;
    E->ID       = atoll               (&*column_value[0]);
    E->loc_id   = atoll               (&*column_value[1]);
    deescaped   = strDeEsc(strNotNULL (&*column_value[2]));
    strcpy_ex(E->fname, deescaped);
    deescaped   = free0(deescaped);
    E->fsize    = (size_t) atoll      (&*column_value[3]);
    E->fmtime   = (time_t) atoul      (&*column_value[4]);
    E->fcrc32   = (crc32_t) atoul     (&*column_value[5]);
    E->lastchk  = sql_parse_date      (&*column_value[6]);
    E->checkcnt = atol                (&*column_value[7]);
    E->founded  = sql_parse_date      (&*column_value[8]);
    return 0;
}

/**
 @brief start the the recode
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param N               Pointer from struct entries_s;
 @param number_of_lines Anzahl der Ergebnisse ( das sind 9 )
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

 sql_datalst wird um den aktuellen Eintrag erweitert.

 Wrapper, da zum struct einlesen @ref sql_cb_GetEntrie aufgeruften wird.

___[ Revision ]______________________________________________________________

 ** 03.05.25 HS Initial
___________________________________________________________________________*/

int sql_cb_GetEntrieNodes(void *N, int number_of_lines, char **column_value, char **column_name)
{
    struct entrie_s *E;
    E=malloc0(sizeof(struct entrie_s));
    sql_cb_GetEntrie((void*)E, number_of_lines, column_value, column_name);
    sql_datalst=Node_Add(sql_datalst,E,Node_Add_Append);
    return 0;
}

/**
 @brief  Alle Eintraege in die Liste
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param loc_id Welches Verzeichnis einlesen ?

 Der wahre Ergebnisliste in sql_datalist ist das Ergebnis

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_getFullList(sqlid_t loc_id)
{
    int rc;
    char *sql;
    sql_datalst=NULL; // naja sollte nicht sein
    sql = strprintf("SELECT * FROM entries WHERE loc_id=" LLD_STR " ORDER BY fname", loc_id);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetEntrieNodes, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    if (rc==SQLITE_OK) return EXIT_SUCCESS;
    sql_datalst=Node_DelAll(sql_datalst);
    return EXIT_FAILURE;
}

/**
 @brief  Alle Eintraege in die Liste wo CRC&Size und NICHT ID stimmen
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param loc_id Welches Verzeichnis einlesen ?

 Der wahre Ergebnisliste in sql_datalist ist das Ergebnis

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS for finddupes
___________________________________________________________________________*/

int sql_entrie_getFullbyCrcSizeNotID(sqlid_t id, size_t fsize, crc32_t fcrc32)
{
    int rc;
    char *sql;
    sql_datalst=NULL; // naja sollte nicht sein
    sql = strprintf("SELECT * FROM entries WHERE ID!=" LLD_STR " AND fsize=%lu AND fcrc32=%lu ORDER BY fname", id, fsize, fcrc32);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetEntrieNodes, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    if (rc==SQLITE_OK) return EXIT_SUCCESS;
    sql_datalst=Node_DelAll(sql_datalst);
    return EXIT_FAILURE;
}

/**
 @brief Lese subdirs von lname, aber nicht deren subdirs
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param lname locationName

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Initial
 ** 11.05.25 HS jetzt mit ORDER BY
___________________________________________________________________________*/

//int sql_location_getSubdirs(char *lname, void **data)
int sql_location_getSubdirs(sqlid_t id, void **data)
{
    int rc;
    char *sql;
    assert(!sql_datalst);
    sql_datalst=NULL;
    sql = strprintf("SELECT * FROM places WHERE parent_id=" LLD_STR " ORDER BY location", id);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetListLocation, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    *data=sql_datalst;
    sql_datalst=NULL;
    if (rc==SQLITE_OK) return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

/**
 @brief Lese einen Datsatz aus der Datenbank
 @return -1=FAIL / 0 ohne Ergebnis / 1 mit Ergebnis
 @param lname locationName
 @param place struct place_s pointer

___[ Revision ]______________________________________________________________

 ** 02.05.25 HS Initial
___________________________________________________________________________*/

//sql_datalst=Node_Add(sql_datalst,E,Node_Add_Append);
int sql_location_getDir(char *lname, struct place_s *place)
{
    int rc;
    char *sql;
    char *escaped_name;
    sql_stored=0;
    escaped_name=strEsc(lname);
    sql = strprintf("SELECT * FROM places WHERE location=\"%s\"", escaped_name);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetLocation, place, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    free0(escaped_name);
    if (rc!=SQLITE_OK)  return -1; //{ printf ("rc=%i\n",rc); return -1;}
    return sql_stored; // 0 or 1
}

/**
 @brief Lese einen Datsatz aus der Datenbank
 @return -1=FAIL / 0 ohne Ergebnis / 1 mit Ergebnis
 @param id location_ID
 @param place struct place_s pointer

___[ Revision ]______________________________________________________________

 ** 07.05.25 HS Initial
___________________________________________________________________________*/

int sql_location_getByID(sqlid_t id, struct place_s *place)
{
    int rc;
    char *sql;
    sql_stored=0;
    sql = strprintf("SELECT * FROM places WHERE ID=" LLD_STR "", id);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetLocation, place, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    if (rc!=SQLITE_OK)  return -1;
    return sql_stored; // 0 or 1
}

/**
 @brief Lösche ein Verzeichnis komplett aus der Datenbank
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param id die gelöscht wird

 Alle Eintrage in diesem Verzeichnis löschen

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
___________________________________________________________________________*/

int sql_location_delete(sqlid_t id)
{
    int rc;
    char *sql;
    sql = strprintf("DELETE FROM entries WHERE loc_id=" LLD_STR "", id);
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc,sql_ErrMsg,sql);
        free0(sql);
        return EXIT_FAILURE;
    }
    free0(sql);
    sql = strprintf("DELETE FROM places WHERE id=" LLD_STR "", id);
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc,sql_ErrMsg,sql);
        free0(sql);
        return EXIT_FAILURE;
    }
    free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief Ergebnis ist eine Liste (es wird "select ID,fname..." ausgegangen)
 @return 0
 @param N               NotUsed VoidPointer
 @param number_of_lines Anzahl der Ergebnisse
 @param column_value    Was steht drin **strlst** - kann NULL enthalten
 @param column_name     Wie heisst die Spalte **strlst**

___[ Revision ]______________________________________________________________

 ** 30.04.25 HS Initial
___________________________________________________________________________*/

int sql_cb_BuildEntryLst(void *N, int number_of_lines, char **column_value, char **column_name)
{
    char *deescaped;
    struct entrie_small_s* E;
    E=malloc0(sizeof(struct entrie_small_s));

    E->ID       = atoll(&*column_value[0]);
    deescaped   = strDeEsc(strNotNULL (&*column_value[1]));
    strcpy_ex(E->fname, deescaped);
    deescaped   = free0(deescaped);
    sql_datalst=Node_Add(sql_datalst,E,Node_Add_Append);
	return 0;
}

/**
 @brief  open database in HSguard_DB
 @return NULL OK, or an ErrorMsg

___[ Revision ]______________________________________________________________

 ** 24.04.25 HS Create
___________________________________________________________________________*/

char *open_valid_sqlite3(void)
{
    int n,rc;
    char header_string[16];
    static char rtErrorMsg[STRING_MAX];
    char *sql;
    FILE *IN;

    if (HSguard_DB[0]==0)                           { sprintf_ex(rtErrorMsg,"No DB was specified. Use --db or DATABASE in hsguard.rc", HSguard_DB); return rtErrorMsg; }
    if (!FileOK(HSguard_DB))                        { sprintf_ex(rtErrorMsg,"%s - does not exists; --initdb may help", HSguard_DB); return rtErrorMsg; }
    if ((IN=fopen(HSguard_DB,"rb")) == NULL)        { sprintf_ex(rtErrorMsg,"%s - can't be open", HSguard_DB); return rtErrorMsg; }
    n=fread(header_string,1,sizeof(header_string),IN);
    fclose (IN);
    if (n!=sizeof(header_string))                   { sprintf_ex(rtErrorMsg,"%s - header size error", HSguard_DB); return rtErrorMsg; }
    if (strcmp("SQLite format 3",header_string))    { sprintf_ex(rtErrorMsg,"%s - header string does not \"SQLite format 3\"", HSguard_DB); return rtErrorMsg; }
    rc = sqlite3_open(HSguard_DB, &sq3db);
    if (rc!=SQLITE_OK)                              { sprintf_ex(rtErrorMsg,"%s - sql open error", HSguard_DB); return rtErrorMsg; }

    sql="select count() from sqlite_master";
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sqlite3_close(sq3db);
                                                      sprintf_ex(rtErrorMsg,"%s - sql get sqlite_master count error", HSguard_DB);
        return rtErrorMsg;
    }

#ifdef OS_LINUX
    sql="PRAGMA case_sensitive_like=ON;";
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sqlite3_close(sq3db);
        sprintf_ex(rtErrorMsg,"pragma not set", HSguard_DB);
        return rtErrorMsg;
    }
#endif
    return NULL;
}

/**
 @brief Einen neuen Datensatz (entrie) anlegen in der Datenbank
 @return -1 Error / oder die ID des Datensatzes
 @param loc_id locatio ID von places
 @param fname Filename
 @param fsize Dateigröße
 @param fmtime Filetime im unixepoche format  (localtime)
 @param crc32 simple crc32 as integer (uint_32_t)

 Neuen Datendatz in der Datenbank anlegen

___[ Revision ]______________________________________________________________

 ** 29.04.25 HS Initial
 ** 23.05.25 HS stat exit_* ID zurückgeben
___________________________________________________________________________*/

sqlid_t sql_entrie_insert(sqlid_t loc_id, char *fname, size_t fsize, time_t fmtime, crc32_t crc32)
{
    int rc;
    char *escaped_name;
    char *sql;
    sqlid_t id;

    escaped_name=strEsc(fname);
    sql = strprintf("INSERT INTO entries ( loc_id, fname, fsize, fmtime, fcrc32, checkcnt ) "
                    "VALUES ( " LLD_STR ", \"%s\", %lu, " TIME_STR_LU " , %lu, 1 )",
                    loc_id, escaped_name, fsize, fmtime, crc32);
    free0(escaped_name);
    rc  = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);

//    if (rc!=SQLITE_OK)
//    {
//        sql_ShowError(rc,sql_ErrMsg,sql);
//        free0(sql);
//        return EXIT_FAILURE;
//    }
//    sql=free0(sql);
//    return EXIT_SUCCESS;

    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc,sql_ErrMsg,sql);
        id=-1;
    }else{
        id=sqlite3_last_insert_rowid(sq3db);
    }
    sql=free0(sql);
    return id;

}
/**
 @brief Update eines Datenbank Eintrags fsize/fmtime/st_crc32
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param id       sql_id des entries
 @param fsize    filesize
 @param fmtime   filemodiftime
 @param st_crc32 die crc32 des files

___[ Revision ]______________________________________________________________

 ** 29.04.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_update(sqlid_t id, size_t fsize, time_t fmtime, crc32_t st_crc32)
{
    int rc;
    char *sql;
    sql = strprintf("UPDATE entries SET fsize=%lu"
                    ",fmtime=" TIME_STR_LU ",fcrc32=%lu"
                    ",lastchk=datetime('now','localtime'),checkcnt=(checkcnt+1) WHERE ID=" LLD_STR "",
                     fsize, fmtime, st_crc32, id);
//    printf ("%s\n",sql);
    rc  = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc,sql_ErrMsg,sql);
        free0(sql);
        return EXIT_FAILURE;
    }
    free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief Bilde eine Liste aus wo die loc_id der in der Datenbank stimmt
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param loc_id   sql_id des places

___[ Revision ]______________________________________________________________

 ** 30.04.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_list(sqlid_t loc_id)
{
    int rc;
    char *sql;

	sql_datalst=Node_DelAll(sql_datalst);
    sql = strprintf("SELECT ID,fname from entries WHERE loc_id=" LLD_STR " ORDER by fname", loc_id);
    rc = sqlite3_exec(sq3db, sql, sql_cb_BuildEntryLst, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sql_datalst=Node_DelAll(sql_datalst);
        sql=free0(sql);
        return EXIT_FAILURE;
    }
    sql=free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief Bilde eine Liste aus wo die fcrc32 in der Datenbank stimmt
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param crc welche CRC ist zu finden

___[ Revision ]______________________________________________________________

 ** 12.05.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_listByCRC(crc32_t crc)
{
    int rc;
    char *sql;

	sql_datalst=Node_DelAll(sql_datalst);
    sql = strprintf("SELECT ID,fname from entries WHERE fcrc32=%lu", crc);
    rc = sqlite3_exec(sq3db, sql, sql_cb_BuildEntryLst, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sql_datalst=Node_DelAll(sql_datalst);
        sql=free0(sql);
        return EXIT_FAILURE;
    }
    sql=free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief  Bilde eine Liste aus wo die fname (like) in der Datenbank stimmt
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  fname filename

___[ Revision ]______________________________________________________________

 ** 12.05.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_listLikeFname(char *fname)
{
    int rc;
    char *sql;
    char *escaped;

	sql_datalst=Node_DelAll(sql_datalst);
    escaped=strEsc(fname);
    sql = strprintf("SELECT ID,fname from entries WHERE LOWER(fname) GLOB LOWER('%s')", escaped);
    rc = sqlite3_exec(sq3db, sql, sql_cb_BuildEntryLst, NULL, &sql_ErrMsg);
    free(escaped);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sql_datalst=Node_DelAll(sql_datalst);
        sql=free0(sql);
        return EXIT_FAILURE;
    }
    sql=free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief loesche einen entries anhand der ID
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 30.04.25 HS Initial
___________________________________________________________________________*/

int sql_entrie_delete(sqlid_t id)
{
    int rc;
    char *sql;
    sql = strprintf("DELETE from entries WHERE ID=" LLD_STR "", id);
//    rc=SQLITE_OK; printf ("%s\n", sql);
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sql=free0(sql);
        return EXIT_FAILURE;
    }
    sql=free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief Hole nach ent ausgehend von locationID und filename
 @return -1 Error / 0 notfound / oder die ID der Datei
 @param loc_id location ID
 @param fname Filename
 @param ent   ein passend großer Speicher für einen @ref struct entrie_s

  @code
struct entrie_s =>ent wir von folgendem SQL geholt
[ID] 		    INTEGER DEFAULT '1' NOT NULL PRIMARY KEY AUTOINCREMENT,\n\
[loc_id]	    INTEGER 			NOT NULL,\n\
[fname] 	    VARCHAR(256)  		NOT NULL,\n\
[fsize]		    INTEGER			    NOT NULL,\n\
[fmtime]	    DATETIME			NOT NULL,\n\
[fcrc32]	    INTEGER			    NOT NULL,\n\
[lastchk]	    DATETIME            DEFAULT (datetime('now', 'localtime')) NOT NULL,\n\
[checkcnt]	    INTEGER				NOT NULL DEFAULT '0',\n\
[founded]       DATETIME            DEFAULT (datetime('now', 'localtime')) NOT NULL\n\
 @endcode

___[ Revision ]______________________________________________________________

 ** 29.04.25 HS Initial
___________________________________________________________________________*/

sqlid_t sql_entrie_getID (sqlid_t loc_id, char *fname, struct entrie_s *ent)
{
    int rc;
    char *sql;
    char *escaped_name;
    sql_stored=0;
    escaped_name=strEsc(fname);
    sql = strprintf("SELECT * FROM entries WHERE loc_id=" LLD_STR " AND fname=\'%s\'", loc_id, escaped_name);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetEntrie, ent, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    free0(escaped_name);
    if (rc!=SQLITE_OK) return -1;
    if (sql_stored==0)  return 0;
    return             ent->ID;
}

/**
 @brief Lese Eintrag, wenn die ID bekannt ist
 @return -1 Error / 0 notfound / oder die ID der Datei
 @param id in der entries DB
 @param pointer auf struct entrie_s

 Es wird versucht die ID zu lesen. Wenn gefunden, dann wird das Ergebnis
 in ent komplett gespeichert.

___[ Revision ]______________________________________________________________

 ** 06.05.25 HS Initial
___________________________________________________________________________*/

sqlid_t sql_entrie_getByID (sqlid_t id, struct entrie_s *ent)
{
    int rc;
    char *sql;
    sql_stored=0;
    sql = strprintf("SELECT * FROM entries WHERE ID=" LLD_STR "", id);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetEntrie, ent, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    free0(sql);
    if (rc!=SQLITE_OK) return -1;
    if (sql_stored==0)  return 0;
    return             ent->ID;
}
/**
 @brief id eines verzeichnisses finden
 @return -1 Error / 0 notfound / oder die ID des Verzeichnisses
 @param startpoint

___[ Revision ]______________________________________________________________

 ** 29.04.25 HS Initial
___________________________________________________________________________*/

sqlid_t sql_location_getID(char *location)
{
    int rc;
    sqlid_t loc_id;
    char *escaped;
    char *sql;

    // finde ID in der Datenbank
    loc_id=0;
    escaped = strEsc(location);
    sql = strprintf("SELECT ID from places where location=\"%s\"", escaped);
    escaped=free0(escaped);
    sql_Data=free0(sql_Data);
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetSingleData, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        loc_id=-1;
    }
    sql=free0(sql);
    if (sql_Data) loc_id=atoi(sql_Data);
//    printf ("%s return " LLD_STR "\n", location,loc_id);
    return loc_id;
}

/// Für das Verlassen des Update, alte Index weg
char *sql_tabs_drop[]={
    "DROP INDEX IF EXISTS plaidx",
    "DROP INDEX IF EXISTS entidx",
    "DROP INDEX IF EXISTS paridx",
    NULL
};
/// Für das Update
char *sql_tabs_create[]={
    "CREATE INDEX plaidx ON places(location)",
    "CREATE INDEX entidx ON entries(loc_id)",
    "CREATE INDEX paridx ON places(parent_id)",
    NULL
};

/**
 @brief  char** liste an der sql abarbeiten
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  cmdidx pointer auf die charliste

___[ Revision ]______________________________________________________________

 ** 29.05.25 HS Initial
___________________________________________________________________________*/

int sql_location_multi(char **cmdidx)
{
    int i;
    int rc;
    for (i=0;cmdidx[i];i++)
    {
        rc = sqlite3_exec(sq3db, cmdidx[i], NULL, NULL, &sql_ErrMsg);
        if (rc!=SQLITE_OK)
        {
            sql_ShowError(rc, sql_ErrMsg, cmdidx[i]);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 @brief  Alle Indexe austragen [sql_tabs_drop]
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 28.05.25 HS Initial
___________________________________________________________________________*/

int sql_location_index_drop(void)   // drop index if EXISTS plaidx;
{
    if (sql_location_multi(sql_tabs_drop)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

/**
 @brief  Alle Indexe erstellen [sql_tabs_create]
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 28.05.25 HS Initial
___________________________________________________________________________*/

int sql_location_index_create(void) // CREATE INDEX plaidx ON places(location);
{
    if (sql_location_multi(sql_tabs_create)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

/**
 @brief ein neues verzeichnis anlegen
 @return -1 Error / 0 notfound / oder die ID des Verzeichnisses
 @param startpoint

___[ Revision ]______________________________________________________________

 ** 29.04.25 HS Initial
 ** 27.05.25 HS parent_id
___________________________________________________________________________*/

sqlid_t sql_location_insert(char *location, sqlid_t pid)
{
    int rc;
    sqlid_t loc_id;
    char *escaped;
    char *sql;

    escaped=strEsc(location);
    sql = strprintf("INSERT INTO places (location,parent_id) VALUES (\"%s\"," LLD_STR ")", escaped, pid);
//    printf ("%s\n",sql);
    rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc,sql_ErrMsg,sql);
        loc_id=-1;
    }else{
        loc_id=sqlite3_last_insert_rowid(sq3db);
    }
    escaped=free0(escaped);
    sql=free0(sql);
    return loc_id;
}
/**
 @brief  Verzeichnisname anhand der ID holen
 @return Verzeichnisname, wenn nicht NULL, dann mit free Speicher freigeben
 @param  loc_id     sql_id des places

 rslt must freed

___[ Revision ]______________________________________________________________

 ** 28.04.25 HS Initial
___________________________________________________________________________*/

char *sql_location_getDirname(sqlid_t loc_id)
{
    int rc;
    char *sql;
    char *rslt;

    if (loc_id<1) return NULL;                                                  // Zur Sicherheit
    if ((sql_lastdirnum>0)&&(sql_lastdirname))                                  // möglicher Cache Zugriff
    {
        if (loc_id==sql_lastdirnum) return strdup_ex(sql_lastdirname);
    }
    sql_Data= free0(sql_Data);                                                  // Daten für getSingleData
    sql = strprintf("SELECT location from places where ID=" LLD_STR "", loc_id );
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetSingleData, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
    sql=free0(sql);
    rslt=NULL;
    if (sql_Data) rslt=strDeEsc(sql_Data);                                      // Wenn Datenvorhanden, dann zuweisen
    return rslt;                                                                // Ergebnis
}

/**
 @brief  entrie Records in der Datenbank mit Abfrage
 @return -1=Error / Anzahl Record
 @param  loc_id location in places

___[ Revision ]______________________________________________________________

 ** 03.05.25 HS Initial
 ** 04.05.25 HS getCount war keine gute Lösung, wieder vereinzelnd
___________________________________________________________________________*/

int sql_entrie_count (sqlid_t loc_id)
{
    int rc;
    char *sql;
    sql = strprintf("SELECT count() FROM entries WHERE loc_id=" LLD_STR "", loc_id);
    sql_Data= free0(sql_Data);                                                  // Daten für getSingleData
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetSingleData, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK) sql_ShowError(rc,sql_ErrMsg,sql);
//    lmsg(mFAIL,"rc=%i data=\"%s\" sql=\"%s\"",rc, strNotNULL(sql_Data), strNotNULL(sql));
    sql=free0(sql);
    if (rc==SQLITE_OK)
    {
        if (sql_Data) return atoi(sql_Data);                                    // Wenn Datenvorhanden, dann zuweisen
    }
    return -1;
}

/**
 @brief  Wird benutzt um ein Feld von (100) ID's anzulegen, die geprüft werden
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  location Ab hier eine Liste
 @param  maxids   Anzahl der ID, die geholt werden
 @param  rdm      1 zufälliger Wert, 0 Die ältesten nacht lastchk

 Es wird mit long_table ein Feld von maxids angelegt. Je nach rdm wird dabei
 strategisch ( die am längsten nicht geprüften Dateien zuerst) oder als chaos
 random() via sql abgerufen.

___[ Revision ]______________________________________________________________

 ** 15.05.25 HS Initial
___________________________________________________________________________*/

int sql_testdbGetAll(char *location, int maxids, int rdm)
{
    int rc;
    char *escaped;
    char *sql;
    escaped = strEsc(location);
    sql = strprintf(
            "select entries.ID as eID"
            " FROM ( entries INNER JOIN places ON entries.loc_id=places.ID )"
            " WHERE places.location LIKE \"%s%%\""
            " ORDER BY %s"   // 18.05.25 HS  ^remove /
            " LIMIT %i",
            escaped,
            rdm?"random()":"entries.lastchk,entries.fname desc",
            maxids);
    escaped=free0(escaped);
    long_table_free();
//    printf ("sql: %s\n",sql); // select entries.ID as eID,entries.fname as eFName,entries.lastchk as eFmtime,entries.fsize as eFsize,entries.fcrc32 as eFcrc32,places.location as pLocation FROM ( entries INNER JOIN places ON entries.loc_id=places.ID )
// WHERE places.location LIKE "/hs/b/%" ORDER BY entries.lastchk, entries.fname desc limit 2
    rc = sqlite3_exec(sq3db, sql, sql_cb_GetTestDBID, NULL, &sql_ErrMsg);
    if (rc!=SQLITE_OK)
    {
        sql_ShowError(rc, sql_ErrMsg, sql);
        sql=free0(sql);
        return EXIT_FAILURE;
    }
    sql=free0(sql);
    return EXIT_SUCCESS;
}

/**
 @brief tatsächlicher Erstelle eine leere Datenbank
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
___________________________________________________________________________*/

char *initDB_cmd1="\n\
PRAGMA foreign_keys=OFF;\n\
BEGIN TRANSACTION;\n\
create table [places] (\n\
[ID]            INTEGER         DEFAULT '1' NOT NULL PRIMARY KEY AUTOINCREMENT,\n\
[parent_id]     INTEGER 		NOT NULL,\n\
[location]      VARCHAR[##PATH_MAX##]   NOT NULL,\n\
[founded]       DATETIME        DEFAULT (datetime('now', 'localtime')) NOT NULL\n\
);\n\
COMMIT;\n\
";

char *initDB_cmd2="\n\
BEGIN TRANSACTION;\n\
CREATE TABLE [entries] (\n\
[ID] 		    INTEGER DEFAULT '1' NOT NULL PRIMARY KEY AUTOINCREMENT,\n\
[loc_id]	    INTEGER 			NOT NULL,\n\
[fname] 	    VARCHAR(##NAME_MAX##)  		NOT NULL,\n\
[fsize]		    INTEGER			    NOT NULL,\n\
[fmtime]	    INTEGER			    ,\n\
[fcrc32]	    INTEGER			    NOT NULL,\n\
[lastchk]	    DATETIME            DEFAULT (datetime('now', 'localtime')) NOT NULL,\n\
[checkcnt]	    INTEGER				NOT NULL DEFAULT '0',\n\
[founded]       DATETIME            DEFAULT (datetime('now', 'localtime')) NOT NULL\n\
);\n\
COMMIT;\n\
";
/*
CREATE TABLE IF NOT EXISTS hashes
    (hashval INTEGER NOT NULL,
    sketch_id INTEGER NOT NULL,
    FOREIGN KEY (sketch_id) REFERENCES sketches (id))

CREATE TABLE IF NOT EXISTS sketches
  (id INTEGER PRIMARY KEY,
   name TEXT,
   ...)

*/
/**
 @brief Einen String zum Erstellen einer Tabelle bauen
 @return übersetzter string muss freed werden
 @param num string number

 Einen String zum Erstellen einer Tabelle bauen

 Inhalte werden ersetzt PATH_MAX mit 4096 unter Linux
 MAC und Windows u.a. können abweichen. NAME_MAX auch.

 Das Ergebnis muss freeed werden

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
___________________________________________________________________________*/

char *getInitDBstr(int num)
{
    char *value;
    value = "";
    switch (num)
    {
      case 1: value=strdup_ex(initDB_cmd1); break;
      case 2: value=strdup_ex(initDB_cmd2); break;
      default: die ("this never happens");  break;
    }
    strstrreplace(value, "##PATH_MAX##", strnum(PATH_MAX));
    strstrreplace(value, "##NAME_MAX##", strnum(NAME_MAX+1));
    return value;
}

/**
 @brief tatsächliches anlegen der Datenbank
 @return EXIT_FAILURE / EXIT_SUCCESS

 sq3db wird zur Datenbank verbundem

___[ Revision ]______________________________________________________________

 ** 25.04.25 HS Initial
___________________________________________________________________________*/

int sql_create_database(void)
{
    int c;
    int rc;
    char *sql;
    char rtErrorMsg[STRING_MAX];

    rc = sqlite3_open(HSguard_DB, &sq3db);
    if (rc!=SQLITE_OK) die ("Database can't create here");
    for (c=1;c<3;c++)
    {
        sql=getInitDBstr(c);
        rc = sqlite3_exec(sq3db, sql, NULL, NULL, &sql_ErrMsg);
//printf ("%s", sql);
        free(sql);
        if (rc==SQLITE_OK) continue;
        sprintf (rtErrorMsg, ":: initDB_cmd%02i ::" ,c);
        sql_ShowError(rc, sql_ErrMsg, rtErrorMsg);
        sqlite3_close(sq3db);
        cleanup(EXIT_FAILURE);
        die(NULL);
    }
    sqlite3_close(sq3db);
    return EXIT_FAILURE;
}
