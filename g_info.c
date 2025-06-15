#include "hsguard.h"

/**
 @brief  HighLevel Code um --list zu starten
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 07.05.25 HS Initial
 ** 27.05.25 HS verwendung von ensureDIR
___________________________________________________________________________*/

int Execute_info(void)
{

    if (cmd_isNoScreen) lmsgGuard(0);
    if (!info_path) if (ensureDir(&info_path, basis)) return EXIT_FAILURE;
    return list_info_printByArg(info_path);
}

/**
 @brief  Worker um Einträge anzuzeigen
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  listdata path oder "@sonderfall"

 - "--info"     zeigt einträge an. ist listdata ein verzeichnis dann als listdir
 - "--infodir"  zeigt nur verzeichnisse an

 path kann eine Datei oder Verzeichnis sein.

 ist das erste Zeichen ein '@', dann ist es ein  Sonderfall. bei '@0x' wird
 nach einer Prüfsumme gesucht. Eine Prüfsumme kann nur für eine entrie-id
 gefunden werden.

 Andernfalls wird die ID angenommen. mit
 '--infodir' wird eine place-id angenommen. Andernfall wird eine Info für eine
 entrie-id angenommen.

___[ Revision ]______________________________________________________________

 ** 07.05.25 HS Initial
___________________________________________________________________________*/

