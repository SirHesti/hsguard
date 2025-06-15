#include "hsguard.h"

int update_dircount=0;
int update_dirsDBcount=0;
int update_dirsDBdeleted=0;

/**
 @brief starten den Updatevorgang
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 24.04.25 HS Initial
 ** 27.05.25 HS verwendung von ensureDIR
___________________________________________________________________________*/

int Execute_update(void)
{
    int rc;
    struct place_s place;

    sql_location_index_drop();
//    if (update_dir) { if (update_dir[0]=='@')
    if (ensureDir(&update_dir,basis)) return EXIT_FAILURE;
    if (cmd_isNoScreen) lmsgGuard(0);
    lmsg (mUSER, "begin update in %s",update_dir);
    sql_location_index_create();
    rc=update_new(update_dir, update_deep);
    lmsg (mUSER, "end   update in %s subs: %i",update_dir,update_dircount);
    if(!rc)
    {
        if (!cmd_isNoDirTest)
        {
            lmsg (mUSER, "begin empty dirchecks in %s",update_dir);
            if (sql_location_getDir(update_dir, &place)>0)
            {
                rc=update_dirsDB(&place,update_deep);
                lmsg (mUSER, "end   empty dirchecks in %s subs: %i deleted: %i",update_dir,update_dirsDBcount,update_dirsDBdeleted);
            }
        }
    }
    sql_location_index_drop();
    return rc;
}

/**
 @brief  ID aus der Datenbank(Cache) holen
 @return location ID - kann als location als Zeichenkette geholt werden
 @param  dirname    das Verzeichnis das gefunden werden soll
 @param  flags      Hier kann das Verhalten gesteuert werden

 loc_id aus der Datenbank, wenn vorhanden

 Flags steuert ob das gecached werden darf, vom Cache gelesen werden)
 Ebenso kann mit F_CREATE_DIR ein Eintrag in der Datenbank angelegt werden
 darf.

___[ Revision ]______________________________________________________________

 ** 27.04.25 HS Initial
 ** 27.05.25 HS parent_id
___________________________________________________________________________*/

sqlid_t update_getdirnum(char *dirname, int flags)
{
    sqlid_t loc_id;
    sql_stored=0;
    if (flags&F_CREATE_CACHED)
    {
        if ((sql_lastdirnum>0)&&(sql_lastdirname))
        {
            if (!strcmp(dirname,sql_lastdirname)) return sql_lastdirnum;
        }
    }
    loc_id=sql_location_getID(dirname);         // -1,0,ID
    if ((loc_id<1)&&(flags&F_CREATE_DIR))
    {
        sqlid_t parent;

        parent=0;
        if (strcmp(basis, dirname))
        {
            parent=update_getdirnum(Cdirname(dirname), flags); //sql_location_getID(Cdirname(dirname));
            if (parent<1)
            {
                lmsg (mROOT, "parent not found for %s", dirname);
                die (NULL);
            }
        }
        sql_stored++;
        loc_id=sql_location_insert(dirname,parent);
    }

    if (loc_id>0)
    {
        sql_lastdirname = free0(sql_lastdirname);
        sql_lastdirname = strdup_ex(dirname);
        sql_lastdirnum  = loc_id;
    }
    return loc_id;
}

/**
 @brief  Einlesen der Verzeichnisstruktur
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  dirname    Verzeichnis ab dem getstartet wird (von update_dir oder basis)
 @param  deep       Verzeichnistiefe (upsprünglich abgeleitet aus update_deep)

 Einlesen der Verzeichnisstruktur ab dirname bis deep erreicht ist. Geänderte,
 neue Einträge werden geupdatet.

 Doese Routine ruft sich selbst recursiv auf solange bis deep==0

 wird -1 übergeben wir deep==0 nicht eintreten.

 Eine Verzeichnistiefe wird je nach Betriebsystem berechnet. Im Fall von Linux
 ist das 4096(PATH_MAX), bei Windows 256(_MAX_PATH). Bei 4096 muss jedes
 Unterverzeichnis 2 Zeichen betragen /a/b/c sind 3 Unterverschachtellungen, aber
 Verzecihnislänge sind 6 chars(bytes). Das sind hochgerechnet 2048
 Unterverzeichnisse. Wobei der String NULL-terminiert ist. Es soll auch nur
 verdeutlicht werden, dass die tiefe nur 2047 sein kann. Es kann natürlich
 passieren das der Speicher ausläuft. Ist aber nur ein hypothetischer Fall.

 Verzeichnisse in der Exclude-Liste werden nicht bearbeitet.

___[ Revision ]______________________________________________________________

 ** 25.04.25 HS Initial
 ** 30.04.25 HS Mehrfach redesigned wenn Teile fertig auch ausgelagert
___________________________________________________________________________*/

