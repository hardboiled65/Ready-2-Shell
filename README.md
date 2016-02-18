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
```sh
$ sudo apt-get install scons
```
For build,
```sh
$ scons
$ sudo cp r2sh /usr/local/bin
```

Usage
-----
![demo](https://raw.githubusercontent.com/hardboiled65/Ready-2-Shell/master/doc/r2shdemo.gif)

Ready 2 Shell will create a file named **.r2shlist** in your home directory first time.
### Print
```sh
$ r2sh
```
Without any options, it prints important and normal priority commands that not installed on this system.

For specific priorities, combine below options.
* **-i** important
* **-n** normal
* **-x** extra

Or for all, use this.
* **-a** all

For an example,
```sh
$ r2sh -i -n
```
is same as default.

### Modify
For add a command, use **-A** option. e.g.,
```sh
$ r2sh -A -c gcc -p 0 -d "gnu c compiler"
```
this add a command(-c) *gcc*, important priority(-p) with description(-d).
It will asks you the omitted part, so you can simply use **-A** option only.

For modifying, use **-M** option with specific command name(-c).
```sh
$ r2sh -M -c vim
command: (vim)
priority [0: important / 1: normal / 2: extra]: (0) 1
description: (Vi IMproved) must have editor
[vim] [1] [must have editor]
$ 
```

You can delete an item with **-D** option.
```sh
$ r2sh -D
command: emacs
deleted: [emacs] [1] [lisp interpreter]
$ 
```

License
-------
Ready 2 Shell is developed in MIT License. For the detail, see the LICENSE file.
