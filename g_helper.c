#include "hsguard.h"

/**
 @brief hilfeseite
 @return EXIT_SUCCESS

 Tabelle mit doppelstring wird interpretiert und Breite selbst ermittelt

 @ zum Export der hilfe im source nicht ändern
 @ //--[ HELP ]--
 @ //--[ EOF HELP ]--

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
___________________________________________________________________________*/

typedef struct{
    char *s1;
    char *s2;
    char *s3;
    char *s4;
    char *s5;
    char *s6;
    char *s7;
    char *s8;
}dbstr_t;

//--[ HELP ]--
dbstr_t helpstr[]={
    { " ",                       "All cmdline options overwrite an option in hsguard.rc" },
    { "--help -? -h",            "Print this helpmsg" },

    { "-d --db sqlite3.db",      "use this \"[PATH/]sqlite3.db\" database **overwrite: [RCdatabase](#RC-DATABASE)**" },
    { "-b --base <dir>",         "basedir for all operations **overwrite [RCbase](#RC-BASE)**" },
    // tutorial
    { "   --create",             "create an empty ConfigFile highcommand to edit this" },
    { "   --initdb",             "create an empty Database" },
    { "   --createExcl",         "create an example excludefile" },
    //todo: -p --section --config ??
    { "-x --exclude  <file>",    "read a list of excluded files -- '#' comment ## **add this list: [RCexclude](#RC-EXCLUDE)**" },
    { "   --exclude_add <file>", "add a single file/dir to excludelist **works too: [RCexadd](#RC-EXADD)**" },
    { "   --ldir/--edir <dir>",  "an LS for exclude file writers (NO Function on DB)" },
    { "   --update_deep <num>",  "update database only this deep (update will set)" },
    { "   --update_dir <dir>",   "update from here into database (update will set)" },
    { "-u --update",             "update database" },
    { "   --rescan",             "update all, but reread crc32" },
    { "-n --nodirtest",          "update will _not_ scan for empty dirs otherwise no exists",
                                 "dirs(and entries) will remove from database" },
    { "   --yes",                "don't ask me; i answer yes" },
    { "-v --verbose",            "use   -v=1 to silence, also use -v- ** overwrite [RCverbose](#RC-VERBOSE)**",
                                 "      -v=2 erros(default) -v FAIL is the same",
                                 "      -v=3 (same -v) userinfo",
                                 "      -vv  -v=4 info",
                                 "      -vvv -v=5 developer"},
    { "-s --noscreen",           "In case of UserCMD's like '--list' make screen for output.","all UserMsg are saved only in the log. to turn off noscreen use -s- **overwrite [RCnoscreen](#RC-NOSCREEN)**"},
    { "-a --ansi on/off/-/+",    "Ansicolor at find/info/list -a- off -a=On or only -a is on","colors can be tuned in the config file **overwrite: [RCansicolor](#RC-ANSICOLOR)**"},
    { "-l --list",               "print out database contens"},
    { "   --listdir",            "print only dirs from database see: --recursive"},
    { "-f --formout",            "Format Out to UserLevel",
                                 "Singleline :ONELINE, LONGLINE, MAXLINE",
                                 "Multiline: :MINI, BASIC, MEDIUM, ALL"},
    { "-r --recursive",          "list subdirs too _-r=deep_ can go deeper _'-r'_ lists everything"},
    { "-i --info <dir>or<path>or@<id>","Info about file in path <entrie> in database","path like /name/too","path without starting '/' will find with '*' case intensive"},
    { "   --find",               "same as info, but finddir is not implemented"},
    { "   --infodir <dir>or@<id>","Info about file in dir <places> in database"},
    { "   --remove  <dir>or<path>","remove entrie or a complete path(include entries/subdirs) from DB","you must confirmed (Y/N) (or use --yes)"},
    { "   --finddupes <dir>",    "find dupes beginn at <dir> or start in base. This can take a while"},
    { "-t --testdb [OPTIONS]",   "_OPTIONS_ are [r],count,[size],[path], where...",
                                 "_[r]_    optional will activate the random Mode *",
                                 "_count_  of Files that will checked",
                                 "_[size]_ optional size that will never reached. You can use 10TB or 500k",
                                 "_[path]_ optional, if not set then start check from BASE",
                                 "* if no random activated, then oldest (from lastchk) will checked" },
    { "-e --emergency [OPTIONS]","PANIC,ABORT=<errorlevel>,SKIP,COUNT=<num>,UPDATE,UPDATEALL **overwrite: [RCemergency](#RC-EMERGENCY)**",
                                 "_PANIC_        : Abort all, use Errorlevel 99 to exit",
                                 "_ABORT_[=<el>] : Abort with Errorlevel 1, if <el>(2-98) is set, then el is use",
                                 "_SKIP_         : Skip this to COUNT, then abort",
                                 "_COUNT_=<num>  : Skip not forever, only for this <num>",
                                 "_UPDATE_       : Update lastchk and chkcount in DB",
                                 "_UPDATEALL_    : Update size,fmtime,crc32 as found too" },
    { "--wbs <size>",            "Warning big size Files default is 16GB **overwrite: [RCwarnbigsize](#RC-WBS)**"},
    {NULL} //,NULL,NULL,NULL,NULL,NULL,NULL
};
//--[ EOF HELP ]--