int list_info_printByArg(char *listdata)
{
    int rc;
    sqlid_t local_id;
    struct entrie_s ent;
    struct place_s  pla;
    void *lst;
    struct entrie_small_s *esmall;
//    lmsg (mROOT, "%s hallo", m_PRGNAME);
//    return EXIT_FAILURE;
    ZeroMemory(&ent,sizeof(ent));
    ZeroMemory(&pla,sizeof(pla));

    if (cmd_isFormOut<FO_MIN)  cmd_isFormOut=FO_INFO_DEFAULT;
    if (cmd_isFormOut>=FO_MAX) cmd_isFormOut=FO_FULL;

    local_id=0;
    if (!listdata) return EXIT_FAILURE;
    if (listdata[0]!='@')
    {
        if (listdata[0]!='/')
        {
            if (sql_entrie_listLikeFname(listdata)) return EXIT_FAILURE;
            if (!sql_datalst)
            {
                char *blist;
                blist=strprintf("%s/%s",basis,listdata);
                if (sql_location_getDir(blist,&pla)>=1)
                {
                    free(blist);
                    return list_print_pla(&pla);
                }
                free(blist);
                lmsg (mFAIL, "no entrys like %s", listdata);
                return EXIT_FAILURE;
            }
            for (lst=sql_datalst;lst;lst=Node_GetNext(lst))
            {
                esmall=Node_GetData(lst);
                if (sql_entrie_getByID(esmall->ID,&ent)<1)   { sql_datalst=Node_DelAll(sql_datalst); return EXIT_FAILURE; }
                if (sql_location_getByID(ent.loc_id,&pla)<1) { sql_datalst=Node_DelAll(sql_datalst); return EXIT_FAILURE; }
                list_print_ent(&ent,&pla,-1);
            }
            rc=Node_Count(sql_datalst);
            if (rc>1) lmsg(mUSER,"%ld entries found", rc);
            sql_datalst=Node_DelAll(sql_datalst);
            return EXIT_SUCCESS;
        }

        if (cmd_isInfo&&!FileOK(listdata)&&DirOK(listdata)) {cmd_isInfo=0;cmd_isInfoDir=1;}
        if (cmd_isInfo)
        {
            rc=sql_location_getDir(Cdirname(listdata),&pla);
            if (rc<0) return EXIT_FAILURE;
            if (rc<1) { lmsg (mFAIL, "dir not found: %s", listdata); return EXIT_FAILURE; }
            rc=sql_entrie_getID(pla.ID,Cbasename(listdata),&ent);
            if (rc<0) return EXIT_FAILURE;
            if (rc<1) { lmsg (mFAIL, "file not found: %s", listdata); return EXIT_FAILURE; }
//----------
            lmsg (mINFO,"d='%s' f='%s'", pla.location,ent.fname);
            // ent & pla gefuellt
        }else if (cmd_isInfoDir){
            rc=sql_location_getDir(listdata,&pla);
            if (rc<0) return EXIT_FAILURE;
            if (rc<1) { lmsg (mFAIL, "dir not found: %s", listdata); return EXIT_FAILURE; }
            lmsg (mINFO,"loc='%s'", pla.location);
            // pla gefuellt ent leer
        }else{
            lmsg(mFAIL, "you can not reach this");
            return EXIT_FAILURE;
        }
    }else{
        if (!strncmp(listdata,"@0x",3))
        {
            if (sql_entrie_listByCRC((crc32_t)hextol(&listdata[3]))) return EXIT_FAILURE;
            // ergebnis ist in sql_datalst
            if (!sql_datalst) { lmsg (mFAIL, "no entrys for %s", listdata); return EXIT_FAILURE; }
            for (lst=sql_datalst;lst;lst=Node_GetNext(lst))
            {
                esmall=Node_GetData(lst);
                if (sql_entrie_getByID(esmall->ID,&ent)<1)   { sql_datalst=Node_DelAll(sql_datalst); return EXIT_FAILURE; }
                if (sql_location_getByID(ent.loc_id,&pla)<1) { sql_datalst=Node_DelAll(sql_datalst); return EXIT_FAILURE; }
                list_print_ent(&ent,&pla,-1);
            }
            rc=Node_Count(sql_datalst);
            if (rc>1) lmsg(mUSER,"%ld entries found", rc);
            sql_datalst=Node_DelAll(sql_datalst);
            return EXIT_SUCCESS;
        }else{
            local_id=atoll(&listdata[1]);
            if (local_id<1)
            {
                lmsg(mFAIL, "@id means an exists value. not: %s", listdata);
                return EXIT_FAILURE;
            }
            if (cmd_isInfo)
            {
                if (sql_entrie_getByID(local_id,&ent)<1)
                {
                    lmsg(mFAIL, "the id %s cant find", listdata);
                    return EXIT_FAILURE;
                }
                rc=sql_location_getByID(ent.loc_id, &pla);
                if (rc<0) return EXIT_FAILURE;
                if (rc<1) { lmsg (mFAIL, "dir not found: %s", listdata); return EXIT_FAILURE; }
            }else if (cmd_isInfoDir){
                rc=sql_location_getByID(local_id, &pla);
                if (rc<0) return EXIT_FAILURE;
                if (rc<1) { lmsg (mFAIL, "dir not found: %s", listdata); return EXIT_FAILURE; }
            }else{
                lmsg(mFAIL, "you can not reach this");
                return EXIT_FAILURE;
            }
        }
    }
    if (ent.ID>0)       list_print_ent(&ent,&pla,-1);//printf("ent=%lld\n",ent.ID); //show entrie &ent
    else if (pla.ID>0)  list_print_pla(&pla);//printf("pla=%lld\n",pla.ID); //show location &pla
    return EXIT_SUCCESS;
}

/**
 @brief  Anzeigen ein Eintrags der in struct @ref place_s pla beschrieben ist
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  pla struct @ref place_s

___[ Revision ]______________________________________________________________

 ** 07.05.25 HS Initial
___________________________________________________________________________*/

int list_print_pla(struct place_s *pla)
{
    int sz=9;
    if (!pla) return EXIT_FAILURE;    //col_ID col_location col_found
    printf ("%s%-*s= " LLD_STR "%s\n" , color_get_ansi(col_ID)       ,sz, "ID"      ,pla->ID                       , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_Location) ,sz, "location",pla->location                 , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_Founded)  ,sz, "founded" ,gmtinfostr(pla->founded)      , color_ansi_str(COLOR_RESET));
    return EXIT_SUCCESS;
}

/**
 @brief  Anzeigen eines Eintrags der in pla/ent pla beschrieben ist
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  ent struct @ref entrie_s
 @param  pla struct @ref place_s

___[ Revision ]______________________________________________________________

 ** 07.05.25 HS Initial
___________________________________________________________________________*/

