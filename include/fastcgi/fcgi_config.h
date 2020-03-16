/*
*  Copied to fcgi_config.h when building on WinNT without cygwin,
*  i.e. configure is not run.  See fcgi_config.h.in for details.
*/
#if _WIN32
#define HAVE_FPOS 1
#define HAVE_LIMITS_H 1
#define HAVE_STREAMBUF_CHAR_TYPE 1
#define HAVE_STRERROR 1
#undef HAVE_ARPA_INET_H
#undef HAVE_DLFCN_H
#undef HAVE_FILENO_PROTO
#undef HAVE_INTTYPES_H
#undef HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
#undef HAVE_LIBNSL
#undef HAVE_LIBSOCKET
#undef HAVE_MEMORY_H
#undef HAVE_NETDB_H
#undef HAVE_NETINET_IN_H
#undef HAVE_PTHREAD
#undef HAVE_SOCKADDR_UN_SUN_LEN
#undef HAVE_SOCKLEN
#undef HAVE_STDINT_H
#undef HAVE_STDLIB_H
#undef HAVE_STRING_H
#undef HAVE_STRINGS_H
#undef HAVE_SYS_PARAM_H
#undef HAVE_SYS_SOCKET_H
#undef HAVE_SYS_STAT_H
#undef HAVE_SYS_TIME_H
#undef HAVE_SYS_TYPES_H
#undef HAVE_UNISTD_H
#undef HAVE_VA_ARG_LONG_DOUBLE_BUG
#undef PTHREAD_CREATE_JOINABLE
#undef STDC_HEADERS
#undef USE_LOCKING
#undef const
#undef inline
#undef ssize_t

#else

/* Define if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if there's a fileno() prototype in stdio.h */
#define HAVE_FILENO_PROTO 1

/* Define if the fpos_t typedef is in stdio.h */
#define HAVE_FPOS 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if cin/cout/cerr has a streambuf assignment operator */
/* #undef HAVE_IOSTREAM_WITHASSIGN_STREAMBUF */

/* Define if you have the `nsl' library (-lnsl). */
#define HAVE_LIBNSL 1

/* Define if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* Define if sockaddr_un in sys/un.h contains a sun_len component */
/* #undef HAVE_SOCKADDR_UN_SUN_LEN */

/* Define if the socklen_t typedef is in sys/socket.h */
/* #undef HAVE_SOCKLEN */

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if char_type is defined in the context of streambuf */
#define HAVE_STREAMBUF_CHAR_TYPE 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if va_arg(arg, long double) crashes the compiler */
/* #undef HAVE_VA_ARG_LONG_DOUBLE_BUG */

/* Name of package */
#define PACKAGE "fcgi"

/* Define to the necessary symbol if this constant uses a non-standard name on
your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if cross-process locking is required by accept() */
/* #undef USE_LOCKING */

/* Version number of package */
#define VERSION "2.4.0"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
if it is not supported. */
/* #undef inline */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

#endif
