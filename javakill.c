/*
 * $Author: vince $
 * $Header: /users/vince/src/sock/RCS/javakill.c,v 1.18 1996/10/07 14:41:47 vince Exp vince $
 * $Date: 1996/10/07 14:41:47 $
 * $Revision: 1.18 $
 * $Locker: vince $
 */

/*
This is a very quick and dirty hack to get rid of all Java/JavaSCRIPT
stuff from your network.  It acts like a proxy server.  It should be
placed somewhere betweeen your browser and your real proxy server:

   Browsers          --> Javakill -->  Proxy-server --> Internet
(on any internal host)  (on host J)    (on host F)

Install Javakill on host J (any host of your network).  Make it
run on any non-privileged port.  Port 23456 can be a good choice.

Javakill requires one single argument, that is F:PORTNUMBER,
where F is the host the real proxy server is running on and
PORTNUMBER is the port number the real proxy server is listening
to.  Proxy servers usually reside on special service hosts and
listen to ports like 8080, 81, 8000, 3128 et cetera.

Set your internal browser in order to make use of Javakill
as proxy server.


INSTALLATION
Javakill should be invoked by inted.  
/etc/services and /etc/inetd.conf should be edited accordingly:

/etc/services:
javakill        23456/tcp

/etc/inetd.conf:
javakill  stream  tcp nowait  nobody  /etc/javakill javakill F:portnumber



COMPILATION
gcc -DINETD javakill.c    (for most systems)
gcc -DSOLARIS -DINETD javakill.c -lsocket -lnsl    (for solaris boxes)


DISCLAIMER
This software is completely unsupported. Use at your own risk.
This software assumes that you have a level of expertise as
a systems manager that will allow youu to install this properly.
If you do not, don't use this software.

--vince       
vince@cryptonet.it
*/

#ifndef	lint
static char *rcsid = "@(#) $Id: javakill.c,v 1.18 1996/10/07 14:41:47 vince Exp vince $";
#endif

#ifndef INETD
#define JAVAKILLPORT	23456
#endif

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <syslog.h>
#include <ctype.h>

#define USAGE \
"Usage: %s [-o logfile] [-O lOgfile] address:portnumber\n\
	-o file:	log browser's HTTP requests\n\
	-O file:	log replies from www server\n\
	address:	address of the real proxy server\n\
     portnumber:	port number the real proxy server is running on\n\
I.e.,:\n\
	$ %s wwwproxy.some.domain:8080\n\
\n\
*** remember to configure your brower's proxies accordingly ***\n"

