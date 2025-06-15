#include "hsguard.h"

/**
 @brief  HighLevel Code um --remove zu auszuführen
 @return EXIT_FAILURE / EXIT_SUCCESS

 Ruft @ref remove_dirsDB am Ende auf. Hier wird noch nachgefragt ob
 man das wirklich möchte

___[ Revision ]______________________________________________________________

 ** 11.05.25 HS Initial
___________________________________________________________________________*/

int Execute_remove(void)
{
    int rc,filerc;
    struct entrie_s ent;
    struct place_s pla;

    if (cmd_isNoScreen) lmsgGuard(0);
    if (!info_path) die ("you must enter path(from file) or fullpath of the dir");
    rc = getPlaceFrom_info_path(&pla);
    lmsg(mUSER,"remove (%i) %s", rc, info_path);
    if (rc==0)
    {
        // where is the file (places)
        filerc=sql_location_getDir(Cdirname(info_path),&pla);
        if (filerc<=0)
        {
            lmsg(mFAIL, "fail to find %s", info_path);
            return EXIT_FAILURE;
        }
        // where is the file (location)
        filerc=sql_entrie_getID(pla.ID, Cbasename(info_path), &ent);
        if (filerc<=0)
        {
            lmsg(mFAIL,"fail to find %s in %s", Cbasename(info_path), pla.location);
            return EXIT_FAILURE;
        }
        // found file in DB, let them destroy
        if (!cmd_isYesFlg)
        {
            list_print_ent(&ent,&pla,-1);
            printf ("Remove File %s/%s from DB (Y/N)", pla.location, ent.fname);
            filerc=getc(stdin);
            // or dont
            if (tolower(filerc)!='y') die ("break here");
        }
        sql_entrie_delete(ent.ID);
        lmsg(mUSER, "removed entID " LLD_STR "", ent.ID);
        return EXIT_SUCCESS;
    }
    //pla in pla.location all and subs will delete, this may heavy data
    if (!cmd_isYesFlg)
    {
        list_print_pla(&pla);
        printf ("Remove All and everything from DB starting %s (Y/N)", pla.location);
        filerc=getc(stdin);
        if (tolower(filerc)!='y') die ("break here");
    }
    rc = remove_dirsDB(pla.location);
    if (rc) lmsg(mFAIL, "Error while removed plaID " LLD_STR " %s", pla.ID, pla.location);
    else    lmsg(mUSER, "removed plaID " LLD_STR " %s", pla.ID, pla.location);

    return rc;
}

/**
 @brief  möglicher recursiver Aufruf um Verzeichnisse aus der DB zu löschen
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  dirname verzeichnis das gelöscht werden soll

___[ Revision ]______________________________________________________________

 ** 11.05.25 HS Initial
___________________________________________________________________________*/

int remove_dirsDB(char *dirname)
{
    int rc;
    struct place_s place;
    struct place_s *subdir;
    void *placelst;
    void *inside;

    lmsg (mINFO, "start remove incl. SUB %s", dirname);
    if (sql_location_getDir(dirname, &place)<1) return EXIT_FAILURE;
    if (sql_location_getSubdirs(place.ID, &placelst))    { update_Placelst_free(placelst); return EXIT_FAILURE; }
    // wenn subdirs, dann erst in die subdirs
    if (placelst)
    {
        for (inside=placelst;inside;inside=Node_GetNext(inside))
        {
            subdir=Node_GetData(inside);
//            if (remove_dirsDB(subdir->location))        { update_Placelst_free(placelst); return EXIT_FAILURE; }
            rc=remove_dirsDB(subdir->location);
            if (rc)
            {
                update_Placelst_free(placelst);
                return rc;
            }
        }
        placelst=update_Placelst_free(placelst);
    }
    // nicht löschen wenn subdirs, die gerade gelöscht waren .. sollte so nicht vorkommen
    if (sql_location_getSubdirs(place.ID,&placelst))     { update_Placelst_free(placelst); return EXIT_FAILURE; }
    if (placelst)                                       { update_Placelst_free(placelst); return EXIT_FAILURE; }
    rc=sql_location_delete(place.ID);
    lmsg (mUSER, "Final delete of " LLD_STR " (%s)", place.ID, place.location);
    return rc;
}