int update_new(char *dirname, int deep)
{
    int i;
    sqlid_t loc_id;
    DIR *dir;

    char **dirlst;

    void *dblst;
    void *dbsearch;
    struct entrie_s *dbent;
    int dbfound;

    struct dirent *dent;
    struct stat st;
    char *local_name;

    update_mover(update_dircount++);
    dirlst=NULL;

    if (access(dirname,R_OK)!=0) return EXIT_FAILURE;                           // Zugang ?
    if (!update_excllst_find(dirname)) return EXIT_SUCCESS;                     // Im Ausnahmeverfahren
    loc_id=update_getdirnum(dirname,F_CREATE_DIR|F_CREATE_CACHED);
    if (sql_stored)                                                             // neues Verzeichnis ?
    {
        lmsg(mUSER, "new DB-Dir(" LLD_STR "): %s", loc_id, dirname );
    }else{
        lmsg(mDEVL, "[%i] loc_id %s => " LLD_STR, deep, dirname, loc_id );
    }
    // select * from entries where loc_id=$loc_id;
    if (sql_entrie_getFullList(loc_id)) return EXIT_FAILURE;
    dblst=get_sql_datalst(); // dblst=sql_datalst; sql_datalst=NULL;

    // Scan Directory und wenn FILE, dann File aufnehmen oder wenn deep!=0 dir aufnehmen(recursiv)
    dir = opendir(dirname);                                                     // Verzeichnis öffnen
    if (!dir)
    {
        lmsg (mFAIL, "can't opendir: %s", dirname);
        return EXIT_FAILURE;
    }
//    lmsg(mDEVL, "[%i] scann loc_id %s => " LLD_STR, deep, dirname, loc_id );
    for (local_name=NULL;;)
    {
        dent=readdir(dir);
        if (dent==NULL) break;
        if (dent->d_name[0]=='.') // unseen or file with dot or directory upper
        {
            if (!strcmp(dent->d_name,"."))  continue;                           // Diese beiden ergäben keinen Sinn
            if (!strcmp(dent->d_name,"..")) continue;
        }
        local_name = free0(local_name);                                         // freigeben falls besetzt
        local_name = strprintf("%s/%s", strcmp(dirname, "/")?dirname:"", dent->d_name);
        if (!update_excllst_find(local_name)) continue;                         // Als Datei in der Excludeliste ?

        if (lstat (local_name, &st)) continue;                                  // kann stat einer Datei nicht bearbeiten
        if (S_ISLNK(st.st_mode))    continue;                                   // Links werden nicht unterstützt
        if (S_ISDIR(st.st_mode))                                                // Verzeichnis
        {
            lmsg(mDEVL,"add 2 dirlst %s", local_name);
            dirlst=strlstadd(dirlst, strdup_ex(local_name));
            continue;
        }
        if (!S_ISREG(st.st_mode)) continue;                                     // Normale Datei oder Ende

        if (access(local_name,R_OK)!=0) continue;                               // Zugriffsrechte auf diese Datei nicht vorhanden
        // dateien mit länge 0 können aufgenommen, aber nicht geprüft werden dh. keine CRC, wenn aber CRC-File, dann konnte es geöffnet werden
        // in der aktuellen DB Liste nachsehen, wenn gefunden, dann update_file aufrufen ( prüfen ob update notwendig und ausführen !!)
        dbfound=0;
        if (dblst)
        {
            for (dbsearch=dblst;dbsearch;dbsearch=Node_GetNext(dbsearch))
            {
                dbent=(struct entrie_s*)Node_GetData(dbsearch);
                if (!strcmp(dbent->fname,dent->d_name))                         // Namen sind geprüft und gefunden
                {   // -1 od. 1
                    dbfound=update_file(local_name, dbent, st.st_size, st.st_mtim.tv_sec); // ggf. Error mit übernehmen
                    dblst=Node_DelOne(dblst,dbsearch);                          // Aus der dblst entfernen
                    break;
                }
            }
        }
        if (dbfound==0) // noch nicht in der Datenbank
        {
            if (update_insert_makeCRC32 (local_name, loc_id, dent->d_name, st.st_size, st.st_mtim.tv_sec)) return EXIT_FAILURE;
        }
//        if (dbfound>=0) (gibs so nicht mehr) filelst = strlstadd(filelst, strdup_ex(dent->d_name));              // filelst enthält alle files, die nicht aus der DB fliegen sollen
//        lmsg (GMSG_DEVL, "use : 0x%08x %s ", st_crc32, local_name);
//        printf ("%s\n", dent->d_name);
    }
    closedir(dir);
    local_name = free0(local_name);

    if (dblst)                              // was darf denn übrig sein ?
    {
        for (dbsearch=dblst;dbsearch;dbsearch=Node_GetNext(dbsearch))
        {
            dbent=(struct entrie_s*)Node_GetData(dbsearch);
//            lmsg (mDEVL, "dblst - unhappy: Lid:" LLD_STR " Fid:" LLD_STR " %s", dbent->loc_id, dbent->ID, dbent->fname);
            lmsg (mUSER, "delete from entries " LLD_STR " / %s", dbent->ID, dbent->fname);
            sql_entrie_delete(dbent->ID);
        }
        dblst=Node_DelAll(dblst);
    }
    // ab hier alles freigegeben bis auf dirlst
    // -1 oder 0+ ; wenn genau 0 erreicht, dann nicht tiefer gehen
    // -1 um eins kleiner ist -2 und immer noch nicht 0...
    if (deep!=0)
    {
        if (dirlst)
        {
            for (i=0;dirlst[i];i++)
            {
                if (access(dirlst[i],R_OK)==0)
                {
//                    printf ("dir->%s\n",dirlst[i]);
                    if (update_new(dirlst[i],deep-1)) EXIT_FAILURE;
                }
            }
        }
    }
    dirlst=strlstfree(dirlst);
    return EXIT_SUCCESS;
}

