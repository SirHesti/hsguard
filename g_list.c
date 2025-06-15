#include "hsguard.h"

/**
 @brief  HighLevel Code um --list zu starten
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
 ** 27.05.25 HS verwendung von ensureDIR
___________________________________________________________________________*/

int Execute_list(void)
{
    if (ensureDir(&list_dir,basis)) return EXIT_FAILURE;
//    lmsg(mROOT,"list cmd started at [%i] (%i) %s", cmd_isNoScreen, cmd_isRecursive, strNotNULL(list_dir));
    if (cmd_isNoScreen) lmsgGuard(0);
    lmsg(mINFO,"list cmd started at [%i] (%i) %s", cmd_isNoScreen, cmd_isRecursive, strNotNULL(list_dir));
    list_liste(list_dir, cmd_isRecursive);
    lmsg(mINFO,"list cmd done");
    return EXIT_SUCCESS;
}

/**
 @brief  Zeige eine Liste ab dirname
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  dirname ab hier anzeigen
 @param  deep, wie tief soll gelistet werden -r=2 kann das lösen

___[ Revision ]______________________________________________________________

 ** 04.05.25 HS Initial
___________________________________________________________________________*/

int list_liste (char *dirname, int deep)
{
    void *lst;
    void *placelst;
    struct entrie_s *E;
    struct place_s *P;
    sqlid_t loc_id;
    int maxlen;

    if (cmd_isFormOut<FO_MIN)  cmd_isFormOut=FO_LIST_DEFAULT;
    if (cmd_isFormOut>=FO_MAX) cmd_isFormOut=FO_FULL;

    if (!update_excllst_find(dirname)) return EXIT_SUCCESS;
    loc_id=update_getdirnum(dirname,F_CREATE_CACHED);
    lmsg(mDEVL, "loc_id %s => " LLD_STR, dirname, loc_id );
    if (loc_id<1)
    {
        lmsg(mFAIL, "dir not in database: %s", dirname);
        return EXIT_FAILURE;
    }
    if (sql_location_getSubdirs(loc_id, &placelst))
    {
        lmsg(mFAIL, "failure to get placelist from: %s", dirname);
        update_Placelst_free(placelst);
        return EXIT_FAILURE;
    }
//    if (cmd_isCaption) printf ("Contens of %s\n", dirname);
    if (cmd_isListAll)
    {
        sql_stored=0;
        if (sql_entrie_getFullList(loc_id)) return EXIT_FAILURE;
        if (sql_datalst)
        {
            maxlen=20;
            for (lst=sql_datalst;lst;lst=Node_GetNext(lst))
            {
                E=(struct entrie_s *) Node_GetData(lst);
                maxlen=max(maxlen,strlen(E->fname)+2);
            }
//            if (cmd_isCaption)
//            {
//                if (cmd_isFullsize>=0) printf ("ID    locid %-*s  CRC32       FSize MTime", maxlen,"Name");
//                if (cmd_isFullsize>0)  printf ("             ChkCount LastChk          FoundDate");
//                printf ("\n");
//            }
            for (lst=sql_datalst;lst;lst=Node_GetNext(lst))
            {
                E=(struct entrie_s *) Node_GetData(lst);
                list_print_ent(E,NULL, maxlen);
            }
            sql_datalst=Node_DelAll(sql_datalst);
        }
    }
    if (placelst)
    {
        maxlen=50;
        for (lst=placelst;lst;lst=Node_GetNext(lst))
        {
            P=(struct place_s*)Node_GetData(lst);
            maxlen=max(maxlen,strlen(P->location)+2);
        }
//        if (cmd_isCaption) printf ("ID    %-*s FoundDate\n", maxlen,"location");
        for (lst=placelst;lst;lst=Node_GetNext(lst))
        {
            P=(struct place_s*)Node_GetData(lst);
            printf ("%s%5lld "  , color_get_ansi(col_locID)      , P->ID);
            printf ("%s%-*s "   , color_get_ansi(col_Location)   , maxlen, P->location);
            printf ("%s%s"      , color_get_ansi(col_locFounded) , gmtinfostr(P->founded));
            printf ("%s\n"      , color_ansi_str(COLOR_RESET));
            if (deep!=0) list_liste(P->location,deep-1);
        }
    }
    update_Placelst_free(placelst);
    return EXIT_SUCCESS;
}
