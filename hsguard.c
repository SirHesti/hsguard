/**	***********************************************************************************************

 @file   hsguard.c
 @author Sir Hesti (hstools@t-online.de)
 @brief  Erstellt(prüft) eine Datenbank von angegebenen Files
 @date   20.04.2025 (01.01.2020)
 @copyright GNU Public License. http://www.gnu.org/licenses/gpl-3.0.html


#-[ HS: Devoloper Notes ]--------------------------------------------------------------------------

--update_dir /tmp --update_deep=2 -b /tmp
/hs/src/cons/hswatchd/manmake.sh
--db /hs/db/hsb-database.db --base /hs/b --update_dir /hs/b --list /hs/b/anne-handy -r -v=USER
--db /hs/db/hsb-database.db --base /hs/b -vvv -e ABORT=16,COUNT=1 -t 10,5GB,/hs/b/later_view/taeglich_gr._weihnachtsmann
--db /hs/db/hsb-database.db --base /hs/b -vvv --update_dir /hs/b/anne-handy -d

----- ??????????  ---------------------------------------------------------------------------------

![Build Status](https://github.com/gnunn1/tilix/workflows/Build%20Test/badge.svg)
[![Translation status](https://hosted.weblate.org/widgets/tilix/-/svg-badge.svg)](https://hosted.weblate.org/engage/tilix/?utm_source=widget)
###### Screenshot
![Screenshot](https://gnunn1.github.io/tilix-web/assets/images/gallery/tilix-screenshot-1.png)

#-[ README-de.md ]---------------------------------------------------------------------------------

Deutsche Doku

#-[ README.md ]------------------------------------------------------------------------------------

switch to german translation
[![Translation status](🇩🇪)](README-de.md)

#### About

hsguard stellt aus den konfigurierbaren Verzeichnisse eine Datenbank zusammen. Dabei wird jede
Datei für sich geprüft und eine CRC-Prüfsumme (OLDSTYLE) erstellt. Ist die Festplatte erst
einmal in der Datenbank, dann kann jede Datei auf (nicht gewollte) Änderungen geprüft werden.

Genau das ist der Zweck von hsguard. Festplatten (HDD) altern und wenn Daten nicht gelesen
werden, dann 'vergisst' die Festplatte mal ein Bit.

Bei mir laufen HDD's rund um die Uhr als Backupsystem, lokales  Filesharing etc.. Daten auf
diesen Platten möchte ich auch nach vielen Jahren behalten.

Zusätzlich kann so erkannt werden, ob eine Malware gerade dabei ist sämtliche Daten zu
verschlüsseln.

Hier greift "hsguard". hsguard speichert alle wichtigen Informationen zu einer Datei. Eine
Datei die geändert wurde, wird in der Datenbank normal geupdatet. Über jede Datei wird
eine CRC32 gebildet. Die Prüfsumme wird pro Datei gespeichert und kann gegengeprüft werden.

Es wird Position, Name, FileSize, FileModifDate und die CRC32 gespeichert. Prüfcounter und
Aufnahmedatum können aus der Datenbank ebenfalls abgerufen werden.

Im CronJob kann hsguard [--update] --check aufgerufen werden, wo verschiedene Dateien dann
gegengeprüft werden können. Diese werden im Zufallsprinzip ausgewählt. Ein Update nimmt
neue oder geänderte Dateien vorher auf.

Mit vielen Möglichkeiten können die Daten aufgelistet und verifiziert werden. So wird im Fall
des Falles die Datenbank eine hilfreiche Ergänzung sein.

WICHTIG! hsguard wird keine Festplatteninhalte ändern. Keine Datei löschen oder hinzufügen. Die
einzige Ausnahme ist die Datenbank selsbt. "hsguard --create" wird einmalig eine
hsguard.rc anlegen. Diese Datei wird nicht überschrieben. Alle Parameter können aber auch auf
der Kommandozeile angegeben werden.

hsguard benötigt lediglich Leserechte für die Daten.

hsguard kann auf vielfältig configuriert werden. Configdatei, Excludedatei und
Kommandozeilenswitch.

Wird hsguard das erste Mal gestartet, dann wird die Datei /etc/hsguard.rc fehlend bemängelt.
Mit "hsguard --create" kann eine erste Version mitsamt einen Unterverzeichnis /var/hsguard
erstellt werden. Ein Beispiel kann in der Datei hsguard_sample.rc nachgeschlagen werden.


#-[ hsguard.man ]----------------------------------------------------------------------------------
---
title: hsguard
section: 8
header: hsguard - coded by Hesti
footer: hsguard $VERSION
date: $DATE
---

# NAME
**hsguard** - Programm um Aenderungen im Dateisystem festzustellen

# SYNOPSIS

$README

# DESCRIPTION
hsguard liest das Dateisystem ein und speichert rudimentäre Daten.
Sofern nicht vorhanden/geändert(fsize/mtime) wird eine CRC32 der Datei
gebildet. Diese wird gegengeprüft und ggf. Alarm geschlagen.

# OPTIONS

$OPTIONS:g_helper.c:HELP

# ENVIRONMENT
siehe Anschnitt: **[hsguard.rc](#hsguard.rc)**

<a name="hsguard.rc"></a>

# /etc/hsguard.rc
<a name="RC-DATABASE"></a>

## DATABASE=
Die Datenbank selbst. Diese kann sinnvoll unter /var sein. Zusammen mit --initDB
kann diese als Grundkonfig erstellt werden. Dafür muss diese Lese-/Schreibrechte haben.
| \-\-db überschreibt die im RC-File angegebene Datei

<a name="RC-BASE"></a>

## BASE=
BASIS alles was angegeben wird. Ab wo soll die Datenbank erstellt werden. Diese Zeile sollte
unbedingt angepasst werden. ZB. kann der Pfad zu den schützendwerten
Daten /server/pictures oder ähnlichem.
-b <path> überschreibt das

<a name="RC-VERBOSE"></a>

## VERBOSE=
stellt den Loglevel dar. Je höher der Level je mehr Informationen zu den erledigten Aufgaben
findet man im Logfile bzw. auf dem Bildschirm. Feiner getunt über den Spezialswitch --noscreen
Verboselevel wird von 0-4 oder besser unterstützt:

 - `ROOT  0 :` Nur Systemfehler
 - `FAIL  1 :` Nur Nachrichten, die Fehler beinhalten
 - `USER  2 :` Standardeinstellung Es werden Informationen zu Start und Ende einer jeden Aktion ausgegeben
 - `INFO  3 :` Das können sehr viele Nachrichten sein
 - `DEVL  4 :` Das sollte man sich besser nicht ansehen, diese sind nur für Entwickler gedacht

<a name="RC-EXCLUDE"></a>

## EXCLUDE=
Es gibt Ordner die muessen nicht durchsucht werden, zB. TempOrdner oder der Ordner dieser Datenbank.
Die Dateien/Verzeichnisse werden zeilenweise notiert, die mit '?' und '*' auch Mehrfachausshluß zulassen.
Ein '#' am Zeilenanfang ist eine Kommentarzeile. Mehr zu diesem Thema **unter: [Exclude-File](#EXCLUDEFILE)**",

<a name="RC-NOSCREEN"></a>

## NOSCREEN=
kann N/Y sein. Bei Y werden fast alle Ausgaben nur im Logfile vermerkt und nicht mehr auf dem
Bildschirm ausgeben. Meldungen stehen nur im Logfile zur Verfügung. -s- auf der Kommandozeile kann dieses
Verhalten abschalten. Auch wenn NOSCREEN=Y angegeben wurde. Es wird empfohlen diese Zeile zum Einrichten
auszukommentieren.

<a name="RC-EMERGENCY"></a>

## EMERGENCY=
Hiermit wird festgelegt, wie --testDB bei Fehlern verfahren soll. Incl. deren Wiederhohlung.
Ausdrücklich wird die Option PANIC empfohlen.

 - `PANIC     :` sofortiger Abbruch. Programmende mit Errorlevel 99
 - `COUNT=    :` Es wird die angegebene Zahl von Fehlern gezählt. Bei 0 STOP
 - `SKIP      :` Es wird geprüft, aber das bleibt ohne weitere Aktion
 - `ABORT     :` Es wird mit Errorlevel 1 "normal" beendet. CONUT!
 - `UPDATE    :` Auch im Fehlerfall wird das lastchk Datum neu gesetzt
 - `UPDATEALL :` Wie Oben, nur die neue Size,CRC,FMTime werden geupdatet

-e oder --emergency können diese Einstellung überschreiben werden

<a name="RC-ANSICOLOR"></a>

## ANSICOLOR=
Viele Ausgaben werden, zur besseren Unterscheidung, farblich unterschiedlich gekennzeichnet.
Y/N kann die Ausgabe von AnsiColors ausschalten. Bei Y können die Farben auch noch variiert werden.
Diese können in der Sample-Datei angesehen werden.

<a name="RC-EXADD"></a>

## EXADD=

Hiermit kann eine Datei oder auch ein Verzeichnis ignoriert werden. Wesentlich besser geht es mit
[RCexclude](#RC-EXCLUDE).

<a name="RC-WBS"></a>

## WARNWBS

Dieser Parameter bewirkt das eine "Warnung" ausgegeben wird, wenn ein "großes File" erkannt
wird. Das kann sinnvoll sein, wenn die zu prüfenden Dateien nicht die schnellste Verbindung
haben. Damit der Cursor, nicht wie eingefroren, auf der Stelle anzeigefrei blinkt kann hier
eine Dateigröße einstellt werden. Die Warnung wird auf Userlevel ausgegeben. Die Angabe
erfolgt Humanlike wie 1TB oder 10MB. Auf 0 setzen um dieses Verhalten auszuschalten.
Defaultwert ist 16GB.

<a name="RC-FORWARD"></a>

## FORWARD

Einmalig kann hier auf eine andere Config verweisen. Das bietet sich an, wenn man andere
Config (z.B.im Netzwerk) verweisen kann. Als Feature ist ganz klar, dass auch die Section
direkt mit angegeben werden kann. Das kann dann so aussehen:
```
FORWARD=/srv/pub/backup.rc,std
```

<a name="EXCLUDEFILE"></a>

### Exclude Dateien

Es gibt viele gute Gründe einige Dateien oder ganze Verzeichnis(bäume) auszuschliessen. Das
kann ganz rudimentär mit nur einer Datei in der ExcludeDatei passieren. Ein Kommentar
beginnt mit einem '#'. Alles folgende ist ein Kommentar.

Mehrere Dateien können jeweils einzeln aufgenommen werden oder über das eingebaute
Fileglobbing eingefangen werden. Das Dateien die schon ein 'ls' einfangen würde, wie
'ls /var/log/*.log'.

Darüber hinnaus gibt es einige Schlüsselwörter, die etwas ausschliessen, wenn eine bestimmte
Situation zutrifft. z.B.:

```
ifhost WKST-Conny /etc/pconly.rc
```

Mit jeder neuen Zeile ist die Situation neu zu bewerten. Dh. keine Mehrzeiler.
Dafür können die 'Situationbewertungen' beliebig tief verschachtelt sein.
Folgendes kann so verwendet werden:

```
ifhost WKST-Conny ifuser werner message "Mach nix kaputt auf Connys PC"
```

token     | Beschreibung
---------:|:-----------------
ifhost X  | Wenn der aktuelle PC X ist
ifnhost X | Wenn der aktuelle PC nicht X ist
ifuser X  | Wenn der aktuelle Benutzer X ist
ifnuser X | Wenn der aktuelle Benutzer nicht X ist
ifbase X  | BASE=X (-b=X)
ifnbase X | wenn BASE nicht zutrifft
include X | X includen. Datei X an dieser Stelle einlesen
message X | Diese Nachricht ausgeben


# AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

# FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

#-[ hsguard.rc ]-----------------------------------------------------------------------------------
# hsguard has a buildin --create to create your *.rc file maybe you need sudo hsguard --initDB too
DATABASE=/var/hsguard/hsguard.db
BASE=/
#ROOT/FAIL/USER/INFO/DEVL
VERBOSE=USER
#If desired, an empty file can be created with --createexcl
#EXCLUDE=/var/hsguard/exclude.lst
NOSCREEN=Y
EMERGENCY=PANIC
ANSICOLOR=Y
#COLORS=BLACK,RED,GREEN,YELLOW,BLUE,PURPLE,CYAN,WHITE,BBLACK,BRED,BGREEN,BYELLOW,BBLUE,BPURPLE,BCYAN,BWHITE
COL_ID=BBLUE
COL_FNAME=BGREEN
COL_FSIZE=BWHITE
COL_FMTIME=BPURPLE
COL_CRC32=BYELLOW
COL_LASTCHK=BPURPLE
COL_CHECKCNT=BPURPLE
COL_FOUNDED=PURPLE
COL_LOCID=CYAN
COL_LOCATION=CYAN
COL_LOCFOUNDED=PURPLE

#-[ Revision ]-------------------------------------------------------------------------------------

 ** 01.01.20 HS Erste Ideen und entwürfe
 ** 20.04.25 HS Erste Programmzeilen
 ** 01.05.25 HS Grundsätzliche Funktionen geschaffen
 ** 04.05.25 HS --list
 ** 06.05.25 HS --rescan / Hauptabfrage nochmal überarbeitet/ getLevel
 ** 07.05.25 HS --info
 ** 08.05.25 HS g_color.c
 ** 10.05.25 HS NOSCREEN und lmsgGuard verschoben/erweitert
 ** 11.05.25 HS --remove
 **             --list aufgeräumt
 ** 12.05.25 HS --info mit 0xcrc und find fname like
 ** 16.05.25 HS --testDB und erster Echtlauf
 ** 18.05.25 HS Weitere Realdata-Test
 ** 19.05.25 HS Exclude jetzt mit "halber" Scriptsprache
 ** 20.05.25 HS --ldir möglich, ist nur zum Einrichten von Exclude-Dateien gedacht
 ** 22.05.25 HS immer noch zu langsam ...
 ** 24.05.25 HS Datum, zurückwandeln schlägt fehlt am Ende der Sommerzeit zwischen 2 und 3
 ** 27.05.25 HS places mit parent und in entries ist fmtime jetzt eine Integer
 ** 28.05.25 HS datenbank multi satt einzelabfragen
 ** 29.05.25 HS exclude mit include
 ** 31.05.25 HS --wbz / EXADD in RC-Files
 ** 01.06.25 HS exludes werden in Listen nicht mehr als singlefile verarbeitet
 ** 08.06.25 HS Dokumentation überarbeitet

#-[ ******************************************************************************************** */