/**
 @brief Erstelle eine crc32 für file an log_id präsenz
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param local_name vollständiger name incl. Pfad
 @param loc_id  location ID - kann aus location als Zeichenkette geholt werden
 @param fname   filename der updated wird
 @param fsize   filesize >0 sonst crc auch 0
 @param fmtime  modtime

 Es wird eine CRC32 gebildet. Die kann auch 0 sein, wenn das File keine
 Size hat.

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
 ** 23.05.25 HS Umbau für neues update ehemals 'update_getcrc32'
 ** 31.05.25 HS cmd_isWarnbigsz
___________________________________________________________________________*/

int update_insert_makeCRC32 (char *local_name, sqlid_t loc_id, char *fname, size_t fsize, time_t fmtime)
{
    crc32_t st_crc32;
    sqlid_t inserted_id;

    if (fsize>0)                                                                // Size>0
    {
        if (cmd_isWarnbigsz) if (fsize>cmd_isWarnbigsz) lmsg(mINFO, "BIG SIZE: %s  -  %s", Num2Human((double)fsize,0), local_name);
        if (CRC32File(local_name,&st_crc32))                                    // ja, dann CRC vom File
        {
            lmsg (mFAIL, "update(2) file CRC error %s", local_name);
            return EXIT_FAILURE;
        }
    }else{
        st_crc32 = 0LU;                                                         // size=0, geht kleiner?
    }
    inserted_id=sql_entrie_insert(loc_id, fname, fsize, fmtime, st_crc32);      // ID falls funktioniert
    if (inserted_id<1)
    {
        lmsg (mFAIL, "insert failure (" LLD_STR ") %s", loc_id, local_name);
        return EXIT_FAILURE;
    }
    lmsg (mUSER, "insert locationID=" LLD_STR " entrieID=" LLD_STR " file=%s", loc_id, inserted_id, fname);
    return EXIT_SUCCESS;
}

