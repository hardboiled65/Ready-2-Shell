Ready 2 Shell
=============
Scanning not installed unix shell commands that personal needed.

About
-----
Ready 2 Shell is the simple tool for scanning shell commands you use for multiple environment.
It is useful when you switch to different machine (e.g. Change computer, re-install the operating system, setting up new vm etc.) and to know which packages should be installed.

Features
--------
* Make a list that shell commands you use
* Scan the list, then shows you which commands are not installed in the system
* Filtered as a priority
* ~~The output file can using after for install packages in batch~~ (Not supported yet)
* ~~Analyze shell history file for which commands are mostly used~~ (Not supported yet)

Installation
------------
We use [SCons](http://scons.org) for build project. You should install SCons before build Ready 2 Shell.
For install this,
### Ubuntu (or other Debian based systems)
```
$ sudo apt-get install scons
```
For build,
```
$ scons
$ sudo cp r2sh /usr/local/bin
```

Usage
-----
Ready 2 Shell will create a file named **.r2shlist** in your home directory first time.
```
$ r2sh -A -c gcc -p 0 -d "gnu c compiler"
```
this add(-A) a command(-c) *gcc*, important priority(-p) with description(-d).

If *gcc* is not installed in your system, below command prints like,
```
$ r2sh
[gcc]	[0]	[gnu c compiler]	-->  [NOT INSTALLED]
$ 
```
By default, it only prints important and normal priority commands.
You can manually select priority by using options like **-i**, **-n** or **-x**, each options will prints important, normal or extra respectively.
Or **-a** option will prints all priorities.

License
-------
Ready 2 Shell is developed in MIT License. For the detail, see the LICENSE file.