int showHelp(void)
{
    int i,k;
    int w;
    int len;
    char *t;
    char *p;
    w=20;
    for (i=0;helpstr[i].s1;i++)
    {
        len=strlen(helpstr[i].s1);
        w=max(w,len);
    }
    w+=2;
    for (i=0;helpstr[i].s1;i++)
    {
        for (k=2;k<=8;k++)
        {
            switch (k)
            {
                case 2: t=strdup_ex(helpstr[i].s2); break;
                case 3: t=strdup_ex(helpstr[i].s3); break;
                case 4: t=strdup_ex(helpstr[i].s4); break;
                case 5: t=strdup_ex(helpstr[i].s5); break;
                case 6: t=strdup_ex(helpstr[i].s6); break;
                case 7: t=strdup_ex(helpstr[i].s7); break;
                case 8: t=strdup_ex(helpstr[i].s8); break;
                default:t=NULL;          break;
            }
            if (!t) break;
            if (k==2) printf ("%-*s ",w,helpstr[i].s1);
            else      printf ("%-*s ",w,"");
            p=strstr(t," **");
            if (p) *p='\0';
            printf ("%s\n",t);
            free(t);
        }
    }
//    printf("%i\n",w);
    return EXIT_SUCCESS;
}

/**
 @brief Kommandoauswertung
 @return EXIT_FAILURE / EXIT_SUCCESS

 switches/Parameter werden von Kommandozeile gelesen und gespeichert

___[ Revision ]______________________________________________________________

 ** 28.04.25 HS Initial
 ** 01.05.25 HS Moved to helper
___________________________________________________________________________*/