/**
 @brief update file, wenn file neu oder (fsize oder fmtime geändert)
 @return -1 Fail, 1 gefunden oder gefunden und upgedatet
 @param local_name  location als Zeichenkette komplett mit filenamen
 @param dbent       kompletter Eintrag aus der Datenbank
 @param fsize       filesize >0 sonst crc auch 0
 @param fmtime      mod time als unixtime

 Der Eintrag ist in der Struktur dbent (@ref struct entrie_s)
 abgebildet und kann mit fsize und fmtime auf geprüft werden ob es eine
 Änderung gab. Wenn nicht, dann verlassen (1)

 Ansonsten wird die neue CRC32 ermittelt und in der Datenbank eingetragen
 mit fsize/fmtime/crc32 .

___[ Revision ]______________________________________________________________

 ** 25.04.25 HS Initial
 ** 23.05.25 HS Neu. Weitgehend
 ** 31.05.25 HS cmd_isWarnbigsz
___________________________________________________________________________*/
  // dbfound=update_file( local_name, dbent, st.st_size, st.st_mtim.tv_sec)

int update_file (char *local_name, struct entrie_s *dbent, size_t fsize, time_t fmtime)
{
    crc32_t st_crc32;
    int dbfound;

    dbfound=1;                                                  // wenn size und fmtime gleich und kein rescan dann keine neue crc32 nötig
    if (   (cmd_isRescanFlg!=0)                                 // Kein Rescan ist kein Rescan
        || (dbent->fsize!=fsize)                                // Size +
        || (dbent->fmtime!=fmtime)                              // filetime müssen gleich sein
       )
    {

    /*
    lmsg(mUSER,"cmd_isRescanFlg=%i  file.size=%d  db.fsize=%d  %s   file.mtime=%s  db.mtime=%s  %s",
                                cmd_isRescanFlg,
                                              fsize,
                                                           dbent->fsize,
                                                               fsize==dbent->fsize?"OK":"[not equal]",
                                                                               timeinfostr(fmtime),
                                                                                            timeinfostr(dbent->fmtime),
                                                                                                fmtime==dbent->fmtime?"OK":"[not equal]"
    );*/
        if (cmd_isWarnbigsz) if (fsize>cmd_isWarnbigsz) lmsg(mINFO, "BIG SIZE: %s  -  %s", Num2Human((double)fsize,0), local_name);
        if (CRC32File(local_name,&st_crc32))                    /*CRC vom File*/
        {
            dbfound=-1;     // update CRC für file in loc_id
            lmsg (mFAIL, "update file CRC error %s", local_name);
        }else{
            /* UPDATE LASTCHECK; if fail dbfouund=-1*/
            if (sql_entrie_update(dbent->ID, fsize, fmtime, st_crc32))
            {
                dbfound=-1;     // update CRC für file in loc_id
                lmsg (mFAIL, "sql update lastcheck error %s", local_name);
            }else{
//                lmsg (mUSER, "update locationID=" LLD_STR " entrieID=" LLD_STR " file=%s", dbent->loc_id, dbent->ID, dbent->fname);
                lmsg (mUSER, "location=%s   rescan=%i  dbmtime=%s  fmtime=%s",local_name , cmd_isRescanFlg, timeinfostr(dbent->fmtime), timeinfostr(fmtime));
//                lmsg (mUSER, "time=" TIME_STR_LU " / " TIME_STR_LU "", dbent->fmtime, fmtime);
//                lmsg (mUSER, "size=%ld / %ld", dbent->fsize, fsize);
            }
        }
    }
    return dbfound;
}

