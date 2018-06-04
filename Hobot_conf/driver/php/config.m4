dnl
dnl $ Id: $
dnl vim:se ts=2 sw=2 et:

PHP_ARG_ENABLE(hconf, whether to enable hconf support,
[  --enable-hconf               Enable hconf support])

dnl PHP_ARG_ENABLE(hconf-compatible, whether to enable hconf compatible support,
dnl [  --enable-hconf-compatible               Enable hconf compatible  support])

PHP_ARG_WITH(libhconf-dir,  for libhconf,
[  --with-libhconf-dir[=DIR]   Set the path to libhconf include prefix.], yes)

if test -z "$PHP_DEBUG"; then
  AC_ARG_ENABLE(debug,
  [  --enable-debug          compile with debugging symbols],[
    PHP_DEBUG=$enableval
  ],[    PHP_DEBUG=no
  ])
fi

if test "$PHP_HCONF" != "no"; then
 
   if test "$PHP_LIBHCONF_DIR" != "no" && test "$PHP_LIBHCONF_DIR" != "yes"; then
     if test -r "$PHP_LIBHCONF_DIR/hconf.h"; then
       PHP_LIBHCONF_INCDIR="$PHP_LIBHCONF_DIR"
     else
       AC_MSG_ERROR([Can't find hconf headers under "$PHP_LIBHCONF_DIR"])
     fi
   else
     PHP_LIBHCONF_DIR="no"
     for i in /usr/local/include/hconf; do
       if test -r "$i/hconf.h"; then
         PHP_LIBHCONF_INCDIR=$i
     break
       fi
     done

     if test "$PHP_LIBHCONF_INCDIR" = ""; then
       AC_MSG_ERROR([Can't find hconf headers under "$PHP_LIBHCONF_DIR"])
     fi
   fi

dnl    PHP_LIBHCONF_INCDIR="/usr/local/include/hconf"
    PHP_ADD_INCLUDE($PHP_LIBHCONF_INCDIR)

    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(stdc++, "", EXTRA_LDFLAGS)
    PHP_NEW_EXTENSION(hconf, php_hconf.c $SESSION_EXTRA_FILES, $ext_shared)

dnl  fi

fi