main(argc, argv)
int	argc;
char	*argv[];
{
	unsigned long	resolvhost();
	char		*peername(), *source;

	char		*realproxy;
	char		*logfile = NULL, *lOgfile = NULL;
	char		myself[64];
	int		realproxyport;
	int		child, c;

#ifndef INETD
	struct sockaddr_in	sin_srv;
	int		srv;
	int		on = 1;
#endif
	struct sockaddr_in	sin_clt;
	int		clt, fh;
	int		sinlen;

	int		errflg = 0;
	extern char	*optarg;
	extern int	optind, optopt;

	void		fireman();

        openlog("javakill", LOG_PID | LOG_ODELAY, LOG_DAEMON);

	while ((c = getopt(argc, argv, "o:O:")) != -1)
		switch (c) {
			case	'o':
				logfile = optarg;
				break;

			case	'O':
				lOgfile = optarg;
				break;

			case	':':
			case	'?':
				errflg++;
				break;
		}


	if (gethostname(myself, sizeof(myself))) {
		syslog(LOG_INFO, "gethostname()");
		exit(1);
	}

	if (errflg || argc < 2) {
#ifndef INETD
		fprintf(stderr, USAGE, argv[0], argv[0]);
#else
		syslog(LOG_INFO, "bad arguments");
#endif
		exit(1);
	}

	/* set address:port argument */
	if ((realproxy = (char *)strtok(argv[optind], ":")) == NULL ||
	    (sin_clt.sin_addr.s_addr = resolvhost(realproxy)) == 0L) {
	    	syslog(LOG_INFO, "%s: bad proxy address", realproxy);
		exit(1);
	}
	if ((realproxyport = atoi((char *)strtok(NULL, ":"))) < 1) {
		syslog(LOG_INFO, "bad proxy port number");
		exit(1);
	}

#ifndef INETD
	if ((srv = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		syslog(LOG_INFO, "socket(): %m");
		exit(1);
	}

	if (setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
						sizeof(on)) == -1) {
		syslog(LOG_INFO, "setsockopt(SO_REUSEADDR): %m");
		close(srv);
		exit(1);
	}

	sin_srv.sin_family 	= AF_INET;
	sin_srv.sin_addr.s_addr	= INADDR_ANY;
	sin_srv.sin_port	= htons((u_short) JAVAKILLPORT);

	if (bind(srv, (struct sockaddr *)&sin_srv, sizeof(sin_srv)) == -1) {
		syslog(LOG_INFO, "bind(): %m");
		close(srv);
		exit(1);
	}

	if (listen(srv, 5) == -1) {
		syslog(LOG_INFO, "listen(): %m");
		close(srv);
		exit(1);
	}

	printf("listening on port %d\n", JAVAKILLPORT);

	(void)signal(SIGCHLD, fireman);

loop:
	sinlen = sizeof(sin_srv);
	if ((fh = accept(srv, (struct sockaddr *)&sin_srv, &sinlen)) == -1) {
		if (errno == EINTR)
			goto loop;
		syslog(LOG_INFO, "accept(): %m");
		close(srv);
		exit(1);
	}

	source = (char *)inet_ntoa(sin_srv.sin_addr);
	/*
	printf("serving connection from %s\n", source);
	*/

	if ((child = fork()) == -1) {
		syslog(LOG_INFO, "fork(): %m");
		exit(1);
	}

	if (child) {
		(void)close(fh);
		goto loop;
	}
#else
	fh = 0;
	if ((source = peername(fh)) == NULL) {
		syslog(LOG_INFO, "peername(): %m");
		exit(1);
	}
	/*
	syslog(LOG_INFO, "serving connection from %s", source);
	*/
#endif

	if ((clt = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		syslog(LOG_INFO, "socket(): %m");
#ifndef INETD
		close(srv);
#endif
		close(fh);
		exit(1);
	}

	/* sin_clt.sin_addr was already set */
	sin_clt.sin_family	= AF_INET;
	sin_clt.sin_port	= htons((u_short) realproxyport);

#ifdef SOCKS
	if (Rconnect(clt, (struct sockaddr *)&sin_clt, sizeof(sin_clt)) == -1)
#else
	if (connect(clt, (struct sockaddr *)&sin_clt, sizeof(sin_clt)) == -1)
#endif
								{
		syslog(LOG_INFO, "connect(): %m");
#ifndef INETD
		close(srv);
#endif
		close(fh);
		close(clt);
	}

	if (makeio(fh, clt, logfile, lOgfile, source)) {
		syslog(LOG_INFO, "makeio() failed");
#ifndef INETD
		close(srv);
#endif
		close(fh);
		close(clt);
	}

	close(fh);
#ifndef INETD
	close(srv);
#endif
	close(clt);
	return(0);
}

makeio(client, server, logfile, lOgfile, source)
int	client, server;
char	*logfile, *lOgfile;
char	*source;
{
	void		logbuf(), javakill(), scriptkill();
	char		*xbuf;
	static		xbufsize;
	int		len;

	if (xbufsize == 0) {
		if ((xbuf = (char *)malloc(BUFSIZ * 5)) == NULL) {
			syslog(LOG_INFO, "malloc(): %m");
			return(1);
		}
		xbufsize = BUFSIZ * 5;
	}

	/* receive HTTP request from client */
	if ((len = read(client, xbuf, xbufsize)) == -1) {
		syslog(LOG_INFO, "read(): %m");
		return(1);
	}
	if (len == 0)
		return(0);
	syslog(LOG_INFO, "HTTP request from %s (%d bytes)", source, len);
	logbuf(logfile, xbuf, len);

	/* send request to proxy server */
	if (write_data(server, xbuf, len) != len) {
		syslog(LOG_INFO, "write_data(): %m");
		return(1);
	}

	/* receive reply from proxy */
	if ((len = read_data(server, xbuf, &xbufsize)) < 0) {
		syslog(LOG_INFO, "read_data(): %m");
		return(1);
	}
	syslog(LOG_INFO, "data from WWW server for %s (%d bytes)",
							source,  len);
	logbuf(lOgfile, xbuf, len);

	javakill(xbuf, len, source);
	scriptkill(xbuf, &len, source);
	logbuf(lOgfile, xbuf, len);

	/* finally, send server's reply to browser */
	if (write_data(client, xbuf, len) != len) {
		syslog(LOG_INFO, "write_data(): %m");
		return(1);
	}

	free(xbuf);

	return(0);
}

void
logbuf(logfile, buf, len)
char	*logfile, *buf;
int	len;
{
	int	fh;

	if (logfile == NULL)
		return;

	fh = open(logfile, O_WRONLY | O_APPEND | O_CREAT, 0600);

	if (fh != -1) {
		if (write(fh, buf, len) != len)
			syslog(LOG_INFO, "%s: %m", logfile); 
		else
			close(fh);
	}
	else {
		syslog(LOG_INFO, "%s: %m", logfile);
		logfile = NULL;
	}
}
		
unsigned long
resolvhost(s)
char	*s;
{
	unsigned long	temp;

	temp = inet_addr(s);
	if (temp == -1L) {
		struct sockaddr_in	sin;
		struct hostent		*host;
		if ((host = gethostbyname(s)) == NULL) {
			syslog(LOG_INFO, "%s: bad host", s);
			return(0);
		}
		memcpy(&sin.sin_addr, host->h_addr, host->h_length);
		temp = sin.sin_addr.s_addr;
	}
	return(temp);
}

int		write_data(s, buf, n)
int		s;
char		*buf;
int		n;
{
	int		bcount;
	int		br;

	bcount= 0;
	br= 0;
	while (bcount < n) {
		if ((br = write(s, buf, n - bcount)) > 0) {
			bcount += br;
			buf += br;
		}
	else if (br < 0)
		return(-1);
	}
	return(bcount);
}

/* automatically expand buffer */
int		read_data(s, buf, n)
int		s;
char		*buf;
int		*n;
{
	int		bcount;
	int		br;
	char		*pbuf;

	bcount= 0;
	br= 0;
	pbuf = buf;
	while (bcount < *n) {
		if ((br = read(s, pbuf, *n - bcount)) > 0) {
			bcount += br;
			pbuf += br;
			if (bcount == *n) {
				buf = (char *)realloc(buf, *n + BUFSIZ);
				if (buf == NULL) {
					syslog(LOG_INFO, "realloc()");
					return(-1);
				}
				*n += BUFSIZ;
			}
		}
		else if (br < 0)
			return(-1);
		else if (!br) {
			return(bcount);
			break;
		}
	}
	return(bcount);
}


/* fireman catches falling children */
void
fireman()
{
#if defined(hpux) || defined(SOLARIS) || defined(IRIX) || defined(AIX) || defined(bsdi)
	int	wstatus;
#else
	union wait      wstatus;
#endif

	while (wait3(&wstatus, WNOHANG, NULL) > 0)
		;
	(void)signal(SIGCHLD, fireman);
}

void
javakill(buf, len, source)
char	*buf, *source;
int	len;
{
	char	*strcasestr();
	char	*first = "<applet ", *last = "</applet>";
	char	*p, *q;

	/* check is this is a HTTP document */
	for (p = buf; isspace(*p); p++)
		;
	if (strncasecmp(p, "HTTP/", strlen("HTTP/")))
		return;
	
	/* check is this is a HTML document */
	/*
	if (strcasestr(buf, "<HTML>") == NULL)
		return;
	*/

	/* needs optimization!!! */
	while ((p = strcasestr(buf, first)) != NULL &&
	       (q = strcasestr(p, last)) != NULL) {

		syslog(LOG_INFO, "JAVA tags for %s: killed", source);

		/* overwrite applet tags with remarks */
		memcpy(p, "<!--KILL", 8);
		memcpy(q, "KILLED-->", 9);
		buf = q + 9;
	}
}

void
scriptkill(buf, len, source)
char	*buf, *source;
int	*len;
{
	char	*strcasestr();
	char	*first = "<SCRIPT", *last = "</SCRIPT>";
	char	*p, *q;

	/* check is this is a HTTP document */
	for (p = buf; isspace(*p); p++)
		;
	if (strncasecmp(p, "HTTP/", strlen("HTTP/")))
		return;

	/* check is this is a HTML document */
	/*
	if (strcasestr(buf, "<HTML>") == NULL)
		return;
	*/

	/* needs optimization!!! */
	while ((p = strcasestr(buf, first)) != NULL &&
	       (q = strcasestr(buf, last)) != NULL) {

		/* delete javascript code */
		q += strlen(last);

		memcpy(buf + *len - strlen(p), q, strlen(q));
		syslog(LOG_INFO, "JAVASCRIPT code for %s: killed (%d bytes)",
						source, strlen(p) - strlen(q));
		*len -= strlen(p) - strlen(q);
		buf[*len] = '\0';
	}
}


char *
peername(fd)
int	fd;
{
	struct sockaddr_in	addr;
	struct hostent		*he;
	static char		name[64];
	int			addrlen = sizeof(addr);

	if (getpeername(fd, (struct sockaddr *)&addr, &addrlen) == -1)
		return(NULL);

	if ((he = gethostbyaddr((char *)&addr.sin_addr,
				sizeof(struct in_addr), AF_INET)) == NULL)
		(void)strcpy(name, (char *)inet_ntoa(addr.sin_addr));
	else
		(void)strncpy(name, he->h_name, sizeof(name));
	return(name);
}

char *
strcasestr(s, find)
char	*s, *find;
{
	register char	c, sc;
	register 	len;

	if ((c = *find++) != 0) {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while (sc != c);
		} while (strncasecmp(s, find, len) != 0);
		s--;
	}
        return(s);
}
// ping
// tick
// final-check
