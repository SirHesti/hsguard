---
title: hsguard
section: 8
header: hsguard - coded by Hesti
footer: hsguard 2.24.59 Beta
date: 15.06.2025
---

### NAME
**hsguard** - Programm um Aenderungen im Dateisystem festzustellen

### SYNOPSIS

About

hsguard stellt aus den konfigurierbaren Verzeichnisse eine Datenbank
zusammen. Dabei wird jede Datei für sich geprüft und eine CRC-Prüfsumme
(OLDSTYLE) erstellt. Ist die Festplatte erst einmal in der Datenbank,
dann kann jede Datei auf (nicht gewollte) Änderungen geprüft werden.

Genau das ist der Zweck von hsguard. Festplatten (HDD) altern und wenn
Daten nicht gelesen werden, dann ‘vergisst’ die Festplatte mal ein Bit.

Bei mir laufen HDD’s rund um die Uhr als Backupsystem, lokales
Filesharing etc.. Daten auf diesen Platten möchte ich auch nach vielen
Jahren behalten.

Zusätzlich kann so erkannt werden, ob eine Malware gerade dabei ist
sämtliche Daten zu verschlüsseln.

Hier greift “hsguard”. hsguard speichert alle wichtigen Informationen zu
einer Datei. Eine Datei die geändert wurde, wird in der Datenbank normal
geupdatet. Über jede Datei wird eine CRC32 gebildet. Die Prüfsumme wird
pro Datei gespeichert und kann gegengeprüft werden.

Es wird Position, Name, FileSize, FileModifDate und die CRC32
gespeichert. Prüfcounter und Aufnahmedatum können aus der Datenbank
ebenfalls abgerufen werden.

Im CronJob kann hsguard [–update] –check aufgerufen werden, wo
verschiedene Dateien dann gegengeprüft werden können. Diese werden im
Zufallsprinzip ausgewählt. Ein Update nimmt neue oder geänderte Dateien
vorher auf.

Mit vielen Möglichkeiten können die Daten aufgelistet und verifiziert
werden. So wird im Fall des Falles die Datenbank eine hilfreiche
Ergänzung sein.

WICHTIG! hsguard wird keine Festplatteninhalte ändern. Keine Datei
löschen oder hinzufügen. Die einzige Ausnahme ist die Datenbank selsbt.
“hsguard –create” wird einmalig eine hsguard.rc anlegen. Diese Datei
wird nicht überschrieben. Alle Parameter können aber auch auf der
Kommandozeile angegeben werden.

hsguard benötigt lediglich Leserechte für die Daten.

hsguard kann auf vielfältig configuriert werden. Configdatei,
Excludedatei und Kommandozeilenswitch.

Wird hsguard das erste Mal gestartet, dann wird die Datei
/etc/hsguard.rc fehlend bemängelt. Mit “hsguard –create” kann eine erste
Version mitsamt einen Unterverzeichnis /var/hsguard erstellt werden. Ein
Beispiel kann in der Datei hsguard_sample.rc nachgeschlagen werden.

### DESCRIPTION
hsguard liest das Dateisystem ein und speichert rudimentäre Daten.
Sofern nicht vorhanden/geändert(fsize/mtime) wird eine CRC32 der Datei
gebildet. Diese wird gegengeprüft und ggf. Alarm geschlagen.

