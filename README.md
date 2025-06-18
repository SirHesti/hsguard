---
title: hsguard
section: 8
header: hsguard - coded by Hesti
footer: hsguard 2.24.59 Beta
date: 18.06.2025
---

### NAME
__hsguard__ - Program to detect changes in the file system

### SYNOPSIS

[🇩🇪 german translation 🇩🇪](README-de.md)

[🙋 Quickstart & Compile Guide 🙋](GUIDE.md)

hsguard creates a database from the configurable directories. Each file
is checked individually and a CRC checksum (OLDSTYLE) is generated. Once
the hard drive is included in the database every file can be checked for
(unwanted) changes.

This is precisely the purpose of hsguard. Hard drives (HDDs) age and if
data is not read the drive may “forget” a bit.

I have HDDs running around the clock as backup systems, local file
sharing, etc. I want to keep the data on these drives for many years.

Additionally, this makes it possible to detect if malware is in the
process of encrypting all your data.

This is where “hsguard” comes in. hsguard stores all important
information about a file. A file that has been changed is normally
updated in the database. A CRC32 is calculated for each file. The
checksum is saved per file and can be checked against.

Position, name, file size, file modification date, and the CRC32 are
stored. Check counters and the date of inclusion can also be retrieved
from the database.

In a cron job, hsguard [–update] –check can be called, where different
files can then be checked. These are selected at random. An update adds
new or changed files beforehand.

With many options, the data can be listed and verified. Thus, in case of
an incident, the database will be a helpful supplement.

IMPORTANT! hsguard will not modify the contents of any hard drive. It
will not delete or add any files. The only exception is the database
itself. “hsguard –create” will, once, create a hsguard.rc. This file
will not be overwritten. However, all parameters can also be specified
on the command line.

hsguard only requires read access to the data.

hsguard can be configured in many ways: config file, exclude file, and
command-line switches.

If hsguard is started for the first time, it will complain that the file
/etc/hsguard.rc is missing. With “hsguard –create”, an initial version
can be created, along with a subdirectory /var/hsguard. An example can
be found in the file hsguard_sample.rc.

### DESCRIPTION
hsguard reads the file system and stores basic data.
If a file is not present or has changed (fsize/mtime), a CRC32 checksum is
generated for the file. This checksum is checked and an alert is triggered
if necessary.

