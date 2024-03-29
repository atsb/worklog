VERSION = 2.1
CFLAGS=-DVERSIONS=\"${VERSION}\" -D_FORTIFY_SOURCE=2 -O -Wall -Wformat -Wformat-security -Werror=format-security
LIBS=-lncurses -lm -z now
BIN=$(DESTDIR)/usr/bin
MAN=$(DESTDIR)/usr/share/man/man1

# uncomment this to allow use of an environment variable to specify the
# default working directory where default log and project files are located
# and they can be #defined as relative paths in worklog.c
DEFAULT_DIR_ENVVAR=HOME
#DEFAULT_DIR_ENVVAR=WORKLOGDIR
USE_ENV_DIR=-DDEFAULT_DIR_ENVVAR=\"$(DEFAULT_DIR_ENVVAR)\"


all:	worklog

install:	all
	install -D -m 0755 worklog $(BIN)/worklog
	install -D -m 0644 worklog.1.gz $(MAN)/man1/worklog.1.gz

worklog:	worklog.c Makefile
	$(CC) $(CFLAGS) $(USE_ENV_DIR) worklog.c $(LIBS) -o worklog

clean:
	rm -f worklog

dist:
	- rm -rf worklog-${VERSION}
	mkdir worklog-${VERSION}
	cp Makefile README TODO worklog.c worklog.1.gz worklog.lsm projects worklog-${VERSION}
	cp worklog.lsm distr/worklog-${VERSION}.lsm
	tar -clzvf distr/worklog-${VERSION}.tar.gz worklog-${VERSION}
	- rm -rf worklog-${VERSION}