//kann zum direkten ausführen und ProgrammEnde führen
int GetCmdlineSwitches(void) // @     g  jk m   q     w y
{                            //
//    int i;
//  make defaults
    char *str;
    if (m_PRG_argc < 2 ) return EXIT_SUCCESS;
    InitARG(m_PRG_argc);

    if (aChkARGlong("create"))      ;
    if (aChkARGlong("config"))      ;
    if (aChkARGlong("section"))     ;
    if (aChkARG("p"))               ;
    if (aChkARG("?")||
        aChkARG("h")||
        aChkARGlong("help"))        ;

    if (aChkARGlong("createexcl"))  cmd_isCreateExc=1;
    if (aChkARG("z"))                                  cmd_isDump=getLevel(ARG,1,1,3,'z',0,NULL); // getNumFromTable

    if (aChkARGlong("yes"))     cmd_isYesFlg=1;
    if (aChkARGlong("initdb"))  cmd_isInitDB=1;
    if (aChkARG("d")||aChkARGlong("db"))               if (ARG) strcpy_ex(HSguard_DB,ARG);

    if (aChkARG("x")||aChkARGlong("exclude"))          if (ARG) { str=NULL; getPath(&str,ARG); exfiles=strlstadd(exfiles,str); }
    if (aChkARGlong("exclude_add"))                    if (ARG) exlst=strlstadd(exlst, strdup_ex(ARG));

    if (aChkARG("b")||aChkARGlong("base"))             if (ARG) getPath(&basis, ARG);
    if (aChkARG("v")||aChkARGlong("verbose"))          lmsg_level=getLevel(ARG,mFAIL,mROOT,mDEVL,'v',mUSER, getNumForVerbose);
                                                       // lmsg( GMSG_DEVL, "verbose set to %i (%s) from values %s",lmsg_level,lmsg_def[lmsg_level].str,ARG); }
    if (aChkARG("s")||aChkARGlong("noscreen"))         cmd_isNoScreen=(getFlagFromString(ARG,1,True))?1:0;
    if (aChkARGlong("update_deep"))                    if (ARG) { update_deep=atoi(ARG); cmd_isUpdate=1; }
    if (aChkARGlong("update_dir"))                     if (ARG) cmd_isUpdate=getPath(&update_dir,ARG);  //  { free0(update_dir); update_dir = strdup_ex(ARG); CorrectPath(update_dir); cmd_isUpdate=1; }

//todo:   if (aChkARGlong("update_links")&&(ARG))  followdir,followfile,skip ..defaul=ignore links
    if (aChkARG("u")||aChkARGlong("update"))           { cmd_isUpdate=1; if(ARG) getPath(&update_dir,ARG); }
    if (aChkARGlong("rescan"))                         { cmd_isUpdate=1; cmd_isRescanFlg=1; }

    if (aChkARG("n")||aChkARGlong("nodirtest"))        cmd_isNoDirTest=1;

    if (aChkARG("r")||aChkARGlong("recursive"))        cmd_isRecursive=getLevel(ARG,-1,-1,INT_MAX,'r',0, NULL);
    if (aChkARG("l")||aChkARGlong("list"))             { cmd_isListAll=1; if (ARG) getPath(&list_dir,ARG); } //{ list_dir=free0(list_dir); list_dir=strdup_ex(ARG); CorrectPath(list_dir); }} // ARG=what- see header
    if (aChkARGlong("listdir"))                        { cmd_isListDir=1; if (ARG) getPath(&list_dir,ARG); } //{ list_dir=free0(list_dir); list_dir=strdup_ex(ARG); CorrectPath(list_dir); }} //

    if (aChkARGlong("remove"))                         cmd_isRemove=getPathID(&info_path, ARG);
    if (aChkARGlong("finddupes"))                      cmd_isFindDupes=getPathID(&info_path, ARG);
    if (aChkARGlong("ldir"))                           { cmd_isDir=1; if (ARG) getPath(&info_path, ARG); } // feature um excludedateien zu unterstützen
    if (aChkARGlong("edir"))                           { cmd_isDir=2; if (ARG) getPath(&info_path, ARG); } // feature um rc-excludes zu unterstützen

    if (aChkARG("i")
        ||aChkARGlong("info")
        ||aChkARGlong("find")
       )                                               cmd_isInfo=getPathID(&info_path, ARG);
    if (aChkARGlong("infodir"))                        { cmd_isInfoDir = 1; if (ARG) getPath(&info_path, ARG); }

    if (aChkARG("f")||aChkARGlong("formout"))          cmd_isFormOut=getLevel(ARG,-1,FO_MIN,FO_MAX-1,'f',0,getNumFromTable);

//    if (aChkARG("c")||aChkARGlong("caption"))          cmd_isCaption=1;
    if (aChkARG("a")||aChkARGlong("ansi"))             cmd_isAnsiFlg=(getFlagFromString(ARG,1,True))?1:0; // + - on off true false 0 1 oder -a+ -a-

    // randomCheck SELECT title FROM albums ORDER BY random() LIMIT 1;
    if (aChkARG("t")
        ||aChkARGlong("testdb")
        ||aChkARGlong("check")
    )                                                  { cmd_isTestDB=1; info_path=free0(info_path); if (ARG) info_path=strdup_ex(ARG); }
    if (aChkARG("e")
        ||aChkARGlong("errhandle")
        ||aChkARGlong("emergency")
    )        										   if (ARG) getEmergency(ARG);

    if (aChkARG("o")||aChkARGlong("orphan"))           { ; }

    if (aChkARGlong("wbs"))                            { cmd_isWarnbigsz=default_wbs; if (ARG) cmd_isWarnbigsz=(size_t)Human2Num(ARG); }
    if (aarg_unused_print())    return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

/**
 @brief Lese den Level der angegeben wurde
 @return Level wie bei -t=7 -t- setzt minimum
 @param cmd       auszuwertendes Kommando
 @param def       defaultwert
 @param minimum   minimum Level
 @param maximum   maximum Level
 @param extension zeichen, wo man den level durch wiederholung angeben kann, wie -ttttttt ist auch 7

___[ Revision ]______________________________________________________________

 ** 06.05.25 HS Initial
___________________________________________________________________________*/

int getLevel(char *cmd, int def, int minimum, int maximum, char extension, int extstart, int(*func)(char*))
{
    int i;
    if (!cmd)
    {
        if (extstart) return getMinMax(extstart,minimum,maximum);
        return getMinMax(def,minimum,maximum);
    }

    if (isdigit(cmd[0]))            return getMinMax(atoi(cmd),minimum,maximum);
    if (cmd[0]=='-') if (!cmd[1])   return getMinMax(minimum,minimum,maximum);
    if ((extension)&&(cmd[0]==extension))
    {
        for (i=0;cmd[i]==extension;i++);
        if (extstart) return getMinMax(i+extstart,minimum,maximum);
        return getMinMax(i+1,minimum,maximum);
    }
    if (func)
    {
        i=func(cmd);
        if (i==getMinMax(i,minimum,maximum))
        {
            i--;i++;
            return i;
        }
    }
    if (!strcasecmp(cmd,"max"))     return getMinMax(maximum,minimum,maximum);
    if (!strcasecmp(cmd,"min"))     return getMinMax(minimum,minimum,maximum);
    return getMinMax(def,minimum,maximum);
}

/**
 @brief  value wird begrenzt auf minimum oder maximum
 @return value
 @param  value prüfwert
 @param  minimum
 @param  maximum

___[ Revision ]______________________________________________________________

 ** 06.05.25 HS Sub fuer getlevel
___________________________________________________________________________*/

int getMinMax(int value, int minimum, int maximum)
{
    if (value<minimum) value=minimum;
    if (value>maximum) value=maximum;
    return value;
}

/**
 @brief  Anhand des Named aus der format_table, die zugehörige nummer holen
 @return -1 on fail, sonst die passende nummer
 @param  name der gesucht werden soll

___[ Revision ]______________________________________________________________

 ** 20.05.25 HS Initial
___________________________________________________________________________*/

int getNumFromTable(char *name)
{
    int i;
    if (!name) return -1;
    for (i=0;format_table[i].tok;i++)
    {
        if (!strcasecmp(name,format_table[i].tok)) return format_table[i].num;
    }
    return -1;
}

/**
 @brief verbose mit optionen
 @param name des levels
 @return den berechneten Level

**ALTE BESCHREIBUNG**
 Im Default wird FAIL gesetzt. Dh. Alle Programmmeldungen mit der
 Inforamtionseinheit FAIL oder schlechter werden angezeigt.

 Befehl | Auswirkung
 -------|----------------
 -v     | Verboselevel mUSER
 -v-    | silence ( nur kritische Fehler, start & ende im syslog )
 -v=1   | fail onlny
 -v=2   | UserLevel
 -v=3   | InfoLevel (viele Nachrichten)
 -v=4   | DeveloperLevel

 Es gibt sehr viele gleichbedeutende Befehle.
 silence   --verbose=0 --verbose 0 --verbose=- -v- -v0
 fail      --verbose=1 -v1 -v=1 -v=fail
 user      --verbose=2 -vv -v=2 -v=user usw.
 info      --verbose=3 -vvv -v=info usw.
 devl      -vvvv usw.

___[ Revision ]______________________________________________________________

 ** 01.05.25 HS Initial
 ** 06.05.25 HS min/max
 ** 25.05.25 HS Ganz neue Funktion da in getlevel eingeflossen
___________________________________________________________________________*/

int getNumForVerbose(char *name)
{
    int i;
    if (!name) return -1;
    for (i=mROOT;i<=mDEVL;i++)
    {
        if (!strcasecmp(name,lmsg_def[i].str)) return i;
    }
    return -1;
}

/**
 @brief getPath neu zuweisen
 @return 1
 @param [IN/OUT] value neue Speicher des Ortes
 @param inputpath

 Wenn inputpath NULL oder leerist, dann wird value=NULL
 ansonsten wird der neue Pfad zugewiesen und ein gesichererter
 Pfad erstellt. Ob der Pfad valid ist wird nicht geprüft.

___[ Revision ]______________________________________________________________

 ** 06.05.25 HS sub getCmdLineSwitches
___________________________________________________________________________*/

int getPath (char **value, char *inputpath)
{   //{ free0(update_dir); update_dir = strdup_ex(ARG); CorrectPath(update_dir); cmd_isUpdate=1; }
    char *result;
    if (inputpath) if (inputpath[0]==0) inputpath=NULL;
    result = free0(*value);
    if (inputpath)
    {
        result = strdup_ex(inputpath);
        CorrectPath(result);
    }
    *value=result;
    return 1;
}

/**
 @brief  Sieht nach ob '@' das erste Zeichen ist
 @return 1
 @param [IN/OUT] value neue Speicher des Ortes
 @param inputpath

 ansonsten wird @ref getPath aufgerufen

___[ Revision ]______________________________________________________________

 ** 10.05.25 HS Initial
___________________________________________________________________________*/

int getPathID(char **value, char *inputpath)
{
    char *result;
    if (inputpath) if (inputpath[0]==0) inputpath=NULL;
    if (inputpath)
    {
        if (inputpath[0]=='@')
        {
            result = free0(*value);
            result = strdup_ex(inputpath);
            *value=result;
            return 1;
        }
    }
    return getPath(value,inputpath);
}

/**
 @brief  Das Verzeichnis in info_path wird in place_s pla gespeichert
 @return 0 ohne Ergebnis / 1 mit Ergebnis -1 wird abgefangen und abgebrochen
 @param  pla zielspeicher

___[ Revision ]______________________________________________________________

 ** 12.05.25 HS Initial
___________________________________________________________________________*/

int getPlaceFrom_info_path(struct place_s *pla)
{
    int rc;
    sqlid_t id;
    if (!info_path) die (NULL);
    if (info_path[0]=='@')
    {
        id=(atoll(&info_path[1]));
        rc=sql_location_getByID(id,pla);
    }else{
        rc=sql_location_getDir(info_path,pla);
    }
    if (rc<0)
    {
        lmsg(mFAIL,"Error while get from DB: %s", strNotNULL(info_path));
        die (NULL);
    }
    return rc;
}

/**
 @brief  Aus der Liste der Angaben Parameter ableiten
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  option Emergency String

 optionen werden in einem String getrennt durch Kommas übergeben

 ZAHL       direkte Verwendung, nur development
 COUNT=     Es wird die angegebene Zahl von Fehlern gezählt. Bei 0 STOP
 SKIP       Es wird geprüft, aber das bleibt ohne weitere Aktion
 ABORT      Es wird mit Errorlevel 1 "normal" beendet.
 UPDATE     Auch im Fehlerfall wird das lastchk Datum neu gesetzt
 UPDATEALL  Wie Oben, nur die neue Size,CRC,FMTime werden geupdatet
 PANIC      Alles wird ignoriert. Sofortige Abbruch mit Errlevel 99

 Es wird ausdrücklich die Option PANIC empfohlen!

testDB ...
 r      Random statt strategisches Suchen der zu prüfenden ID's
 <zahl> Anzahl der Files die geprüft werden soll
 <humn> Größe auf der Festplatte, die geprüft wird. Das kann auch 2MB oder
        40TB sein.
 path   zu Prüfender Pfad


___[ Revision ]______________________________________________________________

 ** 16.05.25 HS Initial
___________________________________________________________________________*/

int getEmergency(char *option)
{
    int i;
    char *local_opt;
    int comma;

    local_opt=strdup_ex(strNotNULL(option));
//    lprintf ("local_opt=%s", local_opt);
    for (i=0;;i++)
    {
        if ((local_opt[i]==0)&&(i>0))
        {
            emergency=atoi(local_opt);
            free(local_opt);
            return EXIT_SUCCESS;
        }
        if (!isdigit(local_opt[i])) break;
    }

    stradd(local_opt,",");
    for (emergency=0;;)
    {
        if (local_opt[0]==0) break;
        comma=strchrpos(local_opt,',',0);
        if (comma<0) break;
        local_opt[comma]=0;

             if (!strcasecmp(local_opt,"skip"))       emergency |= emSKIP;
        else if (!strcasecmp(local_opt,"panic"))      emergency |= emPANIC;
        else if (!strcasecmp(local_opt,"abort"))      emergency |= emABORT;
        else if (!strcasecmp(local_opt,"update"))     emergency = (emergency & ~(emUPDATE)) | emUPLCHK;
        else if (   !strcasecmp(local_opt,"updatefull")
                 || !strcasecmp(local_opt,"updateall")
                )                                     emergency = (emergency & ~(emUPLCHK)) | emUPDATE;
        else if (!strncasecmp(local_opt,"abort=",6))
        {
                emergency |= emABORT;
                if (isdigit(local_opt[6]))
                {
                    i=atoi(&local_opt[6]);
                    if ((i>=2) && (i<99)) abortlevel=i;
                }
        }
        else if (!strncasecmp(local_opt,"count=",6))
        {
            if (isdigit(local_opt[6]))
            {
                i=atoi(&local_opt[6]);
                if ((i>=0)&&(i<emMIN))
                {
                    emergency &= ~(emMIN-1);    // untere Bits ausblenden
                    emergency |= i;             // Zahl wieder einpflegen
                }
            }
        }else{
                sql_ErrMsg=strprintf("unknown to handle emrgency: %s",local_opt);
                free(local_opt);
                die(sql_ErrMsg);
        }
        local_opt[comma]='*';
        strdel(local_opt,0,comma+1);
    }
    free(local_opt);
//    i=emMIN-1;
//    i=emergency&~(emMIN-1);
    if ((emergency&~(emMIN-1))==0) emergency|=emPANIC;
    return EXIT_SUCCESS;
}

/*
UPDATE,UPDATEALL,SKIP,COUNT=2,UPDATE,UPDATEALL
max_errors??
cmd_isErrorCount=3
cmd_isErrorCase=3,Abort99
cmd_isErrorCase=Abort1 PANIK
               =ABORT99
// errorhandle  =
    WriteProtokoll ( Write an Text to /tmp/hsguard-error.txt using in emails)
        AND ABORT           . this ( the entry will later come back ) dflt.
        AND SKIP complete   . this ( the entry will later come back )
        AND update lastchk only ( old parameters are leave untouched  )
        AND update fsize&crc&lastchk only ( this is like a rescan )
*/

/**
 @brief die and exit
 @param msg string to say goodbye

___[ Revision ]______________________________________________________________

 ** 24.04.25 HS Initial
___________________________________________________________________________*/

void die(char *msg)
{
    if (msg) lmsg (mFAIL,"%s", msg);
    cleanup(EXIT_FAILURE);
    exit(EXIT_FAILURE);
}

/**
 @brief Erstelle RCFullname und lese die Datei. Ergebnis in Liste
 @return void*

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Abstrahiert von GetConfig
___________________________________________________________________________*/

void *readCFG(int opt_defined, char* path)
{
    int opt;
    opt = opt_defined|opt_RCdir_nodflt;

    if (path)
    {
        opt = opt|opt_RCdir_PathFile;
        return RCread(opt,NULL,path);
    }
#ifdef HS_DEBUG
    opt=opt|opt_RCdir_isrc|opt_RCdir_PRG;
#else
    opt=opt|opt_RCdir_isrc|opt_RCdir_root_etc;
#endif
    return RCread(opt,NULL,m_PRGNAME);

//debug opt = opt_RCdir_nodflt|opt_RCdir_isrc|opt_RCdir_root_etc;
//
//    return RCread(opt_defined|opt_RCdir_nodflt|opt_RCdir_PathFile,NULL,"/hs/b/hsb.rc");
//    return RCread(opt,"/hs/src/cons/hsguard/hsguard.my.rc",m_PRGNAME);
}


/**
 @brief Lese Config aus Datei
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 24.04.25 HS Initial
 **    04.25 HS Mehrfach umgebaut
 ** 28.05.25 HS bis dato in der Funktion, jetzt GetConfigFrom
___________________________________________________________________________*/

int GetConfig (void)
{
    int rc;
    void *cfg;
    char *sec;
    char *forward;
    char *tmp;
#define RC_DEFAULT_READCFG opt_RCdir_silence|opt_RCdir_isExpVars|opt_RCdir_imSections
    cfg=NULL;
    sec=NULL;
    if (aChkARGlong("config"))
    {
        if (!ARG)         die("use --config <EXISTRC>");
        if (!FileOK(ARG)) die("file not found: use --config <EXISTRC>");
              cfg = readCFG(RC_DEFAULT_READCFG, ARG);
    }
    if (!cfg) cfg = readCFG(RC_DEFAULT_READCFG, NULL);
    if (cfg)
    {
        forward=strdup_ex(RCfind(cfg,"FORWARD"));
        if (forward)
        {
            tmp=strchr(forward,',');                                            // for , section
            if (tmp)
            {
                *tmp='\0';
                tmp++;
                sec=strdup_ex(tmp);
            }
            Node_DelAll(cfg);
            cfg=readCFG(RC_DEFAULT_READCFG, forward);
            free(forward);
        }
    }
    if (!cfg)
    {
        lmsg (mFAIL, "%s not exists", RCFullName);
        lmsg (mFAIL, "create cfg with \"sudo %s --create\" to create the base", m_PRGNAME);
        sec=free0(sec);
        return EXIT_FAILURE;
    }
//    if (0==0) { for (void *ll=cfg;ll;ll=Node_GetNext(ll)) printf ("%s\n",(char*)Node_GetData(ll)); exit (0); }
//    char *section=""; value=g_RCfind(cfg, "DATABASE", section); read section(backup).DATABASE or fallback to "DATABASE"
    if (aChkARG("p")||aChkARGlong("section"))
    {
        if (ARG)
        {
            sec=free0(sec);
            sec=strdup_ex(ARG);
        }
    }
    rc = GetConfigFrom(cfg,NULL);
    if (!rc) if (sec) rc=GetConfigFrom(cfg,sec);
    Node_DelAll(cfg);
    sec=free0(sec);
    return rc;
}

/**
 @brief  Lese aus Node_List die Config
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  cfg point zum StartNode
 @param  section (darf NULL sein) die mit dem Parameter gelesen wird

___[ Revision ]______________________________________________________________

 ** 28.05.25 HS Initial
___________________________________________________________________________*/

int GetConfigFrom (void *cfg, char *section)
{
    int i;
    int mapnum;
    int colnum;

    char *value;
    char *str;

    value=RCfind_in_Section(cfg, "DATABASE", section,0);
    if (value) strcpy_ex(HSguard_DB, value);

    value=RCfind_in_Section(cfg, "BASE", section,0);
    if (value) getPath(&basis,value);

    value=RCfind_in_Section(cfg, "VERBOSE", section,0);
    if (value) lmsg_level=getLevel(value,mFAIL,mROOT,mDEVL,'v',mUSER, getNumForVerbose);

    value=RCfind_in_Section(cfg, "EMERGENCY", section,0);
    if (value) if (value[0]) getEmergency(value);

    value=RCfind_in_Section(cfg, "NOSCREEN", section,0);
    if (value) cmd_isNoScreen = (getFlagFromString(value,1,True))?1:0;

    value=RCfind_in_Section(cfg, "ANSICOLOR", section,0);
    if (value) cmd_isAnsiFlg = (getFlagFromString(value,1,True))?1:0;

    value=RCfind_in_Section(cfg, "WARNWBS", section,0);
    if (value) cmd_isWarnbigsz=(size_t)Human2Num(value);

    for (i=col_MIN;i<col_MAX;i++)
    {
        mapnum = color_get_mapnum(i);
        str = strdup_ex(strNotNULL(color_getmap_name(mapnum)));
        strupr(str);
        value=RCfind_in_Section(cfg, str, section,0);
        if (value)
        {
            colnum=color_getcolByName(value);
//            printf ("%s # %s # %i\n", str, value,colnum);
            if (colnum>=0) color_setmap_color(mapnum,colnum);
//        }else{
//            printf ("%s # %s\n", str, "not found");
        }
        free(str);
    }

    for (mapnum=0;;mapnum++)
    {
        value=RCfind_in_Section(cfg, "EXCLUDE", section, mapnum);
        if (!value) break;
        str=NULL;
        getPath(&str,value);
        exfiles=strlstadd(exfiles,str);
    }

    for (mapnum=0;;mapnum++)
    {
        value=RCfind_in_Section(cfg, "EXADD", section, mapnum);
        if (!value) break;
        exlst=strlstadd(exlst, strdup_ex(value));
    }
    return EXIT_SUCCESS;
}

/**
 @brief Show all internal Vars and EXIT_SUCCESS
 @return EXIT_SUCCESS

 Stellt sicher, dass man da ist

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
___________________________________________________________________________*/

#define ShowPars(PAR) #PAR, strNotNULL(PAR)
#define ShowPar(PAR) #PAR, PAR
#define ShowFRM "%-22s"

int showDump(void)
{
    int i;
    if (cmd_isDump==2)
    {
        printf(ShowFRM "%s\n", ShowPar(HSguard_DB));
        printf(ShowFRM "%i\n", ShowPar(lmsg_level));
        printf(ShowFRM "%i\n", ShowPar(cmd_isDump));
        printf(ShowFRM "%i\n", ShowPar(cmd_isFormOut));
        return EXIT_SUCCESS;
    }

    printf(ShowFRM "%s\n", ShowPar(RCFullName));
    printf(ShowFRM "%s\n", ShowPar(LogFileName));
    printf(ShowFRM "%s\n", ShowPar(HSguard_DB));
    printf(ShowFRM "%i\n", ShowPar(lmsg_level));
    printf(ShowFRM "%i\n", ShowPar(cmd_isNoScreen));
    printf(ShowFRM "%i\n", ShowPar(cmd_isAnsiFlg));

    printf(ShowFRM "%i\n", ShowPar(cmd_isDump));
    printf(ShowFRM "%i\n", ShowPar(cmd_isInitDB));
    printf(ShowFRM "%i\n", ShowPar(cmd_isUpdate));
    printf(ShowFRM "%i\n", ShowPar(cmd_isRescanFlg));
    printf(ShowFRM "%i\n", ShowPar(cmd_isNoDirTest));
    printf(ShowFRM "%i\n", ShowPar(cmd_isYesFlg));
    printf(ShowFRM "%i\n", ShowPar(cmd_isRecursive));
    printf(ShowFRM "%i\n", ShowPar(cmd_isListAll));
    printf(ShowFRM "%i\n", ShowPar(cmd_isListDir));
    printf(ShowFRM "%i\n", ShowPar(cmd_isTestDB));
//    printf(ShowFRM "%i\n", ShowPar(cmd_isCaption));
    printf(ShowFRM "%i\n", ShowPar(cmd_isFormOut));
    printf(ShowFRM "%i\n", ShowPar(sql_stored));

    printf(ShowFRM "%i\n", ShowPar(update_deep));
    printf(ShowFRM "%s\n", ShowPars(update_dir));
    printf(ShowFRM "%s\n", ShowPars(list_dir));
    printf(ShowFRM "%i\n", ShowPar(lmsg_level));

    printf(ShowFRM "%i\n", ShowPar(cmd_isInfo));
    printf(ShowFRM "%i\n", ShowPar(cmd_isInfoDir));
    printf(ShowFRM "%s\n", ShowPars(info_path));

    printf(ShowFRM LLD_STR "\n", ShowPar(sql_lastdirnum));
    printf(ShowFRM "%s\n", ShowPars(sql_lastdirname));
    printf(ShowFRM "%s\n", ShowPars(list_dir));
    printf(ShowFRM "%s\n", "emergency", RightStr(binstr(emergency ),16));
    printf(ShowFRM "%s\n", ShowPars(basis));

    if (exfiles)
    {
        for (i=0;exfiles[i];i++)
        {
            printf(ShowFRM "%s\n", "exfile", exfiles[i]);
        }
    }else{
        printf(ShowFRM "%s\n", "exfile", "-");
    }
    if (cmd_isDump<3) return EXIT_SUCCESS;
    if (exlst)
    {
        printf ("excludes:\n");
        for (int dmp=0;exlst[dmp];dmp++) { printf ("%02i - %s\n",dmp,exlst[dmp]); }
    }
    return EXIT_SUCCESS;
}

/**
 @brief Erstelle eine ConfigDatei
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
___________________________________________________________________________*/

int Execute_Create(void)
{
    int i,c,mapnum;
    char *str;
    char rtErrorMsg[STRING_MAX];
    FILE *OUT;

    void *cfg;
    cfg = readCFG(opt_RCdir_silence,NULL);
    Node_DelAll(cfg);

//    printf ("access: %i %s\n",access(Cdirname(RCFullName),W_OK),RCFullName); return 0;

#ifdef HS_DEBUG
    if (aChkARGlong("yes"))     cmd_isYesFlg=1;
    if (FileOK(RCFullName)) if (cmd_isYesFlg) remove(RCFullName);
#endif // HS_DEBUG
    if (FileOK(RCFullName))
    {
        sprintf_ex(rtErrorMsg, "File exists. Please remove first: %s", RCFullName);
        die (rtErrorMsg);
    }
    printf ("\n\nSure you want to create %s (y/n) ? ",RCFullName  );
    for (;;)
    {
        if (cmd_isYesFlg) c='y';
        else c=getc(stdin);
        if (tolower(c)=='y') break;
        if (tolower(c)=='n') break;
    }
    if (tolower(c)!='y') die ("break here");
    printf ("Y\n\n");

    //strcpy (RCFullName,"/tmp/ichdarf.das");
//    if (access(Cdirname(RCFullName),W_OK)!=0) return EXIT_FAILURE;

    if ((OUT=fopen(RCFullName,"wt")) == NULL)
    {
        sprintf_ex(rtErrorMsg, "File can't be opened for writing; may sudo help?: %s", RCFullName);
        die (rtErrorMsg);
    }
    HSguard_DB[0]=0;
#ifdef HS_DEBUG
    if (HSguard_DB[0]==0) if (DirOK("/hs/db"))       strcpy (HSguard_DB,"/hs/db/hsguard_debug.db");
#endif
    if (HSguard_DB[0]==0) if (DirOK("/hs/db"))       strcpy (HSguard_DB,"/hs/db/hsguard.db");
    if ((HSguard_DB[0]==0) && (!DirOK("/var/hsguard")))
    {
        printf ("Create Dir in /var/hsguard (y/n) ? ");

        for (;;)
        {
            if (cmd_isYesFlg) c='y';
            else c=getc(stdin);
            if (tolower(c)=='y') break;
            if (tolower(c)=='n') break;
        }
        if (tolower(c)=='y')
        {
            printf ("Y\n\n");
            mkdir("/var/hsguard",0775);
        }else{
            printf ("NO\n\n");
        }
    }
    if (HSguard_DB[0]==0) if (DirOK("/var/hsguard")) strcpy (HSguard_DB,"/var/hsguard/hsguard.db");

    fprintf (OUT, "DATABASE=%s\n",HSguard_DB);
    fprintf (OUT, "BASE=/\n");
    fprintf (OUT, "#ROOT/FAIL/USER/INFO/DEVL\n");
    fprintf (OUT, "VERBOSE=USER\n");
    fprintf (OUT, "#If desired, an empty file can be created with --createexcl\n");
#ifdef HS_DEBUG
    fprintf (OUT, "EXCLUDE=/var/hsguard/exclude-debug.lst\n");
#else
    fprintf (OUT, "#EXCLUDE=/var/hsguard/exclude.lst\n");
#endif
    fprintf (OUT, "NOSCREEN=Y\n");
    fprintf (OUT, "EMERGENCY=PANIC\n");
    fprintf (OUT, "ANSICOLOR=Y\n");

    fprintf (OUT, "#COLORS=");
    for (i=COLOR_MIN;i<COLOR_MAX;i++) fprintf (OUT, "%s%s", i==0?"":",", color_getcolor(i));
    fprintf (OUT, "\n");

    for (i=col_MIN;i<col_MAX;i++)
    {
        mapnum=color_get_mapnum(i);
        str = strdup_ex(strNotNULL(color_getmap_name(mapnum)));
        strupr(str);
        fprintf (OUT, "%s=%s\n", str, color_getmap_colorname(mapnum));
        free(str);
    }
//    fprintf (OUT, "\n");
    fclose  (OUT);
    printf ("generated %s\n",RCFullName);
    return EXIT_SUCCESS;
}


/**
 @brief Erstelle eine leere Datenbank
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 26.04.25 HS Initial
 ** 27.04.25 HS real Create nach g_sql.c verschoben
___________________________________________________________________________*/

int Execute_InitDB(void)
{
    int c;
    char rtErrorMsg[STRING_MAX];
#ifdef HS_DEBUG
    if (FileOK(HSguard_DB)) remove(HSguard_DB);
#endif
    if (FileOK(HSguard_DB))
    {
        sprintf_ex(rtErrorMsg, "File exists. Please remove first: %s", HSguard_DB);
        die (rtErrorMsg);
    }
    printf ("\n\nSure you want to create %s (y/n) ? ", HSguard_DB);
    if (cmd_isYesFlg) c='y';
    else c=getc(stdin);
    if (tolower(c)!='y') die ("break here");
    printf ("Y\n\n");
    return sql_create_database();                                               // Aufruf zum REAL-Create in g_sql.c
}

/**
 @brief Prüfe ob Datenbank und basis gesetzt und vorhanden sind
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 05.05.25 HS Initial
___________________________________________________________________________*/

int check_basics(void)
{
    if (HSguard_DB[0]==0)
    {
        lmsg (mFAIL, "base fail: DATABASE= in \"%s\" not found", RCFullName);
        return EXIT_FAILURE;
    }
    if (!basis)
    {
        lmsg(mFAIL,"no base set; noir in RC or CMDSwitch");
        return EXIT_FAILURE;
    }
    if (!DirOK(basis))
    {
        lmsg(mFAIL,"base is set; but not found; try -b/ [-l %s]", basis);
        return EXIT_FAILURE;
    }
    if (! (emergency & (emPANIC|emABORT|emSKIP) ))
    {
        lmsg(mFAIL,"emergency need an actionparameter, but no one found. try -e SKIP,COUNT=3");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 @brief  sql_datalst freigeben aber den inhalt als Rückgabe
 @return ehemaligen Inhalt von sql_datalst

___[ Revision ]______________________________________________________________

 ** 23.05.25 HS Initial
___________________________________________________________________________*/

void *get_sql_datalst(void)
{
    void*x;
    x=sql_datalst;
    sql_datalst=NULL;
    return x;
}

/**
 @brief  Sicherstellen, dass in info-/update_dir ein Verzeichnis ist.
 @return verzeichnisname wo es los geht

 War update_getDir_or_basis_orDie

___[ Revision ]______________________________________________________________

 ** 24.04.25 HS Initial
 ** 27.05.25 HS Sowas von neu entwickelt
 ___________________________________________________________________________*/

int ensureDir(char **origin, char*base)
{
    char *lidir;
    char *a;

    if (!base)                                                                  // Basis muss vorhanden sein
    {
        lmsg(mFAIL, "need a base to work");
        return EXIT_FAILURE;
    }
    lidir=*origin;                                                              // Verzeichnis bisher
    if (!lidir) lidir=strdup_ex(base);                                          // wenn nix, dann Basis
    if (lidir[0]!=cDIR_SEP)                                                     // erstes Zeichen ein slash?
    {
        a=lidir;                                                                // merke "altes Verzeichnis"
        lidir=strdup_ex(base);                                                  // basis (Kopie)
        if (strcmp(base,DIR_SEP)) stradd(lidir,DIR_SEP);                        // Wenn Base kein Slash ist ...
        stradd(lidir,a);                                                        // Altes Verzeichnis dran
        free(a);                                                                // und jetzt kann das auch weg
    }
    if (strcmp(base,"/"))                                                       // lidir muss ein Teil von basis sein
    {
        if (strncmp(lidir,base,strlen(base)))
        {
            lmsg(mFAIL, "dir must a part of base or for with -b /");
            return EXIT_FAILURE;
        }
    }
    *origin=lidir;                                                              // Wieder zurückgeben
    return EXIT_SUCCESS;
}

/**
 @brief lmsg setup
 @return EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 01.05.25 HS Initial
 ** 10.05.25 HS int log_screen/moved 2 g_helper
 ** 17.05.25 HS LMSG_SILENT ist im RELEASE immer silent, will ich aber nicht
___________________________________________________________________________*/

int lmsgGuard(int log_screen)
{
    int i;
    lmsg_setup(LMSG_SILENT, LOG_LOGFILE | LOG_SYSLLOG, "SILN");
#ifdef HS_DEBUG
    lmsg_setup(mROOT,                 LOG_STDERR | LOG_LOGFILE | LOG_SYSLLOG, "ROOT");
#else
    lmsg_setup(mROOT,                              LOG_LOGFILE | LOG_SYSLLOG, "ROOT");
#endif
    lmsg_setup(mFAIL,                 LOG_STDERR | LOG_LOGFILE | LOG_SYSLLOG, "FAIL");
    lmsg_setup(mUSER,  (log_screen?LOG_STDERR:0) | LOG_LOGFILE              , "USER");
    lmsg_setup(mINFO,  (log_screen?LOG_STDERR:0) | LOG_LOGFILE              , "INFO");
    lmsg_setup(mDEVL,  (log_screen?LOG_STDERR:0) | LOG_LOGFILE              , "DEVL"); //->silent ?
    for (i=mMAX;i<LMSG_MAX;i++)
    {
        lmsg_setup(i,log_screen?LOG_STDERR:0,"UNDEF");
    }
    lmsg_format=2;
    return EXIT_SUCCESS;
}

/**
 @brief Alle Strings etc freigeben
 @return was immer in exitcode steht
 @param exitcode der rückgabewert

___[ Revision ]______________________________________________________________

 ** 27.04.25 HS Initial
___________________________________________________________________________*/

int cleanup(int exitcode)
{
    lmsg (mUSER, "cleaning up");
    long_table_free();
    sql_lastdirname=free0(sql_lastdirname);
    sql_ErrMsg = free0(sql_ErrMsg);
    update_dir = free0(update_dir);
    sql_Data   = free0(sql_Data);
    basis      = free0(basis);
    list_dir   = free0(list_dir);
    info_path  = free0(info_path);
    exfiles    = strlstfree(exfiles);
    exlst      = strlstfree(exlst);
    sql_datalst= Node_DelAll(sql_datalst);
    return exitcode;
}