### OPTIONS

 - __\-\-help \-? \-h__             <p>Print this helpmsg</p>
 - __\-d \-\-db sqlite3.db__        <p>use this "\[PATH/\]sqlite3.db" database __overwrite: [RCdatabase](#RC-DATABASE)__</p>
 - __\-b \-\-base \<dir\>__         <p>basedir for all operations __overwrite [RCbase](#RC-BASE)__</p>
 - __\-\-create__                   <p>create an empty ConfigFile highcommand to edit this</p>
 - __\-\-initdb__                   <p>create an empty Database</p>
 - __\-\-createExcl__               <p>create an example excludefile</p>
 - __\-x \-\-exclude  \<file\>__    <p>read a list of excluded files \-\- '#' comment #### __add this list: [RCexclude](#RC-EXCLUDE)__</p>
 - __\-\-exclude_add \<file\>__     <p>add a single file/dir to excludelist __works too: [RCexadd](#RC-EXADD)__</p>
 - __\-\-ldir/\-\-edir \<dir\>__    <p>an LS for exclude file writers (NO Function on DB)</p>
 - __\-\-update_deep \<num\>__      <p>update database only this deep (update will set)</p>
 - __\-\-update_dir \<dir\>__       <p>update from here into database (update will set)</p>
 - __\-u \-\-update__               <p>update database</p>
 - __\-\-rescan__                   <p>update all, but reread crc32</p>
 - __\-n \-\-nodirtest__            <p>update will _not_ scan for empty dirs otherwise no exists\
                                    dirs(and entries) will remove from database</p>
 - __\-\-yes__                      <p>don't ask me; i answer yes</p>
 - __\-v \-\-verbose__              <p>use&nbsp;&nbsp;&nbsp;\-v=1 to silence, also use \-v\-__  overwrite [RCverbose](#RC-VERBOSE)__\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-v=2 erros(default) \-v FAIL is the same\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-v=3 (same \-v) userinfo\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-vv&nbsp;&nbsp;\-v=4 info\
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\-vvv \-v=5 developer</p>
 - __\-s \-\-noscreen__             <p>In case of UserCMD's like '\-\-list' make screen for output.\
                                    all UserMsg are saved only in the log. to turn off noscreen use \-s\- __overwrite [RCnoscreen](#RC-NOSCREEN)__</p>
 - __\-a \-\-ansi on/off/\-/\+__    <p>Ansicolor at find/info/list \-a\- off \-a=On or only \-a is on\
                                    colors can be tuned in the config file __overwrite: [RCansicolor](#RC-ANSICOLOR)__</p>
 - __\-l \-\-list__                 <p>print out database contens</p>
 - __\-\-listdir__                  <p>print only dirs from database see: \-\-recursive</p>
 - __\-f \-\-formout__              <p>Format Out to UserLevel\
                                    Singleline :ONELINE, LONGLINE, MAXLINE\
                                    Multiline: :MINI, BASIC, MEDIUM, ALL</p>
 - __\-r \-\-recursive__            <p>list subdirs too _\-r=deep_ can go deeper _'\-r'_ lists everything</p>
 - __\-i \-\-info \<dir\>or\<path\>or@\<id\>__  <p>Info about file in path \<entrie\> in database\
                                    path like /name/too\
                                    path without starting '/' will find with '*' case intensive</p>
 - __\-\-find__                     <p>same as info, but finddir is not implemented</p>
 - __\-\-infodir \<dir\>or@\<id\>__  <p>Info about file in dir \<places\> in database</p>
 - __\-\-remove  \<dir\>or\<path\>__  <p>remove entrie or a complete path(include entries/subdirs) from DB\
                                    you must confirmed (Y/N) (or use \-\-yes)</p>
 - __\-\-finddupes \<dir\>__        <p>find dupes beginn at \<dir\> or start in base. This can take a while</p>
 - __\-t \-\-testdb \[OPTIONS\]__   <p>_OPTIONS_ are \[r\],count,\[size\],\[path\], where...\
                                    _\[r\]_&nbsp;&nbsp;&nbsp;&nbsp;optional will activate the random Mode *\
                                    _count_&nbsp;&nbsp;of Files that will checked\
                                    _\[size\]_ optional size that will never reached. You can use 10TB or 500k\
                                    _\[path\]_ optional, if not set then start check from BASE\
                                    * if no random activated, then oldest (from lastchk) will checked</p>
 - __\-e \-\-emergency \[OPTIONS\]__  <p>PANIC,ABORT=\<errorlevel\>,SKIP,COUNT=\<num\>,UPDATE,UPDATEALL __overwrite: [RCemergency](#RC-EMERGENCY)__\
                                    _PANIC_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Abort all, use Errorlevel 99 to exit\
                                    _ABORT_\[=\<el\>\] : Abort with Errorlevel 1, if \<el\>(2\-98) is set, then el is use\
                                    _SKIP_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Skip this to COUNT, then abort\
                                    _COUNT_=\<num\>&nbsp;&nbsp;: Skip not forever, only for this \<num\>\
                                    _UPDATE_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: Update lastchk and chkcount in DB\
                                    _UPDATEALL_&nbsp;&nbsp;&nbsp;&nbsp;: Update size,fmtime,crc32 as found too</p>
 - __\-\-wbs \<size\>__             <p>Warning big size Files default is 16GB __overwrite: [RCwarnbigsize](#RC-WBS)__</p>

### ENVIRONMENT
see section: __[hsguard.rc](#hsguard.rc)__

<a name="hsguard.rc"></a>

### /etc/hsguard.rc
<a name="RC-DATABASE"></a>

#### DATABASE=
The database itself. This can usefully be located under /var. Together with --initDB,
this can be created as a basic configuration. For this, it must have read/write
permissions.
_\-\-db overrides the file specified in the RC file_

<a name="RC-BASE"></a>

#### BASE=
BASE for everything specified. Where should the database be created from? This line
should definitely be adjusted. For example, the path to the data to be protected
could be /server/pictures or similar.
_\-b <path> overrides this_

<a name="RC-VERBOSE"></a>

#### VERBOSE=
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

#### EXCLUDE=
There are folders that do not need to be searched, e.g., temp folders or the folder
of this database. The files/directories are listed line by line, and with '?'
and '*' you can exclude multiple items. A '#' at the beginning of a line is a
comment. More on this topic __under: [Exclude-File](#EXCLUDEFILE)__

<a name="RC-NOSCREEN"></a>

#### NOSCREEN=
Can be Y/N. If Y, almost all output is only written to the log file and no longer
displayed on the screen. Messages are only available in the log file. -s- on the
command line can disable this behavior, even if NOSCREEN=Y is specified. It is
recommended to comment out this line when setting up.

<a name="RC-EMERGENCY"></a>

#### EMERGENCY=
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

#### ANSICOLOR=
Many outputs are color-coded for better distinction.
Y/N can disable the output of ANSI colors. If Y, colors can also be varied further.
These can be viewed in the sample file.

<a name="RC-EXADD"></a>

#### EXADD=

Here you can ignore a file or even a directory. It’s much better to
use [RCexclude](#RC-EXCLUDE).

<a name="RC-WBS"></a>

#### WARNWBS

This parameter triggers a "warning" when a "large file" is detected.
This can be useful if the files to be checked are not connected via the fastest
connection. To prevent the cursor from appearing frozen while waiting, a file size
can be set here. The warning is issued at user level. Specify in human-readable
format like 1TB or 10MB. Set to 0 to disable this behavior.

Default value is 16GB.

<a name="RC-FORWARD"></a>

#### FORWARD

This can be used once to refer to another config. This is useful if you can refer
to other configs (e.g., on the network). As a feature, the section can be specified
directly. This might look like:

```
FORWARD=/srv/pub/backup.rc,std
```

<a name="EXCLUDEFILE"></a>

##### Exclude Files

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


### AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

### BUGS
Please report bugs (including in this manpage)

