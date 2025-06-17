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

#-[ README.md ]---------------------------------------------------------------------------------

$LINK:🇩🇪 german translation 🇩🇪:README-de.md:hsguard-de.html:null

hsguard creates a database from the configurable directories. Each file is checked
individually and a CRC checksum (OLDSTYLE) is generated. Once the hard drive is included
in the database every file can be checked for (unwanted) changes.

This is precisely the purpose of hsguard. Hard drives (HDDs) age and if data is not read
the drive may "forget" a bit.

I have HDDs running around the clock as backup systems, local file sharing, etc. I want
to keep the data on these drives for many years.

Additionally, this makes it possible to detect if malware is in the process of encrypting
all your data.

This is where "hsguard" comes in. hsguard stores all important information about a file.
A file that has been changed is normally updated in the database. A CRC32 is calculated
for each file. The checksum is saved per file and can be checked against.

Position, name, file size, file modification date, and the CRC32 are stored. Check
counters and the date of inclusion can also be retrieved from the database.

In a cron job, hsguard [--update] --check can be called, where different files can then
be checked. These are selected at random. An update adds new or changed files beforehand.

With many options, the data can be listed and verified. Thus, in case of an incident,
the database will be a helpful supplement.

IMPORTANT! hsguard will not modify the contents of any hard drive. It will not delete
or add any files. The only exception is the database itself. "hsguard --create" will,
once, create a hsguard.rc. This file will not be overwritten. However, all parameters
can also be specified on the command line.

hsguard only requires read access to the data.

hsguard can be configured in many ways: config file, exclude file, and
command-line switches.

If hsguard is started for the first time, it will complain that the
file /etc/hsguard.rc is missing. With "hsguard --create", an initial version can be
created, along with a subdirectory /var/hsguard. An example can be found in the
file hsguard_sample.rc.

#-[ README-de.md ]------------------------------------------------------------------------------------

$LINK:🇬🇧 english version 🇬🇧:README.md:hsguard.html:null

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
**hsguard** - Program to detect changes in the file system

# SYNOPSIS

$README

# DESCRIPTION
hsguard reads the file system and stores basic data.
If a file is not present or has changed (fsize/mtime), a CRC32 checksum is
generated for the file. This checksum is checked and an alert is triggered
if necessary.

# OPTIONS

$OPTIONS:g_helper.c:HELP

