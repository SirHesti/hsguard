#include "hsguard.h"

/**
 @brief  HighLevel Code um --finddupes zu starten
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
 ** 27.05.25 HS verwendung von ensureDIR
___________________________________________________________________________*/

int Execute_FindDupes(void)
{
    int rc;
    struct place_s pla;
    if (cmd_isNoScreen) lmsgGuard(0);

    if (cmd_isFormOut<FO_MIN)  cmd_isFormOut=FO_FINDDUPES_DEFAULT;
    if (cmd_isFormOut>=FO_MAX) cmd_isFormOut=FO_FULL;
    if (ensureDir(&info_path,basis)) die ("dir not given or else wrong");
    if (info_path) CorrectPath(info_path);
    rc=getPlaceFrom_info_path(&pla);
    if (rc<1)
    {
        lmsg(mFAIL,"No Subdir found in DB: %s", strNotNULL(info_path));
        die (NULL);
    }
    lmsg (mUSER, "start finddupes in %s", pla.location);
    long_table_free();                      //null findlist
    sql_location_index_drop();
    sql_location_index_create();

    rc=finddupes_ByDir(pla.location);

    sql_location_index_drop();
    long_table_free();                      //null findlist
    if (rc) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

/**
 @brief  Finde doppelte Files in der Datenbank
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  dirname startpoint

 Es wird grundsätzlich das aktuelle Verzeichnis und aller unterhalt durchsucht

 Dabei wird jeder(!) Eintrag unterhalb von dirname mit jedem(!) in der
 gesammten Datenbank verglichen. Es müssen fize&cr32 übereinstimmen. Name
 und modtime können variieren. Size und CRC32 sind fest.

 Nicht gefunden werden Dateien mit einer size von 0. Die CRC32 ist da immer 0
 und führt zu nicht gewollten Ausgaben.

___[ Revision ]______________________________________________________________

 ** 13.05.25 HS Initial
___________________________________________________________________________*/

int finddupes_ByDir(char *dirname)
{
    struct place_s pla;
    struct place_s epla;
    void *placelst;
    void *inside;
    void *insdup;
    struct place_s  *subdir;
    struct entrie_s *subent;
    struct entrie_s *dupent;

    lmsg (mINFO, "finddupes at %s", dirname);

    //todo: DUPES    if dirname in finddupes-exclude then return;

    if (sql_location_getDir(dirname, &pla)<1)           return EXIT_FAILURE;
    if (sql_location_getSubdirs(pla.ID, &placelst))     { update_Placelst_free(placelst); return EXIT_FAILURE; }
    if (placelst)
    {
        for (inside=placelst;inside;inside=Node_GetNext(inside))
        {
            subdir=Node_GetData(inside);
            if (finddupes_ByDir(subdir->location))      { update_Placelst_free(placelst); return EXIT_FAILURE; }
        }
        placelst=update_Placelst_free(placelst);
    }
    if (sql_entrie_getFullList(pla.ID)) return EXIT_FAILURE;
    if (!sql_datalst) return EXIT_SUCCESS;                      // Keine Einträge

    placelst=get_sql_datalst();
    for (inside=placelst;inside;inside=Node_GetNext(inside))
    {
        subent=Node_GetData(inside);
        if (subent->fsize==(size_t)0) continue;
        //select * from entrie where loc_id!=23 and fsize=123 and fcrc32=12347542
        if (sql_entrie_getFullbyCrcSizeNotID(subent->ID, subent->fsize, subent->fcrc32))
        {  // in error condition
            sql_datalst=Node_DelAll(sql_datalst);
            placelst=Node_DelAll(placelst);
            return EXIT_FAILURE;
        }
        if (!sql_datalst) continue;                             // Nichts gefunden, was doppelt ist (Der Normalfall)
        if (long_table_bsearch(subent->ID)<0)                   // Wenn noch nicht dargestellt, dann erledige das
        {                                                       // Das war der der in diesem Verzeichnis (dirname) ist
            long_table_insert(subent->ID);
            list_print_ent(subent,&pla,-1);
        }
        for (insdup=sql_datalst;insdup;insdup=Node_GetNext(insdup)) // jetzt alle Eintraege listen, die die gleich CRC haben
        {
            dupent=Node_GetData(insdup);
            if (long_table_bsearch(dupent->ID)<0)               // Jeder Eintrag soll nur einmal gelistet werden
            {
                long_table_insert(dupent->ID);                  // es wird angenommen das das funktioniert
                if (sql_location_getByID(dupent->loc_id,&epla)==1) // Suchen einen Eintrag in der Verzeichnisliste (places)
                {                                               // Auch hier sollte epla gefunden werden.
                    list_print_ent(dupent,&epla,-1);
                }else{                                          // mit NULL ist ein Sonderfall, dann ist places nicht mehr vollständig
                    list_print_ent(dupent,NULL,-1);                // vielleicht einen Sonderfall im Log vermerken ?
                }
            }
        }
        sql_datalst=Node_DelAll(sql_datalst);
//        printf ("id=%lld name=%s\n",subent->ID,subent->fname);
    }
    placelst=Node_DelAll(placelst);
    return EXIT_SUCCESS;
}
