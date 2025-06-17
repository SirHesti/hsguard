
Contens/Verzeichnis

1. [Build from Source](#BUILD)
1. [Base Config](#CONFIG)
1. [Forward a Config](#FORWARD)
1. [After init some daily helps](#DAILY)
1. [Check Database][#DATABASE]

<a name="BUILD">Build from Source</a>

```
@> git clone https://github.com/SirHesti/hsguard.git
clone to 'hsguard'...
remote: Enumerating objects: 193, done.
[etc]
@> cd hsguard
@> cmake .
```

Maybe an Error occured __"SQLite3 not found"__ if sqlite3(dev) missing. Then enter
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

To run hsguard, configuration is required. Simply let hsguard do this as follows:

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

That's all fine, but _BASE_ need your Attension. Enter your Datapath. For a Test use an exists path:
```
BASE=/usr/share/keyrings
```
We need also a Datebase Just Run it:
```
@>hsguard --initdb
Sure you want to create /var/hsguard/hsguard.db (y/n) ? Y
```
Now run an update:
```
@>hsguard -u
```
This take time if your have many ...

to see (real colored) what we have just type: 
```
@>hsguard -l
 8 1 debian-archive-bookworm-automatic.gpg            0x290e8fa0    8 KB 2023-03-18 15:53:41
 6 1 debian-archive-bookworm-security-automatic.gpg   0x320bd4fc    9 KB 2023-03-18 15:53:41
[etc]
```
Now see the [README](README.md)

<a name="FORWARD">Forward your Config</a>

In some cases, the configuration may have been saved somewhere else entirely. All options
can be overridden with switches. hsguard can use a completely different file than the one
in /etc with '--config <file>'. A "real" forward can be entered in /etc/hsguard.

Just write in /etc/hsguard.rc:
```
FORWARD=/srv/admin/configs/hsguard.rc
```
and now hsguard read there config from here.

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

if you got a PANIC Error use the LOG to see what happens. In this case
i wish you 'good luck'. Hope this a concurrent access.

<a name="DATABASE">Check Database</a>



