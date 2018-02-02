VERSION = 1.9
CFLAGS=-02 -DVERSIONS=\"${VERSION}\" -D_FORTIFY_SOURCE=2 -g -Wall -Wextra -Wpointer-arith -Wuninitialized -Wshadow -Winit-self -Wmissing-declarations -Wformat -Wformat-security -Werror=format-security
LIBS=-lncurses -lm -z now
BIN=$(DESTDIR)/usr/bin
MAN=$(DESTDIR)/usr/share/man/man1

# uncomment this to allow use of an environment variable to specify the
# default working directory where default log and project files are located
# and they can be #defined as relative paths in worklog.c
# DEFAULT_DIR_ENVVAR=HOME

DEFAULT_DIR_ENVVAR=WORKLOGDIR
USE_ENV_DIR=-DDEFAULT_DIR_ENVVAR=\"$(DEFAULT_DIR_ENVVAR)\"


all:	worklog Makefile

install:	all
	cp worklog $(BIN)/worklog
	chmod a+rx $(BIN)/worklog
	#cp worklog.1 $(MAN)/man1/worklog.1
	#chmod a+r $(MAN)/man1/worklog.1

install.zipman: install
	#gzip $(MAN)/man1/worklog.1

worklog:	worklog.c Makefile
	gcc $(CFLAGS) $(USE_ENV_DIR) worklog.c $(LIBS) -o worklog

clean:
	rm -f worklog

tar:
	- rm -rf worklog-${VERSION}
	mkdir worklog-${VERSION}
	cp Makefile README TODO worklog.c worklog.1 worklog.lsm projects worklog-${VERSION}
	cp worklog.lsm distr/worklog-${VERSION}.lsm
	tar -clzvf distr/worklog-${VERSION}.tar.gz worklog-${VERSION}
	- rm -rf worklog-${VERSION}
