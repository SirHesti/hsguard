#include "hsguard.h"

/**
 @brief  Verzeichnis in eine structur lesen (nur die Namen)
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  dirname welches Verzeichnis
 @param  dlst ergebnisse hier

___[ Revision ]______________________________________________________________

 ** 20.05.25 HS Initial
 ** 01.06.25 HS root wie / kann nicht gelistet werden CresolvPath
___________________________________________________________________________*/

int getDirList(char *dirname, getDirList_t *dlst)
{
    struct dirent *dent;
    DIR *dir;
    char *local_name;

//    local_name = CresolvPath(dirname);
//    if (!strcmp(dirname,"/")) dlst->fullname=strdup_ex(dirname);
//    else
    dlst->fullname=strdup_ex(strNotNULL(CresolvPath(dirname)));
    CorrectPath(dlst->fullname);
    dir=opendir(dlst->fullname);
    if (!dir) return EXIT_FAILURE; //die ("can't opendir \"/\"");
    for (;;)
    {
        dent=readdir(dir);
        if (dent==NULL) break;
        if (!strcmp(dent->d_name,"."))  continue;                           // Diese beiden ergäben keinen Sinn
        if (!strcmp(dent->d_name,"..")) continue;
        local_name = strprintf("%s/%s", strcmp(dlst->fullname,"/")?dlst->fullname:"" ,dent->d_name);
             if (DirOK (local_name)) dlst->d=strlstadd(dlst->d,local_name);
        else if (FileOK(local_name)) dlst->f=strlstadd(dlst->f,local_name);
        else local_name = free0(local_name);
    }
    closedir(dir);

    if (dlst->option & D_DIRONLY)  dlst->f=strlstfree(dlst->f);
    if (dlst->option & D_FILEONLY) dlst->d=strlstfree(dlst->d);

    if (dlst->option & D_SORTET)
    {
        if (dlst->d) strlstsort(dlst->d);
        if (dlst->f) strlstsort(dlst->f);
    }
    return EXIT_SUCCESS;
}

/**
 @brief  Alle Werte nullen im struct getDirList_s
 @return EXIT_SUCCESS
 @param  dlst vom typedef getDirList_t

___[ Revision ]______________________________________________________________

 ** 20.05.25 HS Initial
___________________________________________________________________________*/


int getDirList_init(getDirList_t *dlst)
{
    dlst->d=NULL;
    dlst->f=NULL;
    dlst->option=D_SORTET;
    dlst->fullname=NULL;
    return EXIT_SUCCESS;
}

/**
 @brief  Alle Werte wieder freigeben im struct getDirList_s
 @return EXIT_SUCCESS
 @param  dlst vom typedef getDirList_t

___[ Revision ]______________________________________________________________

 ** 20.05.25 HS Initial
___________________________________________________________________________*/

int getDirList_delete(getDirList_t *dlst)
{
    dlst->d=strlstfree(dlst->f);
    dlst->d=strlstfree(dlst->d);
    dlst->fullname=free0(dlst->fullname);
    return EXIT_SUCCESS;
}

/**
 @brief  Verzeichnis ausgeben nach stdout, nutzbar in excludes
 @return EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 20.05.25 HS Initial
 ** 31.05.25 HS ldir/edir edir mit EXADD vorweg, n bíschen pfusch darf sein
___________________________________________________________________________*/

int Execute_Dir(void)
{
    int i;
    getDirList_t gdlst;
    getDirList_init(&gdlst);
    if (!info_path)                     die ("dirname missing");
    if (!DirOK(info_path))              die ("given dirname not found");
    if (getDirList(info_path, &gdlst )) die ("getDirList: not seen error occured");
    if (gdlst.d) for (i=0;i<strlstcount(gdlst.d);i++) printf ("%s%s\n", (cmd_isDir==2)?"EXADD=":""  ,gdlst.d[i]);
    if (gdlst.f) for (i=0;i<strlstcount(gdlst.f);i++) printf ("%s%s\n", (cmd_isDir==2)?"EXADD=":"" ,gdlst.f[i]);
    getDirList_delete(&gdlst);
    return EXIT_SUCCESS;
}

/**
 @brief Erstelle eine nicht ganz leere ExcludeDatei
 @return EXIT_FAILURE / EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 17.05.25 HS Initial
 ** 20.05.25 HS Verzeichnis ausgelagert
___________________________________________________________________________*/