#define hsguard_main
#include "hsguard.h"
#undef hsguard_main

#include "VERSION.h"

int startMsg(void);

int main(int argc, char *argv[])
{
    int rc;
    char *rc_msg;

	if (InitTools(argc , argv, "%d%m%v%b%t", I_MAJOR, I_MINOR, I_BUILD, I_BETA, I_BuildCount, I_BDate, LOG_SYSLLOG|LOG_STDERR)) return -1;

    lmsgGuard(1); lmsg_level=mFAIL;
    startMsg();

    if (aChkARG("?")||
        aChkARG("h")||
        aChkARGlong("help"))        return cleanup(showHelp());
    if (aChkARGlong("create"))      return cleanup(Execute_Create());

	if (GetConfig())                return cleanup(EXIT_FAILURE);
	if (GetCmdlineSwitches())       return cleanup(EXIT_FAILURE);

    if (check_basics())             return cleanup(EXIT_FAILURE);
    if (cmd_isCreateExc)            return cleanup(Execute_CreateExclude());

    if (excludeReadAll())           return cleanup(EXIT_FAILURE);

	if (cmd_isDump)                 return cleanup(showDump());

	if (cmd_isInitDB)               return cleanup(Execute_InitDB());
	if (cmd_isDir)                  return cleanup(Execute_Dir());

    if ((rc_msg=open_valid_sqlite3()) != NULL) die(rc_msg);

	if (cmd_isUpdate)               Execute_update();

	if (( cmd_isListAll
         +cmd_isListDir
         +cmd_isInfo
         +cmd_isInfoDir
         +cmd_isRemove
         +cmd_isFindDupes
         +cmd_isTestDB
        )>1) die ("if some entries cannot be made simultaneously");
    rc=0;
	if ((cmd_isListAll)||(cmd_isListDir))   rc=Execute_list();
	if ((cmd_isInfo   )||(cmd_isInfoDir))   rc=Execute_info();
	if (cmd_isRemove )                      rc=Execute_remove();
	if (cmd_isFindDupes )                   rc=Execute_FindDupes();
	if (cmd_isTestDB )                      rc=Execute_TestDB();

	cleanup(rc);
	lmsg (mROOT, "%s done%s", m_PRGNAME, rc?" with errors":"");
    return rc;
}