/**
 @brief suche nach einem Eintrag in der exclude-liste
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param findme was immer gefunden werden soll

 EXIT_SUCCESS wenn der Eintrag gefunden wird

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
___________________________________________________________________________*/

int update_excllst_find(char *findme)
{
    int i;
    // In der exludeliste nach "diesem" Verzeichnis oder File suchen
    if (exlst)                                                                  // Wenn excludeliste vorhanden, dann abbruch beim Auffinden
    {
        for (i=0;exlst[i];i++)
        {
//            if (!strcmp(exlst[i],findme)) return EXIT_SUCCESS;                  // { printf ("ex: %s\n", dirname); return EXIT_SUCCESS; }
            if (strmtch(exlst[i],findme,0)) return EXIT_SUCCESS;

        }
    }
    return EXIT_FAILURE;
}

/**
 @brief Ganze Liste freigeben
 @return Liste, die jetzt NULL sein sollte
 @param Listen Pointer

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
___________________________________________________________________________*/

void *update_Placelst_free(void *data)
{
    void *inside;
    struct place_s *place;
    if (!data) return data;
    for (inside=data;inside;inside=Node_GetNext(inside))
    {
        place=Node_GetData(inside);
        place->location=free0(place->location);
    }
    return Node_DelAll(data);
}

/**
 @brief start the the recode
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param startpoint

 - EXIT_FAILURE, wenn error
 - EXIT_SUCCESS heißt nicht dass leer oder so, sondern nur dass kein Fehler war

 Prüfvorgang

 - einlesen aller subverzeicnisse
 - recursiv erst unterverzeichnisse, bis deep=0
 - nochmal nachschauen, obs doch noch Unterverzeichnisse gibt
 - das Verzeichnis darf auch nicht mehr existieren, nur leer reicht nicht
 - erst dann wird das Verzeichnis und evtl. vorhandene Einträge gelöscht

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
 ** 24.05.25 HS memory_update verhindert unnötiges neu laden von placelst
___________________________________________________________________________*/

// return error(EXIT_FAILURE) / OK-heißt nicht dass leer oder so, sondern nur dass kein Fehler war
int update_dirsDB(struct place_s *place, int deep)
{
    int memory_update_dirsDBdeleted;
    struct place_s *subdir;
    void *placelst; // sql_datalst
    void *inside;   // Node_

//    lmsg (mDEVL, "dirname checking %s (%i)", dirname, deep);
    if (!update_excllst_find(place->location)) return EXIT_SUCCESS;
    lmsg (mDEVL, "location checking %lld %s %s", place->ID, place->location, strstime(place->founded, "%c"));

    update_mover(update_dirsDBcount++);
    // wenn subdirs, dann erst in die subdirs
    if (sql_location_getSubdirs(place->ID, &placelst))    { update_Placelst_free(placelst); return EXIT_FAILURE; }
    if (placelst)
    {
        if (deep==0)
        {
            lmsg (mINFO, "cant walk down deep=0 under: %s", place->location);
            update_Placelst_free(placelst);
            return EXIT_SUCCESS;
        }
        //merke alten "kontostand: deleted"
        memory_update_dirsDBdeleted=update_dirsDBdeleted;
        for (inside=placelst;inside;inside=Node_GetNext(inside))
        {
            subdir=Node_GetData(inside);
            if (update_dirsDB(subdir,deep-1)) { update_Placelst_free(placelst); return EXIT_FAILURE; }
        }
        // wenn nichts gelöscht, dann ist auch kein update nötig
        if (memory_update_dirsDBdeleted!=update_dirsDBdeleted)
        {
            lmsg (mINFO, "location reload placelist %lld %s %s", place->ID, place->location, strstime(place->founded, "%c"));
            placelst=update_Placelst_free(placelst);
            if (sql_location_getSubdirs(place->ID,&placelst))     { update_Placelst_free(placelst); return EXIT_FAILURE; }
        }
        // nicht löschen wenn noch subdirs
        if (placelst)                                       { update_Placelst_free(placelst); return EXIT_SUCCESS; }
    }
    // ++ exists, dann nichts löschen, es sei denn ist jetzt in der excludelist
    if (DirOK(place->location))
    {
//        lmsg (mUSER, "update_dirDB in %s", place->location);
        if (update_excllst_find(place->location)) return EXIT_SUCCESS;  // wenn nicht in ignorelist
        lmsg (mINFO, "%s found in ignore-list, so they have left the the DB", place->location); // und das kann auch gar nicht sein, da es oben bereits rausfliegt
    }
    // ++ entries werden mitgelöscht, wenn das Verzeichnis gelöscht wird
    update_dirsDBdeleted++;
    sql_location_delete(place->ID);
    lmsg (mUSER, "Delete in location: " LLD_STR " (%s)", place->ID, place->location);
    return EXIT_SUCCESS;
}