int Execute_CreateExclude(void)
{
    int c,i;
    struct utsname hostinfo;
    FILE *OUT;
    char *exfile;

    getDirList_t gdlst;
    getDirList_init(&gdlst);

    if (!exfiles) die ("No exclude file is defined. Either enter EXCLUDE= in the rc-file or start with the --exclude= option.");
#ifdef HS_DEBUG
//    if (aChkARGlong("yes"))     cmd_isYesFlg=1;
//    if (FileOK(exfile))     if (cmd_isYesFlg) remove(exfile);
#endif // HS_DEBUG
    exfile=exfiles[0];
    if (FileOK(exfile)) { lmsg(mFAIL, "%s file exists; first remove it", exfile); die(NULL); }


    if (access(Cdirname(exfile),W_OK)!=0) die ("no permission to create");
    if (uname(&hostinfo)) die ("uname failure");

    printf ("\n\nSure you want to create %s (y/n) ? ", exfile );
    if (cmd_isYesFlg) c='y';
    else c=getc(stdin);
    if (tolower(c)!='y') die ("break here");
    printf ("Y\n\n");

    if ((OUT=fopen(exfile,"wt")) == NULL)
    {
        sql_ErrMsg=strprintf("File can't be opened for writing; may sudo help?: %s", exfile);
        die (sql_ErrMsg);
    }
    fprintf (OUT, "# this file is only intended as an example\n");
    fprintf (OUT, "# generated: %s\n", timeinfostr(unixtime()));
    fprintf (OUT, "\n");
    fprintf (OUT, "# follow you find your rootdir; disable or delete lines for working\n");

    if (getDirList("/", &gdlst )) die ("getDirList: not seen error occured");
    if (gdlst.d)
    {
        for (i=0;i<strlstcount(gdlst.d);i++) fprintf (OUT, "%s\n",gdlst.d[i]);
    }
    if (gdlst.f)
    {
        fprintf (OUT, "# some files too\n");
        for (i=0;i<strlstcount(gdlst.f);i++) fprintf (OUT, "%s\n",gdlst.f[i]);
    }
    getDirList_delete(&gdlst);
    fprintf (OUT, "# yes file/dir use with wildcard like \"/home/h?go*\"\n");
    fprintf (OUT, "# better. if the exclude running on more than one mashine add for host:\n");
    fprintf (OUT, "ifhost %s /home/%s/xxx-pictures\n", hostinfo.nodename, univault("%USER%"));
    fprintf (OUT, "# Nested queries are also possible\n");
    fprintf (OUT, "ifnbase /home ifhost \"PC-Backup\" \"/var/tmp\" \n");
    fprintf (OUT, "#eof#\n");

    fclose(OUT);
    printf ("please open with editor of your choice and view: %s\n",exfile);
#ifdef HS_DEBUG
//    sql_ErrMsg=free0(sql_ErrMsg);
//    sql_ErrMsg=strprintf("cat %s", exfile);
//    system(sql_ErrMsg);
#endif // HS_DEBUG
    return EXIT_SUCCESS;
}


/**
 @brief  start the the recode
 @return EXIT_FAILURE / EXIT_SUCCESS
 @param  startpoint

 Stellt sicher, dass man da ist

___[ Revision ]______________________________________________________________

 ** 01.06.25 HS Initial
___________________________________________________________________________*/