int list_print_ent(struct entrie_s *ent, struct place_s *pla, int fname_maxsize)
{
    int p;
    int sz=9;
    if (!ent) return EXIT_FAILURE;

    p=0;
    if (pla)
    {
        if (pla->ID>0) if (pla->ID==ent->loc_id) p=1;
        if (!p) list_print_pla(pla);
    }

    if ((cmd_isFormOut==FO_ONELINE)||(cmd_isFormOut==FO_LONGLINE)||(cmd_isFormOut==FO_MAXLINE))
    {
        printf ("%s%s"    , color_get_ansi(col_ID)      , HSLnum2Str(ent->ID,' ',0,5));
        printf ("%s %s"   , color_get_ansi(col_locID)   , HSLnum2Str(ent->loc_id,' ',0,5));
        printf ("%s %-*s" , color_get_ansi(col_Fname)   , (fname_maxsize>0)?fname_maxsize:40, ent->fname);
        if ((cmd_isFormOut==FO_LONGLINE)||(cmd_isFormOut==FO_MAXLINE))
        {
            printf ("%s 0x%08x", color_get_ansi(col_CRC32)   , (unsigned int)ent->fcrc32);
            printf ("%s %7s"   , color_get_ansi(col_Fsize)   , Num2Human(ent->fsize, 0));
            printf ("%s %s"    , color_get_ansi(col_Fmtime)  , timeinfostr(ent->fmtime));
        }
        if (cmd_isFormOut==FO_MAXLINE)
        {
                printf ("%s %3ld"  , color_get_ansi(col_CheckCount) , (unsigned long)ent->checkcnt);
                printf ("%s %s"    , color_get_ansi(col_LastChk) , gmtinfostr(ent->lastchk));
                printf ("%s %s"    , color_get_ansi(col_Founded) , gmtinfostr(ent->founded));

        }
        printf ("%s\n", color_ansi_str(COLOR_RESET));
        return EXIT_SUCCESS;
    }

    printf ("%s%-*s= " LLD_STR "%s\n" , color_get_ansi(col_ID) ,sz, "ID" ,ent->ID                       , color_ansi_str(COLOR_RESET));
    if (p)
    {
        printf ("%s%-*s= (" LLD_STR ") %s %s%s%s\n" , color_get_ansi(col_Location)  ,sz, "location"  ,ent->loc_id, pla->location,
                                                      color_get_ansi(col_locFounded),gmtinfostr(pla->founded), color_ansi_str(COLOR_RESET));
    }else{
        printf ("%s%-*s= " LLD_STR "%s\n" , color_get_ansi(col_locID)  ,sz, "loc_id"  ,ent->loc_id , color_ansi_str(COLOR_RESET));
    }
    if (cmd_isFormOut==FO_MINI) return EXIT_SUCCESS;
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_Fname)      ,sz, "fname"   ,ent->fname                    , color_ansi_str(COLOR_RESET));
    if (cmd_isFormOut==FO_BASIC) return EXIT_SUCCESS;
    printf ("%s%-*s= %ld"      "%s\n" , color_get_ansi(col_Fsize)      ,sz, "fsize"   ,ent->fsize                    , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_Fmtime)     ,sz, "fmtime"  ,timeinfostr(ent->fmtime)      , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= 0x%08lx"   "%s\n", color_get_ansi(col_CRC32)      ,sz, "crc32"   ,ent->fcrc32                   , color_ansi_str(COLOR_RESET));
    if (cmd_isFormOut==FO_MEDIUM) return EXIT_SUCCESS;
    printf ("%s%-*s= %ld"      "%s\n" , color_get_ansi(col_CheckCount) ,sz, "chkcount",ent->checkcnt                 , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_LastChk)    ,sz, "lastchk" ,gmtinfostr(ent->lastchk)     , color_ansi_str(COLOR_RESET));
    printf ("%s%-*s= %s"       "%s\n" , color_get_ansi(col_Founded)    ,sz, "founded" ,gmtinfostr(ent->founded)     , color_ansi_str(COLOR_RESET));
    return EXIT_SUCCESS;
}