### OPTIONS

 - $${\color{red}$$-u \-\-update $${\colorend$$              <p>>update database</p>
 - $${\color{red}$$-\-rescan $${\colorend$$                  <p>>update all, but reread crc32</p>
 - $${\color{red}$$-n \-\-nodirtest $${\colorend$$           <p>>update will _not_ scan for empty dirs otherwise no exists\
                                    dirs(and entries) will remove from database</p>
 - $${\color{red}$$-\-yes $${\colorend$$                     <p>>don't ask me; i answer yes</p>


 - \color{red}-\-help \-? \-h **            <p>>Print this helpmsg</p>
 - \color{red}-d \-\-db sqlite3.db **       <p>>use this "\[PATH/\]sqlite3.db" database **overwrite: [RCdatabase](#RC-DATABASE)**</p>


 - **\-\-help \-? \-h **            <p>>Print this helpmsg</p>
 - **\-d \-\-db sqlite3.db **       <p>>use this "\[PATH/\]sqlite3.db" database **overwrite: [RCdatabase](#RC-DATABASE)**</p>
 - **\-b \-\-base \<dir\> **        <p>>basedir for all operations **overwrite [RCbase](#RC-BASE)**</p>
 - **\-\-create **                  <p>>create an empty ConfigFile highcommand to edit this</p>
 - **\-\-initdb **                  <p>>create an empty Database</p>
 - **\-\-createExcl **              <p>>create an example excludefile</p>
 - **\-x \-\-exclude  \<file\> **   <p>>read a list of excluded files \-\- '#' comment #### **add this list: [RCexclude](#RC-EXCLUDE)**</p>
 - **\-\-exclude_add \<file\> **    <p>>add a single file/dir to excludelist **works too: [RCexadd](#RC-EXADD)**</p>
 - **\-\-ldir/\-\-edir \<dir\> **   <p>>an LS for exclude file writers (NO Function on DB)</p>
 - **\-\-update_deep \<num\> **     <p>>update database only this deep (update will set)</p>
 - **\-\-update_dir \<dir\> **      <p>>update from here into database (update will set)</p>
 - **\-u \-\-update **              <p>>update database</p>
 - **\-\-rescan **                  <p>>update all, but reread crc32</p>
 - **\-n \-\-nodirtest **           <p>>update will _not_ scan for empty dirs otherwise no exists\
                                    dirs(and entries) will remove from database</p>
 - **\-\-yes **                     <p>>don't ask me; i answer yes</p>
 - **\-v \-\-verbose **             <p>>use&nbsp;&nbsp;&nbsp;\-v=1 to silence, also use \-v\- ** overwrite [RCverbose](#RC-VERBOSE)**\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-v=2 erros(default) \-v FAIL is the same\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-v=3 (same \-v) userinfo\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-vv&nbsp;&nbsp;\-v=4 info\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-vvv \-v=5 developer</p>
 - **\-s \-\-noscreen **            <p>>In case of UserCMD's like '\-\-list' make screen for output.\
                                    all UserMsg are saved only in the log. to turn off noscreen use \-s\- **overwrite [RCnoscreen](#RC-NOSCREEN)**</p>
 - **\-a \-\-ansi on/off/\-/\+ **   <p>>Ansicolor at find/info/list \-a\- off \-a=On or only \-a is on\
                                    colors can be tuned in the config file **overwrite: [RCansicolor](#RC-ANSICOLOR)**</p>
 - **\-l \-\-list **                <p>>print out database contens</p>
 - **\-\-listdir **                 <p>>print only dirs from database see: \-\-recursive</p>
 - **\-f \-\-formout **             <p>>Format Out to UserLevel\
                                    Singleline :ONELINE, LONGLINE, MAXLINE\
                                    Multiline: :MINI, BASIC, MEDIUM, ALL</p>
 - **\-r \-\-recursive **           <p>>list subdirs too _\-r=deep_ can go deeper _'\-r'_ lists everything</p>
 - **\-i \-\-info \<dir\>or\<path\>or@\<id\> ** <p>>Info about file in path \<entrie\> in database\
                                    path like /name/too\
                                    path without starting '/' will find with '*' case intensive</p>
 - **\-\-find **                    <p>>same as info, but finddir is not implemented</p>
 - **\-\-infodir \<dir\>or@\<id\> ** <p>>Info about file in dir \<places\> in database</p>
 - **\-\-remove  \<dir\>or\<path\> ** <p>>remove entrie or a complete path(include entries/subdirs) from DB\
                                    you must confirmed (Y/N) (or use \-\-yes)</p>
 - **\-\-finddupes \<dir\> **       <p>>find dupes beginn at \<dir\> or start in base. This can take a while</p>
 - **\-t \-\-testdb \[OPTIONS\] **  <p>>_OPTIONS_ are \[r\],count,\[size\],\[path\], where...\
                                    _\[r\]_&nbsp;&nbsp;&nbsp;&nbsp;optional will activate the random Mode *\
                                    _count_&nbsp;&nbsp;of Files that will checked\
                                    _\[size\]_ optional size that will never reached. You can use 10TB or 500k\
                                    _\[path\]_ optional, if not set then start check from BASE\
                                    * if no random activated, then oldest (from lastchk) will checked</p>
 - **\-e \-\-emergency \[OPTIONS\] ** <p>>PANIC,ABORT=\<errorlevel\>,SKIP,COUNT=\<num\>,UPDATE,UPDATEALL **overwrite: [RCemergency](#RC-EMERGENCY)**\
                                    _PANIC_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Abort all, use Errorlevel 99 to exit\
                                    _ABORT_\[=\<el\>\] : Abort with Errorlevel 1, if \<el\>(2\-98) is set, then el is use\
                                    _SKIP_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Skip this to COUNT, then abort\
                                    _COUNT_=\<num\>&nbsp;&nbsp;: Skip not forever, only for this \<num\>\
                                    _UPDATE_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Update lastchk and chkcount in DB\
                                    _UPDATEALL_&nbsp;&nbsp;&nbsp;&nbsp;: Update size,fmtime,crc32 as found too</p>
 - **\-\-wbs \<size\> **            <p>>Warning big size Files default is 16GB **overwrite: [RCwarnbigsize](#RC-WBS)**</p>

### ENVIRONMENT
siehe Anschnitt: **[hsguard.rc](#hsguard.rc)**

<a name="hsguard.rc"></a>

### /etc/hsguard.rc
<a name="RC-DATABASE"></a>

#### DATABASE=
Die Datenbank selbst. Diese kann sinnvoll unter /var sein. Zusammen mit --initDB
kann diese als Grundkonfig erstellt werden. Dafür muss diese Lese-/Schreibrechte haben.
| \-\-db überschreibt die im RC-File angegebene Datei

<a name="RC-BASE"></a>

#### BASE=
BASIS alles was angegeben wird. Ab wo soll die Datenbank erstellt werden. Diese Zeile sollte
unbedingt angepasst werden. ZB. kann der Pfad zu den schützendwerten
Daten /server/pictures oder ähnlichem.
-b <path> überschreibt das

<a name="RC-VERBOSE"></a>

#### VERBOSE=
stellt den Loglevel dar. Je höher der Level je mehr Informationen zu den erledigten Aufgaben
findet man im Logfile bzw. auf dem Bildschirm. Feiner getunt über den Spezialswitch --noscreen
Verboselevel wird von 0-4 oder besser unterstützt:

 - `ROOT  0 :` Nur Systemfehler
 - `FAIL  1 :` Nur Nachrichten, die Fehler beinhalten
 - `USER  2 :` Standardeinstellung Es werden Informationen zu Start und Ende einer jeden Aktion ausgegeben
 - `INFO  3 :` Das können sehr viele Nachrichten sein
 - `DEVL  4 :` Das sollte man sich besser nicht ansehen, diese sind nur für Entwickler gedacht

<a name="RC-EXCLUDE"></a>

#### EXCLUDE=
Es gibt Ordner die muessen nicht durchsucht werden, zB. TempOrdner oder der Ordner dieser Datenbank.
Die Dateien/Verzeichnisse werden zeilenweise notiert, die mit '?' und '*' auch Mehrfachausshluß zulassen.
Ein '#' am Zeilenanfang ist eine Kommentarzeile. Mehr zu diesem Thema **unter: [Exclude-File](#EXCLUDEFILE)**",

<a name="RC-NOSCREEN"></a>

#### NOSCREEN=
kann N/Y sein. Bei Y werden fast alle Ausgaben nur im Logfile vermerkt und nicht mehr auf dem
Bildschirm ausgeben. Meldungen stehen nur im Logfile zur Verfügung. -s- auf der Kommandozeile kann dieses
Verhalten abschalten. Auch wenn NOSCREEN=Y angegeben wurde. Es wird empfohlen diese Zeile zum Einrichten
auszukommentieren.

<a name="RC-EMERGENCY"></a>

#### EMERGENCY=
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

#### ANSICOLOR=
Viele Ausgaben werden, zur besseren Unterscheidung, farblich unterschiedlich gekennzeichnet.
Y/N kann die Ausgabe von AnsiColors ausschalten. Bei Y können die Farben auch noch variiert werden.
Diese können in der Sample-Datei angesehen werden.

<a name="RC-EXADD"></a>

#### EXADD=

Hiermit kann eine Datei oder auch ein Verzeichnis ignoriert werden. Wesentlich besser geht es mit
[RCexclude](#RC-EXCLUDE).

<a name="RC-WBS"></a>

#### WARNWBS

Dieser Parameter bewirkt das eine "Warnung" ausgegeben wird, wenn ein "großes File" erkannt
wird. Das kann sinnvoll sein, wenn die zu prüfenden Dateien nicht die schnellste Verbindung
haben. Damit der Cursor, nicht wie eingefroren, auf der Stelle anzeigefrei blinkt kann hier
eine Dateigröße einstellt werden. Die Warnung wird auf Userlevel ausgegeben. Die Angabe
erfolgt Humanlike wie 1TB oder 10MB. Auf 0 setzen um dieses Verhalten auszuschalten.
Defaultwert ist 16GB.

<a name="RC-FORWARD"></a>

#### FORWARD

Einmalig kann hier auf eine andere Config verweisen. Das bietet sich an, wenn man andere
Config (z.B.im Netzwerk) verweisen kann. Als Feature ist ganz klar, dass auch die Section
direkt mit angegeben werden kann. Das kann dann so aussehen:
```
FORWARD=/srv/pub/backup.rc,std
```

<a name="EXCLUDEFILE"></a>

##### Exclude Dateien

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
----------|:-----------------
ifhost X  | Wenn der aktuelle PC X ist
ifnhost X | Wenn der aktuelle PC nicht X ist
ifuser X  | Wenn der aktuelle Benutzer X ist
ifnuser X | Wenn der aktuelle Benutzer nicht X ist
ifbase X  | BASE=X (-b=X)
ifnbase X | wenn BASE nicht zutrifft
include X | X includen. Datei X an dieser Stelle einlesen
message X | Diese Nachricht ausgeben


### AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

### FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