/**
 @brief startnachricht zusammen bauen (Parameter)
 @return EXIT_SUCCESS

___[ Revision ]______________________________________________________________

 ** 01.05.25 HS Initial
___________________________________________________________________________*/

int startMsg(void)
{
    int k;
    char *msg;
    msg=strprintf("%s.%i.%i.%i.%i.%s starting []", m_PRGNAME, m_MAJOR, m_MINOR, m_BUILD, m_BUILDCOUNT, m_BETA);
    if (m_PRG_argc>1)
    {
        for (k=1;k<m_PRG_argc;k++)
        {
            stradd(msg," ");
            stradd(msg,m_PRG_arguv[k]);
        }
    }
    lmsg (mROOT, "%s", msg);
    free(msg);
    return EXIT_SUCCESS;
}

//hsguard --update_deep=4 --update_dir /tmp
//	rc_msg=strdup_ex("wurst");
//	printf ("eins=%i\n", getPath(&rc_msg, "\\alles/kaese/"));
//	printf ("zwei=%s\n", rc_msg );
//    free(rc_msg);
//    return 14;

//    printf("%i %s\n", format_table[1].num, format_table[1].tok);
//	int n;
//    n=  1; rc = long_table_memory(n);  lmsg(mROOT,"ltm (%i) %i",n,rc);
//    n=  3; rc = long_table_memory(n);  lmsg(mROOT,"ltm (%i) %i",n,rc);
//    n=  4; rc = long_table_memory(n);  lmsg(mROOT,"ltm (%i) %i",n,rc);
//    n= 15; rc = long_table_memory(n);  lmsg(mROOT,"ltm (%i) %i",n,rc);
//    long_table_tester();die(NULL);
//    printf ("%i %i %i %s\n",emPANIC, emABORT, emSKIP, RightStr(binstr(emMIN-1),16));
//    printf ("%s\n",RightStr(binstr(emergency),16));
//    emergency =                0b01011;
//    printf ("%s\n",RightStr(binstr(emergency),16));
//    emergency =  emergency & ~(0b01000);
//    printf ("%s\n",RightStr(binstr(emergency),16));
//    die(NULL);
//    rc_msg=strprintf("\"this one\"");
//    rc_msg=strprintf("\"c\".");
//    strstrip(rc_msg,'\"');
//    printf ("rc_msg=%s\n",rc_msg);
//    free(rc_msg);
//    die(NULL);
//	Execute_Readcfg();
//	return cleanup(showDump());
// cmd_isDump=2; showDump(); die(0);

//    char *s="2025-05-27 12:08:10";
#if 0
    char *s[]={
        "2025-05-23 08:10:51",
        "2025-05-23 08:10:52",
        "2022-10-30 01:53:02",
        "2022-10-30 02:21:32",
        "2022-10-30 03:01:01"
    };

    int i;
    char *b;
    for (i=0;i<_countof(s);i++)
    {
//        printf ("s=%s\n",s[i]);
        printf ("d=" TIME_STR_LD " ... s=%s\n", sql_parse_date(s[i]),s[i]);
        b=strprintf("date --utc --date \"%s\" +\"u=%%s\"", s[i]); system (b); free(b);
        b=strprintf("date       --date \"%s\" +\"l=%%s\"", s[i]); system (b); free(b);
    }
    die(0);
#endif
#if 0
    #include <sys/ioctl.h>
    struct winsize ws;
    ioctl(1, TIOCGWINSZ, &ws);
    printf("Columns: %d\tRows: %d\n", ws.ws_col, ws.ws_row);

    char *s=getenv("COLUMNS");
    int ws_col = atoi(strNotNULL(getenv("COLUMNS")));
    printf("Columns: %s %d\n",s, ws_col);
#endif