int excludeReadAll(void)
{
    int i;
    if (!exfiles) return EXIT_SUCCESS;
    for (i=0;exfiles[i];i++)
    {
        if (excludeRead(exfiles[i])) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* notes :
/proc
/dev
/mnt
/run
/sys

#-- if [root] /srv      dann /srv/pub/temp       pub/temp
#-- if [root] /srv/ub d dann /srv/pubb/temp       temp
#-- if [root] /tmp/test dann ....

#ifhost pc-nas    dann add /root/me_not
#ifhost pc-hesti  dann add /home/hesti/temp

#ifhost "PC-Hesti" /home/hesti/temp
ifhost numpf /procedure
ifhost "pc-Hesti" /home/hesti/xxx-pictures

#da muss es passen
root/temp
/tmp/a_2/b_2
#/tmp/a/big_data.file
/
*/

/**
 @brief Lese eine Liste von ausgeschlossenen Verzeichnissen/Dateien
 @return EXIT_FAILURE / EXIT_SUCCESS

 Die Liste wird abgelegt in einer Stringliste

___[ Revision ]______________________________________________________________

 ** 27.04.25 HS Initial
___________________________________________________________________________*/

enum {
    Nmessage=0,
    Nifhost,
    Nifnhost,
    Nifbase,
    Nifnbase,
    Nifuser,
    Nifnuser,
    Ninclude
};

int_char_t token[]={
    { Nmessage, "message" },
    { Nifhost,  "ifhost"  },
    { Nifnhost, "ifnhost" },
    { Nifbase,  "ifbase"  },
    { Nifnbase, "ifnbase" },
    { Nifuser,  "ifuser"  },
    { Nifnuser, "ifnuser" },
    { Ninclude, "include" }
};

int excludeRead(char *excludefile_toread)
{
    int i;
    int tok;
    frall_t *content;
    char **cmdlst;
    int iftrue;
    int cmdcount;
    int tokcount=_countof(token);
    char *username;
    struct utsname hostinfo;

    content = fread_all(excludefile_toread);                                                // mehrere dateien ? jetzt nur eine
    if (!content)
    {
        lmsg (mFAIL, "exclude not found: %s; maybe using --createexcl", excludefile_toread );
        return EXIT_FAILURE;
    }
    if (uname(&hostinfo)) die ("uname failure");
    username=strNotNULL(getlogin());

    for (;!fread_all_getline(content);)
    {
        strCL(content->nextline);                                               // unnötige Leerzeichen löschen
        if (!content->nextline[0]) continue;                                    // Leerzeilen sind ohne Inhalt
        if (content->nextline[0]=='#') continue;                                // Kommentare sind wie Leerzeilen

        cmdlst=strlst(content->nextline);                                       // content->nextline aufsplitten in einzelne schlüssel
        for (;cmdlst;)                                                          // solange eine cmdlst gibt
        {
            if (content->nextline[0]=='/') break;                               // wenn es mit '/' beginnt, dann ist das ein pfad
            cmdcount=strlstcount(cmdlst);                                       // aktuelle anzahl der schluesselworte
            if (cmdcount<1)  { content->nextline[0]=0; break; }                 // leer?, dann LEER
            if (cmdcount==1) { strcpy (content->nextline, cmdlst[0]); break; }  // ist nur noch ein Argument übrig, dann ende
#ifdef xHS_DEBUG
            printf ("::--------------------\n");
            for (i=0;i<cmdcount;i++) printf ("::%s\n", cmdlst[i]);
#endif
            for (tok=-1,i=0;i<tokcount;i++)                                     // Alle token durchsuchen nach dem schlüsselwort
            {
                if (strcmp(token[i].tok,cmdlst[0])) continue;
                tok=token[i].num;
                break;
            }
            strstrip(cmdlst[1],'\"');                                           // Argument von '"' berfreien
#ifdef xHS_DEBUG
            printf ("detect %i %s\n", tok, cmdlst[1]);
#endif
            iftrue=0;                                                           // Zeiger für weiteres Verhalten
            switch (tok)
            {
                case Nmessage:  lmsg (mUSER, "%s", cmdlst[1]);            iftrue--; break;
                case Nifhost:   if  (strcmp(cmdlst[1],hostinfo.nodename)) iftrue--; else iftrue++; break;
                case Nifnhost:  if (!strcmp(cmdlst[1],hostinfo.nodename)) iftrue--; else iftrue++; break;
                case Nifbase:   if  (strcmp(cmdlst[1],basis))             iftrue--; else iftrue++; break;
                case Nifnbase:  if (!strcmp(cmdlst[1],basis))             iftrue--; else iftrue++; break;
                case Nifuser:   if  (strcmp(cmdlst[1],username))          iftrue--; else iftrue++; break;
                case Nifnuser:  if (!strcmp(cmdlst[1],username))          iftrue--; else iftrue++; break;
                case Ninclude:  if (excludeRead(cmdlst[1]))
                                {
                                    strlstfree(cmdlst);
                                    fread_all_close(content);
                                    return EXIT_FAILURE;
                                }
                                iftrue--;
                                break;

                default:        lmsg (mFAIL, "Error in %s - near '%s' %s", excludefile_toread, cmdlst[0], content->nextline);
                                strlstfree(cmdlst);
                                fread_all_close(content);
                                return EXIT_FAILURE;
                                break;
            }
            if (iftrue<0)                                                       // false gefunden, nichts weiter nachschauen
            {
                content->nextline[0]=0;                                         // keine weiteren dinge hiermit machen
                break;
            }
            if (iftrue>0)                                                       // true, aber argemntcheck kommt
            {
                if (cmdcount-2<iftrue)                                          // genuegend Argumente vorhanden?
                {                                                               // nein, dann abbruch
                    lmsg (mFAIL, "Error in %s - near '%s %s'", excludefile_toread, cmdlst[0], cmdlst[1]);
                    strlstfree(cmdlst);
                    fread_all_close(content);
                    return EXIT_FAILURE;
                }
            }
            cmdlst=strlstdelete(cmdlst,0);                                      // schluesselwort removen
            cmdlst=strlstdelete(cmdlst,0);                                      // argument removen und dann nächster schritt
        }
        strCL(content->nextline);                                               // Rest aufhübschen
        strstrip(content->nextline,'\"');
        strstrip(content->nextline,'\'');
        if (!content->nextline[0]) continue;
        if (content->nextline[0]!='/')                                          // if not ("/",1) then add base...
        {
            if (strcmp(basis,"/")) strins(content->nextline,"/",0);             // basis schliesst nicht mit einem '/' ab, wenn basis also nicht '/'
            strins(content->nextline,basis,0);                                  // basis vor dem Salat einfügen
        }

        if ((!FileOK(content->nextline)) && (!DirOK(content->nextline)))
        {                                                                       // strmtch-Zeichen *? beachten
            if (!strchr(content->nextline,'*') && !strchr(content->nextline,'?'))
                lmsg(mINFO, "not exist tag, but in EXCLUDE-File: %s",content->nextline);
        }
        exlst=strlstadd(exlst,strdup_ex(content->nextline));
    }
    fread_all_close(content);
    return EXIT_SUCCESS;
}