# ENVIRONMENT
see section: **[hsguard.rc](#hsguard.rc)**

<a name="hsguard.rc"></a>

# /etc/hsguard.rc
<a name="RC-DATABASE"></a>

## DATABASE=
The database itself. This can usefully be located under /var. Together with --initDB,
this can be created as a basic configuration. For this, it must have read/write
permissions.
_\-\-db overrides the file specified in the RC file_

<a name="RC-BASE"></a>

## BASE=
BASE for everything specified. Where should the database be created from? This line
should definitely be adjusted. For example, the path to the data to be protected
could be /server/pictures or similar.
_\-b <path> overrides this_

<a name="RC-VERBOSE"></a>

## VERBOSE=
Represents the log level. The higher the level, the more information about the tasks
performed can be found in the log file or on the screen. More finely tuned via the
special switch _--noscreen_ .

Verbosity levels are supported from 0-4 or better:

 - `ROOT  0 :` Only system errors
 - `FAIL  1 :` Only messages that include errors
 - `USER  2 :` Default setting. Information about the start and end of each action is output
 - `INFO  3 :` This can be a lot of messages
 - `DEVL  4 :` Better not to look at this—intended only for developers

<a name="RC-EXCLUDE"></a>

## EXCLUDE=
There are folders that do not need to be searched, e.g., temp folders or the folder
of this database. The files/directories are listed line by line, and with '?'
and '*' you can exclude multiple items. A '#' at the beginning of a line is a
comment. More on this topic **under: [Exclude-File](#EXCLUDEFILE)**

<a name="RC-NOSCREEN"></a>

## NOSCREEN=
Can be Y/N. If Y, almost all output is only written to the log file and no longer
displayed on the screen. Messages are only available in the log file. -s- on the
command line can disable this behavior, even if NOSCREEN=Y is specified. It is
recommended to comment out this line when setting up.

<a name="RC-EMERGENCY"></a>

## EMERGENCY=
Here you specify how --testDB should handle errors, including their repetition.
The PANIC option is expressly recommended.

 - `PANIC     :` Immediate abort. Program ends with error level 99
 - `COUNT=    :` The specified number of errors is counted. STOP at 0
 - `SKIP      :` Checked, but no further action is taken
 - `ABORT     :` Ends "normally" with error level 1. COUNT!
 - `UPDATE    :` Even in case of errors, the lastchk date is updated
 - `UPDATEALL :` As above, but also updates new size, CRC, FMTime

_-e or --emergency can override these settings_

<a name="RC-ANSICOLOR"></a>

## ANSICOLOR=
Many outputs are color-coded for better distinction.
Y/N can disable the output of ANSI colors. If Y, colors can also be varied further.
These can be viewed in the sample file.

<a name="RC-EXADD"></a>

## EXADD=

Here you can ignore a file or even a directory. It’s much better to
use [RCexclude](#RC-EXCLUDE).

<a name="RC-WBS"></a>

## WARNWBS

This parameter triggers a "warning" when a "large file" is detected.
This can be useful if the files to be checked are not connected via the fastest
connection. To prevent the cursor from appearing frozen while waiting, a file size
can be set here. The warning is issued at user level. Specify in human-readable
format like 1TB or 10MB. Set to 0 to disable this behavior.

Default value is 16GB.

<a name="RC-FORWARD"></a>

## FORWARD

This can be used once to refer to another config. This is useful if you can refer
to other configs (e.g., on the network). As a feature, the section can be specified
directly. This might look like:

```
FORWARD=/srv/pub/backup.rc,std
```

<a name="EXCLUDEFILE"></a>

### Exclude Files

There are many good reasons to exclude certain files or entire directory trees. This
can be done very simply with a single file in the exclude file. A comment begins
with a '#'. Everything following is a comment.

Multiple files can each be added individually or captured using the built-in file
globbing. This works for files that 'ls' would find, like 'ls /var/log/*.log'.

Additionally, there are some keywords that exclude something if a certain situation
applies. For example:

```
ifhost WKST-Conny /etc/pconly.rc
```

With each new line, the situation is re-evaluated. That is, no multi-line blocks.
However, the 'situation evaluations' can be nested arbitrarily deep.
The following can be used in this way:

```
ifhost WKST-Conny ifuser werner message "Don't break anything on Conny's PC"
```

token     | Description
---------:|:-----------------
ifhost X  | If the current PC is X
ifnhost X | If the current PC is not X
ifuser X  | If the current user is X
ifnuser X | If the current user is not X
ifbase X  | BASE=X (-b=X)
ifnbase X | if BASE does not apply
include X | Include X. Read file X at this point
message X | Output this message


# AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

# BUGS
Please report bugs (including in this manpage)

#-[ hsguard-de.man ]----------------------------------------------------------------------------------
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

_\-\-db überschreibt die im RC-File angegebene Datei_

<a name="RC-BASE"></a>

## BASE=
BASIS alles was angegeben wird. Ab wo soll die Datenbank erstellt werden. Diese Zeile sollte
unbedingt angepasst werden. ZB. kann der Pfad zu den schützendwerten
Daten /server/pictures oder ähnlichem.

_\-b <path> überschreibt das_

<a name="RC-VERBOSE"></a>

## VERBOSE=
stellt den Loglevel dar. Je höher der Level je mehr Informationen zu den erledigten Aufgaben
findet man im Logfile bzw. auf dem Bildschirm. Feiner getunt über den Spezialswitch _--noscreen_

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

_-e oder --emergency können diese Einstellung überschreiben werden_

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

#-[ GUIDE.md ]-------------------------------------------------------------------------------------

Contens/Verzeichnis

1. [Build from Source](#BUILD)
1. [Base Config](#CONFIG)
1. [Forward a Config](#FORWARD)
1. [After init some daily helps](#DAILY)
1. [Check Database](#DATABASE)
1. [Exclude some Files](#EXCLUDE)

<a name="BUILD">Build from Source</a>

```
@> git clone https://github.com/SirHesti/hsguard.git
clone to 'hsguard'...
remote: Enumerating objects: 193, done.
[etc]
@> cd hsguard
@> cmake .
```

Maybe an Error occured like __"SQLite3 not found"__ . sqlite3(dev) is missing. In this case enter
```
sudo apt install sqlite3 libsqlite3-dev
password: ***********
```
to solve this problem. run "cmake ." again!

```
@> make all
-- The C compiler identification is GNU 12.2.0
-- Detecting C compiler ABI info
[etc]
-- Build files have been written to: /@/hsguard
@> sudo make install
password: ***********
[ 88%] Built target hsguard
[100%] Built target man
Install the project...
-- Install configuration: ""
-- Installing: /usr/bin/hsguard
-- Installing: /usr/share/man/man8/hsguard.8.gz
-- Installing: /usr/share/man/de/man8/hsguard.8.gz
```
we're done.

<a name="CONFIG">Configure hsguard</a>

To run hsguard, a configuration is required. Simply let hsguard do this as follows:
```
@>hsguard
[DATE] /etc/hsguard.rc not exists ....
@>sudo hsguard --create
password: ***********
Sure you want to create /etc/hsguard.rc (y/n) ? Y
Create Dir in /var/hsguard (y/n) ? Y
generated /etc/hsguard.rc
```

Now see the config like:

```
DATABASE=/var/hsguard/hsguard.db
BASE=/
#ROOT/FAIL/USER/INFO/DEVL
VERBOSE=USER
#If desired, an empty file can be created with --createexcl
#EXCLUDE=/var/hsguard/exclude.lst
NOSCREEN=Y
EMERGENCY=PANIC
ANSICOLOR=Y
COL_* can be delete. This is only necessary if you want to become an absolute pro.
```

That's all fine, but _BASE_ need your Attention. Enter here your Datapath. For a Test use an exists path:
```
BASE=/usr/share/keyrings
```
We need also a Datebase. Enter following:
```
@>hsguard --initdb
Sure you want to create /var/hsguard/hsguard.db (y/n) ? Y
```
Now run an update:
```
@>hsguard -u
```
This take a short time to see (real colored) what we have just see: 
```
@>hsguard -l
 8 1 debian-archive-bookworm-automatic.gpg            0x290e8fa0    8 KB 2023-03-18 15:53:41
 6 1 debian-archive-bookworm-security-automatic.gpg   0x320bd4fc    9 KB 2023-03-18 15:53:41
[etc]
```


<a name="FORWARD">Forward your Config</a>

In some cases, the configuration may have been saved somewhere else entirely. All options
can be overridden with switches. hsguard can use a completely different file than the one
in /etc with '--config <file>'. A "real" forward can be entered in /etc/hsguard.

Just write in /etc/hsguard.rc:
```
FORWARD=/srv/admin/configs/hsguard.rc
```
and now hsguard continue the config from here.

<a name="DAILY">After init some helps</a>

After all the hard work we need a script. Create a systemd-unit if you want. I am can do
this, but only a daily-call is need. Just use /etc/cron.daily and write a little script like:

```
#!/bin/(ba)sh
LOGDIR=/srv/admin/log
function diemsg(){
	rc=$1
	msg=$2
	if [ "$msg" != "" ]; then
		echo "$msg"
	fi
	exit $rc
}
hsguard -u
rc=$?
[ $rc -gt 0 ]  && diemsg 31 "Error[$rc] while hsguard update ${i}"
hsguard --testdb 10,10GB
rc=$?
if [ $rc -eq 0 ]; then
	hsguard --testdb r,100,100GB
	rc=$?
fi
#mailmsg not included 99 may mailed
[ $rc -ge 99 ] && diemsg 99 "PANIC - Error [$rc] while hsguard testdb"
[ $rc -gt 0 ]  && diemsg $rc "Error[$rc] while hsguard testdb"
```

if you got a PANIC Error use the LOG-File to see what exactly happened. In this
case i wish you 'good luck'. Hope this a concurrent access.

<a name="DATABASE">Check Database</a>

``` 
-l --list     print out database contens
   --listdir  print only dirs from database
              -r=deep can go deeper '-r' lists everything
-i --info     Info about file in path <entrie> in database
              path like /name/too
              path without starting '/' will find with '*' case intensive
   --find     same as info, but finddir is not implemented
   --infodir  Info about file in dir <places> in database           
```
Something can be listed with -l. You can specify a directory.
```
@>hsguard -l
    9     1 journald.conf        0x3532257f    1 KB 2022-08-07 15:25:09
    5     1 logind.conf          0x6c8caa63    1 KB 2022-08-07 15:25:09
    4     1 networkd.conf        0xd7c3b869     609 2021-02-02 16:29:47
    2     1 pstore.conf          0x138466ad     529 2021-02-02 16:29:47
    6     1 resolved.conf        0x1c561f03     943 2022-08-07 15:25:09
    8     1 sleep.conf           0x7b985243     790 2021-02-02 16:29:47
    7     1 system.conf          0xb9fbab51    2 KB 2022-08-07 15:25:09
    1     1 timesyncd.conf       0x26972bdc     695 2022-12-09 09:37:20
    3     1 user.conf            0x52dff3f9    1 KB 2022-08-07 15:25:09
    2 /etc/systemd/network                               2025-06-17 12:48:27
    3 /etc/systemd/system                                2025-06-17 12:48:27
   22 /etc/systemd/user                                  2025-06-17 12:48:30
```
Beware the switch _-r_ . You list recursive all yourfile in __BASE__.

\-\-listdir will only dirs list. this is the _places_ in the database.
```
@>hsguard --listdir
    2 /etc/systemd/network                               2025-06-17 12:48:27
    3 /etc/systemd/system                                2025-06-17 12:48:27
   22 /etc/systemd/user                                  2025-06-17 12:48:30
```
Attentive observers will notice that the output is identical. The exception is
that only the directories are listed.

In other cases you will find a file. You can enter it with fileglobbing. Note
that the shell does not evaluate the asterisk, etc. Use single quotes.
```
@>hsguard -i 'login*'
ID       = 5
location = (1) /etc/systemd 2025-06-17 12:48:26
fname    = logind.conf
fsize    = 1145
fmtime   = 2022-08-07 15:25:09
crc32    = 0x6c8caa63
chkcount = 1
lastchk  = 2025-06-17 12:48:27
founded  = 2025-06-17 12:48:27
```
This output is real full-colored. All Files will list only once.

<a name="EXCLUDE">Exclude some Files</a>

our _BASE_ has been moved to /etc/systemd

There a more than 2 Idea's to exclude some Files. Some Files may changed daily or else.
But that should not be reason for this consideration. We will not have them in de Database.

First we have a switch to execlude like:
```
@>hsguard -u --exclude_add '*logind.conf'
```
_/etc/systemd/logind.conf_ are excluded from update. just type
```
@>hsguard -l
```
to see the result. if you have many files this. However, if there are many files,
things can quickly become confusing.

in the config you can an entry to create an exclude file. Remove the Comment-mark.
```
#If desired, an empty file can be created with --createexcl
EXCLUDE=/var/hsguard/exclude.lst
```
Now the file does not exists.
```
[DATE] exclude not found: /var/hsguard/exclude.lst; maybe using --createexcl
```
Create an sample file.

```
@>hsguard --createexcl
Sure you want to create /var/hsguard/exclude.lst (y/n) ? Y
please open with editor of your choice and view: /var/hsguard/exclude.lst
```
Ther are god stuff to begin, but we need have the BASE to '/etc/systemd'. Remove
all lines and write only:
```
/etc/systemd/logind.conf
/etc/*logind.conf
/etc/systemd/logind.*
```
Its not all the same, but in this case every line excludes: _/etc/systemd/logind.conf_

You can add excludes if [not] conditions true. So an 'ifhost dcyqx-wkst' will execute
next token. if the token an file then that will excluded or next ifxxxx will checked.
You can add a Message if you like.
```
ifhost WK-Reception message "Hope nobody here"
```
You can find some info more [here in the Readme](README.md#EXCLUDEFILE)

<a name="HESTI">I was here</a>


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
 ** 16.06.25 HS Dokumentation überarbeitet (Übersetzung vorgesehen)
 ** 17.06.25 HS English <-> German
 ** 17.06.25 HS guide.md (english only)

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
