APP = tomato

CC     = gcc
CFLAGS = 
CLIBS  = -lncurses
RM     = rm -f

PREFIX = ${HOME}/usr/local

default: tomato

tomato: tomato.c config.h
	$(CC) $(CFLAG) -o ${APP} tomato.c $(CLIBS)

config.h:
	cp config.def.h $@

clean:
	$(RM) ${APP}

install: tomato
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${APP} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${APP}

uninstall: 
	rm -f $(DESTDIR)$(PREFIX)/bin/${APP}

