[![Build Status](https://travis-ci.org/atsb/worklog.svg?branch=master)](https://travis-ci.org/atsb/worklog)

INTRODUCTION
------------

Worklog is a program that helps you keep track of your time.  I wrote
this program because I have several projects that require me to keep a
timesheet.  I sometimes spend 5 minutes here and there doing
miscellaneous administrative tasks.  Rather than invoke an editor on a
timesheet file for every little segment of my time spent doing these
tasks, I wanted to leave a clock running and indicate with one or two
keystrokes the onset or change of any given task.  Worklog is a simple
ncurses based program to do just that.

INSTALLATION
------------

The home FTP site for worklog is ftp://ftp.truxton.com/pub/
You should also be able to get the latest version at sunsite.unc.edu
or via the Linux Software Map.

Before compiling worklog you need to have the ncurses package.  You can
obtain ncurses from prep.ai.mit.edu in the /pub/gnu directory, or from
ftp://ftp.truxton.com/pub/ncurses-4.2.tar.gz

Optionally modify the default log and config file #defines at the top
of worklog.c.

To compile worklog, issue the command :
	make

To install worklog and its manpage in /usr/local/[bin,man/man1] :
	make install

Or, if your man-package supports gzipped man pages :
	make install.zipman

To test worklog with the example projects file :
	worklog project

Then look at the time.log* files created.

THE PROJECT CONFIGURATION FILE
------------------------------

The project configuration file is a text file.  Each project name and
key-character is listed on a seperate line in the format

<key>:<name>

For example, consider the following 4 lines :

B:BOWER consultation
L:LIS consultation
R:Research
r:Read NetNews

It should be noted that the order of appearance in worklog of the
projects listed in the configuration file is REVERSED.

The key of each project is used to construct a separate log file,
made unique by appending .X to the main log filename, where X is the
key character.  Usually keys are case-insensitive, however, if you
have a key in upper AND lower case, case matters.

RUNNING WORKLOG
---------------

I like to use an alias like this :
alias wl='worklog $HOME/logs/worklog.projects $HOME/logs/worklog.time.log'

You should be able to resize your xterm and have worklog automatically
adapt to the new scren size, however xterm sometimes does not send
SIGWINCH so it may be necessary to force a redraw by pressing control-L.

MISCELLANY
----------

Worklog was written by Truxton King Fulton II.

I hereby place worklog into the public domain.  You can do whatever you
want with this program.  I assume no responsibility for it.

The most current version of worklog can be obtained at :
	http://www.truxton.com/~trux/software/worklog.tar.gz

If you wish to contact me about worklog, please use the address :
	truxton@truxton.com

Thanks to Tim Newsome <drz@cybercomm.net> for improvements incorporated
in version 1.3.

Thanks to Egil Kvaleberg <egilk@sn.no> and
Mark Sebastian Fischer <Mark.Sebastian.Fischer@horz.technopark.gmd.de>
for improvements incorporated in version 1.4.

Thanks to Mike Butler <mgb@mitre.org> for suggesting the "categorize
afterwards" feature incorporated in version 1.5.

Thanks to Vaiva Bichnevicius <vaiva@vaiva.com> for suggesting the
persistant state time feature.

Thanks to Gary <glory@cnsp.com> for suggesting support for window
resizing.

Enjoy...
