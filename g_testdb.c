#include "hsguard.h"

//char      *testdb_dir=NULL;
// ??? -t r,m=12,s=1TB,p=/path/in/location,d=2025-04-11

//todo:desc fehlt völlig und fmtime time muss mit geprüft werden
// ** 27.05.25 HS verwendung von ensureDIR
int Execute_TestDB(void)
{
    int       rc;
    int       commapos;
    int       i;
    int       z;
    struct    stat st;
    struct    entrie_s ent;
    struct    place_s  pla;
    char      *local_name;
    crc32_t    activeCRC;
    int        testdb_random=0;
    int        testdb_maxfiles=0;
    uint64_t   testdb_maxsize=0;
    int        testdb_sz;

    if (cmd_isNoScreen) lmsgGuard(0);
    if (!info_path) if(ensureDir(&info_path, basis)) die("test what? use --testDB r,30,10GB will testing max 30Files OR 10GB whatever it comes first");
//    if (!info_path) die("give up; no data to test");
    if (!strncasecmp(info_path,"r,",2))
    {
        testdb_random=1;
        strdel(info_path,0,2);
    }
    if ( (info_path[0]!='/') && (isdigit(info_path[0])) )                       // Erste Zahl ist maxfiles
    {
        commapos=strchrpos(info_path,',',0);                                    // Komma suchen
        if (commapos>=0)
        {
            info_path[commapos]=0;
            if (commapos!=0) testdb_maxfiles=atoi(info_path);                   // kann nicht - sein, das es dann ein Parameter ist
            info_path[commapos]='*';
            strdel(info_path,0,commapos+1);
        }else{
            testdb_maxfiles=atoi(info_path);                                    // Kein Komma, dann ende
            info_path[0]=0;
        }
    }
    if ( (info_path[0]!='/') && (isdigit(info_path[0])) )                       // Zweite Zahl ist maxsize
    {
        commapos=strchrpos(info_path,',',0);
        if (commapos>=0)
        {
            info_path[commapos]=0;
            if (commapos!=0) testdb_maxsize=(uint64_t)Human2Num(info_path);     // uint kann auch nicht minus sein
            info_path[commapos]='*';
            strdel(info_path,0,commapos+1);
        }else{
            testdb_maxsize=(uint64_t)Human2Num(info_path);
            info_path[0]=0;
        }
    }
    if (info_path[0]==0) info_path=free0(info_path);                            // Gleiches nochmal
    if (!info_path) info_path=strdup_ex(basis);                                 // Pfad muss gg. sein
    if (strcmp(basis,"/")) if (strncmp(basis,info_path,strlen(basis))) die ("dir must a part of base or for with -b /");
#ifdef HS_DEBUG
    if (testdb_maxfiles<1) testdb_maxfiles=4;
#else
    if (testdb_maxfiles<1) testdb_maxfiles=100;
#endif
    CorrectPath(info_path);
    if (sql_testdbGetAll(info_path,testdb_maxfiles,testdb_random)) return EXIT_FAILURE;      // sql-Get, ja sehr sql-lastig ...
    testdb_sz=long_table_count();
    if (testdb_sz<=0) return EXIT_SUCCESS;
//    lmsg(mDEVL,"maxsize =" LLU_STR "",testdb_maxsize);
    lmsg(mUSER,"maxfiles=%i (%i) maxsize = %s path =%s" , testdb_maxfiles, testdb_sz, Num2Human((double)testdb_maxsize,3) , info_path);
    local_name=NULL;
    rc=EXIT_SUCCESS;
    for (i=0;i<testdb_sz;i++)
    {
        ZeroMemory(&ent,sizeof(struct entrie_s));
        ZeroMemory(&pla,sizeof(struct place_s));
        if (sql_entrie_getByID(long_table_value(i),&ent)<1) return EXIT_FAILURE;
        if (sql_location_getByID(ent.loc_id,&pla)<1)        return EXIT_FAILURE;

        lmsg (mINFO, "check ID=" LLD_STR " Name=%s" DIR_SEP "%s", ent.ID, pla.location, ent.fname );

        local_name=strprintf("%s" DIR_SEP "%s", pla.location, ent.fname );
        if (   (lstat (local_name, &st))                                        // status Lesen und
             ||(CRC32File(local_name,&activeCRC))                               // crc32 neu berechnen
           )
        {
            // error-fall also fehler bei lstat oder fehler bei CRC32File
            lmsg (mFAIL, "CRC/lstat error: %s", local_name);
            free0(local_name);
            return EXIT_FAILURE;
        }
        local_name=free0(local_name);
        if ((st.st_size!=ent.fsize)||(activeCRC!=ent.fcrc32)||(st.st_mtim.tv_sec!=ent.fmtime))
        {

// lisstformat=kompakt
//          crc32 DB/File size DB/File mTime DB/File ** File Location
//          0x12345678  0x12345678  1.234.456.789 1.234.456.789  2025-12-06 15:33:12 2021-04-22 11:22:54  Weihnachten.jpg    /bilder/familie
// lisstformat=detail
// Weihnachten.jpg    /bilder/familie
//          0x12345678          0x12345678
//       1.234.456.789       1.234.456.789
// 2025-12-06 15:33:12 2021-04-22 11:22:54
//ausgabe detailieren fmtime lastchk beim ersten mal incl emergency options
            lmsg(mFAIL,"size / crc / fmtime geändert !! ID=" LLD_STR " NAME=%s" DIR_SEP "%s", ent.ID, pla.location, ent.fname );
            lmsg(mINFO,"file.size=%d  db.fsize=%d  %s   file.mtime=%s  db.mtime=%s  %s CRC=0x%08x  db.crc=0x%08x  %s",
                                  st.st_size,
                                               ent.fsize,
                                                   st.st_size==ent.fsize?"OK":"[not equal]",
                                                                   timeinfostr(st.st_mtim.tv_sec),
                                                                                timeinfostr(ent.fmtime),
                                                                                    st.st_mtim.tv_sec==ent.fmtime?"OK":"[not equal]",
                                                                                              activeCRC,
                                                                                                            ent.fcrc32,
                                                                                                                  activeCRC==ent.fcrc32?"OK":"[not equal]"
            );
            // Sofortiger Programmabbruch
            if (emergency&emPANIC) exit (cleanup(99));
            if (!(emergency&(emABORT|emSKIP)))
            {
                lmsg(mFAIL,"PANIC | ABORT | SKIP must set for emergency; never happens i think");
                return EXIT_FAILURE;
            }
            // Wenn Update, dann neue Werter eintragen (updatefull)
            if (emergency&emUPDATE) if (sql_entrie_update(ent.ID, st.st_size, st.st_mtim.tv_sec, activeCRC )) return EXIT_FAILURE;
            // Wenn Update, dann nur lstcheck updaten
            if (emergency&emUPLCHK) if (sql_entrie_update(ent.ID, ent.fsize , ent.fmtime       , ent.fcrc32)) return EXIT_FAILURE;
            if (emergency&emABORT) return abortlevel;
            // An dieser Stelle ist alles abgearbeitet bis auf SKIP, wenn der nicht gesetz ist, dann *peng*
            if (!(emergency&emSKIP)) exit (cleanup(1));
            z = emergency & (emMIN-1);
            // count überprüfen 0=nicht prüfen 1-Anzahl
            lmsg(mDEVL,"SKIP : COUNT=%i",z);
            if (z>0)
            {
                z--;
                if (z<=0) return EXIT_SUCCESS;
                emergency = (emergency & ~(emMIN-1)) | z ;        // untere Bits ausblenden z-1 wieder einpflegen
            }
        }else{
            if (sql_entrie_update(ent.ID, ent.fsize, ent.fmtime, ent.fcrc32)) return EXIT_FAILURE;
        }
        testdb_maxfiles--;
        if (testdb_maxsize>0)
        {
            if (testdb_maxsize<=st.st_size)
            {
                testdb_maxsize=0;
                break;
            }
            testdb_maxsize=testdb_maxsize-st.st_size;
        }
    }
    lmsg(mDEVL,"maxfiles=%i  (%i)", testdb_maxfiles, testdb_sz);
    lmsg(mDEVL,"maxsize =" LLU_STR "",testdb_maxsize);
    lmsg(mDEVL,"path    =%s",info_path);
    return rc;
}
