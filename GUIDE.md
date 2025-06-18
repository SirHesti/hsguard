---
title: QuickStart & Compile-Guide
---
<a name="HOME"></a>

[README / Man-Page](README.md)

Contens/Verzeichnis

1. [Build from Source](#BUILD)
1. [Base Config](#CONFIG)
1. [Forward a Config](#FORWARD)
1. [After install/init Daily Workshop](#DAILY)
1. [Check Database](#DATABASE)
1. [Exclude some Files](#EXCLUDE)

<a name="BUILD">Build from Source</a>

```sh
@> git clone https://github.com/SirHesti/hsguard.git
clone to 'hsguard'...
remote: Enumerating objects: 193, done.
[etc]
@> cd hsguard
@> cmake .
```
Maybe an Error occured like __"SQLite3 not found"__ . sqlite3(dev) is missing. In this case enter
```sh
sudo apt install sqlite3 libsqlite3-dev
password: ***********
```
to solve this problem. run "cmake ." again!
```sh
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
```sh
@>hsguard
[DATE] /etc/hsguard.rc not exists ....
@>sudo hsguard --create
password: ***********
Sure you want to create /etc/hsguard.rc (y/n) ? Y
Create Dir in /var/hsguard (y/n) ? Y
generated /etc/hsguard.rc
```
Now see the config like:
```ini
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
```ini
BASE=/usr/share/keyrings
```
We need also a Database. Enter following:
```sh
@>hsguard --initdb
Sure you want to create /var/hsguard/hsguard.db (y/n) ? Y
```
Now run an update:
```sh
@>hsguard -u
```
This take a short time to see (real is colored) what we have just see: 
```sh
@>hsguard -l
 8 1 debian-archive-bookworm-automatic.gpg            0x290e8fa0    8 KB 2023-03-18 15:53:41
 6 1 debian-archive-bookworm-security-automatic.gpg   0x320bd4fc    9 KB 2023-03-18 15:53:41
[etc]
```
The [README](#HOME) has more Information.

<a name="FORWARD">Forward your Config</a>

In some cases, the configuration need saved somewhere else. All options
can be overridden with switches. hsguard can use a completely different file than the one
in /etc with '--config <file>'. A "real" forward can be entered in /etc/hsguard.rc

Just write in /etc/hsguard.rc:
```ini
FORWARD=/srv/admin/configs/hsguard.rc
```
and now hsguard continue the config from here.

<a name="DAILY">Daily Workshop</a>

After all the hard work we need a script. Create a systemd-unit if you want. I can do
this, but only a daily-call is need. Just use /etc/cron.daily (systemd sign over
crontab anyway) and write a little script:
```sh
#!/bin/sh
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
case i wish you 'good luck'. Hope only a concurrent access.

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
The standard option for listings is the -l option. You can, but don't have to, specify a directory.
```sh
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

\-\-listdir wird nur Verzeichnisse aufzeigen. This is the _places_ in the database.
```sh
@>hsguard --listdir
    2 /etc/systemd/network                               2025-06-17 12:48:27
    3 /etc/systemd/system                                2025-06-17 12:48:27
   22 /etc/systemd/user                                  2025-06-17 12:48:30
```
Attentive observers will notice that the output is identical. The exception is
that only the directories are listed.

In other cases you need to find a file. You can do it with fileglobbing. Note
that the shell does not evaluate the '*', etc. Use single quotes.
```sh
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

First we have a switch to exclude like:
```sh
@>hsguard -u --exclude_add '*logind.conf'
```
_/etc/systemd/logind.conf_ are excluded from update. just type
```sh
@>hsguard -l
```
to see the result. if you have many files this. However, if there are many files,
things can quickly become confusing.

in the config you can an entry to create an exclude file. Remove the Comment-mark.
```ini
#If desired, an empty file can be created with --createexcl
EXCLUDE=/var/hsguard/exclude.lst
```
Now the file exclude.lst does not exists.
```sh
[DATE] exclude not found: /var/hsguard/exclude.lst; maybe using --createexcl
```
Create an sample file.
```sh
@>hsguard --createexcl
Sure you want to create /var/hsguard/exclude.lst (y/n) ? Y
please open with editor of your choice and view: /var/hsguard/exclude.lst
```
There are good stuff to begin, but we need have the BASE to '/etc/systemd'. Remove
all lines and write only:
```sh
/etc/systemd/logind.conf
/etc/*logind.conf
/etc/systemd/logind.*
```
Its not all the same, but in this case every line excludes: _/etc/systemd/logind.conf_ .

You can add excludes if [not] conditions true. So an 'ifhost dcyqx-wkst' will execute
next token. if the token an file then that will excluded or next ifxxxx will checked.
You can add a Message if you like.

Exclusions can also be made dependent on certain conditions being met. For
example, 'ifhost dcyqx-wkst' will execute the next token. The next token can
again be a condition or an exclude.

A message can also be sent instead of the exclude.
```sh
ifhost WK-Reception message "Hope nobody here"
```
You can find some info more

[here in the Readme](README.md#EXCLUDEFILE)
 

<!-- a name="HESTI">I was here</a -->