/**
 @brief  ja, alt bekannt und bewährt

___[ Revision ]______________________________________________________________

 ** 01.01.15 HS Initial
___________________________________________________________________________*/

void update_mover(int shownumber)
{
    static int dots=0;                                                          // Starte mit 0
    static char dotData[] = "/-\\|";                                            // global machen ?
    if (!cmd_isNoScreen)
    {
        dots++;                                                                 // Zähle zum nächsten
        if (dots>4) dots=0;                                                     // zu groß, dann 0
        if (shownumber>=0)  printf ("\r  %i\r%c\r\r", shownumber, dotData[dots]);   // mover + number
        else                printf ("\r%c\r\r", dotData[dots]);                 // mover only Ausgeben
#ifdef OS_LINUX
        fflush(stdout);                                                         // Linux wartet sonst auf CR
#endif
    }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 @brief abarbeitung der filelst und Abgleich mit mit den Daten in der DB
 @return neue filelst, die NULL sein muss
 @param filelst Fileliste die abgearbeitet werden soll
 @param loc_id  Location, die zur filelst passt

 Es wird Liste in den Speicher gesichert die zur DB mit der loc_id passt.

 Dann wird die filelst durchsucht ( immer der erste Eintrag, der
 gelöscht wird wenn der Durchlauf fertig ist.

 Dieser Eintrag wird dann in der DB->Liste(sql_datalst) gesucht. Ist er
 gefunden, dann wird er aus der DB->Liste gelöscht. Am Ende bleiben die
 Einträge übrig für die es keinen passenden Eintrag in der filelst gab.

 Diese werden dann gelöscht

___[ Revision ]______________________________________________________________

 ** 30.04.25 HS Initial
___________________________________________________________________________*/
#ifdef update_delete_need
char **update_delete(char **filelst, sqlid_t loc_id)
{
    // Lösche Daten aus der Datenbank, wenn Sie nicht mehr in Filelist(aktuell vorhandene Files)
    void  *entrie_pnt;
    struct entrie_small_s *entrie_dta;

    sql_datalst=Node_DelAll(sql_datalst);
    if (sql_entrie_list(loc_id)==EXIT_SUCCESS)                                  // Select id,fname from ....
    {
        for (;filelst;filelst=strlstdelete(filelst,0))                          // Solange filelst vorhanden ist
        {
            if (!sql_datalst) break;                                            // Daten aus der Datenbank->sonst fertig
            for (entrie_pnt=sql_datalst;entrie_pnt;entrie_pnt=Node_GetNext(entrie_pnt)) // Alle Daten in der Datenbank finden
            {
                entrie_dta=Node_GetData(entrie_pnt);                            // saved Data
                if (!strcmp(filelst[0],entrie_dta->fname))                      // Eintrag gefunden
                {
                    sql_datalst=Node_DelOne(sql_datalst,entrie_pnt);            // Daten aus der Liste löschen
                    break;
                }
            }
        }
        if (sql_datalst)                                                        // Wenn noch Daten in der Liste übrig, dann sind die weg
        {
            for (entrie_pnt=sql_datalst;entrie_pnt;entrie_pnt=Node_GetNext(entrie_pnt))
            {
                entrie_dta=Node_GetData(entrie_pnt);
                lmsg (mUSER, "delete from entries " LLD_STR " / %s", entrie_dta->ID, entrie_dta->fname);
                sql_entrie_delete(entrie_dta->ID);
            }
        }
    }
    sql_datalst=Node_DelAll(sql_datalst);
    return strlstfree(filelst);
}
#endif

//noz longer in USE
#ifdef update_getcrc32
int update_getcrc32(int loc_id, char *fname, size_t fsize, crc32_t *crc32)
{
    char *local_name;
    *crc32 = 0x0;
    if (fsize>0)
    {
        local_name=sql_location_getDirname(loc_id);
        if (!local_name) return EXIT_FAILURE;
        stradd(local_name,DIR_SEP);
        stradd(local_name,fname);
        if (CRC32File(local_name,crc32))
        {
            lmsg (mFAIL, "update file CRC error %s", local_name);
            free0(local_name);
            return EXIT_FAILURE;
        }
        local_name=free0(local_name);
    }
    return EXIT_SUCCESS;
}
#endif
#ifdef update_file_need
/*
 brief update file, wenn file neu oder (fsize oder fmtime geändert)
 return EXIT_FAILURE / EXIT_SUCCESS
 param loc_id  location ID - kann aus location als Zeichenkette geholt werden
 param fname   filename der updated wird
 param fsize   filesize >0 sonst crc auch 0
 param fmtime  mod time als unixtime
*/
int update_file(int loc_id, char *fname, size_t fsize, time_t fmtime)
{
    sqlid_t entID;
    crc32_t st_crc32;
    struct entrie_s ent={.fsize=0,.fmtime=0,.fcrc32=0};                         // make compiler happy
    entID = sql_entrie_getID(loc_id,fname,&ent);                                // ID aus der Datenbank
    if (entID<0) return EXIT_FAILURE;                                           // kleiner 0 Fehler
    if (entID==0)                                                               // gleich 0, -> neu anlegen
    {
//        lmsg(mDEVL,"INSERT 1");
        //getcrc32
        if (update_getcrc32(loc_id,fname,fsize,&st_crc32))          return EXIT_FAILURE;    // build crc32
//        lmsg(mDEVL,"INSERT 2");
        //sql_insert_entrie(int loc_id, char *fname, size_t fsize, time_t fmtime);
        if (sql_entrie_insert(loc_id,fname,fsize,fmtime,st_crc32)<1)return EXIT_FAILURE;    // insert entry
//        lmsg(mDEVL,"INSERT 3");
        if (sql_entrie_getID(loc_id,fname,&ent)<1)                  return EXIT_FAILURE;    // the new ID
//        lmsg(mDEVL,"INSERT 4");
        lmsg(mUSER,"insert location=" LLD_STR " file=%s", loc_id, fname);
        // maybe inc found++
        return EXIT_SUCCESS;
    }
    // ab hier schon in der datenbank
    if (   (cmd_isRescanFlg==0)                                                 // Kein Rescan ist kein Rescan
        && (fsize==ent.fsize)                                                   // Size +
        && (fmtime==ent.fmtime)                                                 // filetime müssen gleich sein
       ) return EXIT_SUCCESS;                                                   // wenn size und fmtime gleich und kein rescan dann keine neue crc32 nötig
    // update CRC32 / maybe inc updated++
    lmsg(mDEVL,"cmd_isRescanFlg=%i  file.size=%d  db.fsize=%d  %s   file.mtime=%s  db.mtime=%s  %s",
                                cmd_isRescanFlg,
                                              fsize,
                                                           ent.fsize,
                                                               fsize==ent.fsize?"OK":"[not equal]",
                                                                               timeinfostr(fmtime),
                                                                                            timeinfostr(ent.fmtime),
                                                                                                fmtime==ent.fmtime?"OK":"[not equal]"
        );

    if (update_getcrc32(loc_id,fname,fsize,&st_crc32)) return EXIT_FAILURE;     // update CRC für file in loc_id
    if (sql_entrie_update(ent.ID, fsize, fmtime, st_crc32)) return EXIT_FAILURE;// update entrie in DB
    lmsg(mUSER,"update location=" LLD_STR " file=%s", loc_id, fname);
    return EXIT_SUCCESS;
}
#endif
