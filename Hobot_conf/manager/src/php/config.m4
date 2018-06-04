dnl $Id$
dnl config.m4 for extension hconf_manager

PHP_ARG_WITH(hconfzk-dir,  for libhconfzk,
[  --with-hconfzk-dir[=DIR]   Set the path to libhconfzk include prefix.], yes)

PHP_ARG_ENABLE(hconf_manager, whether to enable hconf_manager support,
[  --enable-hconf_manager           Enable hconf_manager support])


if test "$PHP_HCONF_MANAGER" != "no"; then
    if test "$PHP_HCONFZK_DIR" != "no" && test "$PHP_HCONFZK_DIR" != "yes"; then
        if test -r "$PHP_HCONFZK_DIR/hconf_zk.h"; then
            PHP_HCONFZK_INCDIR="$PHP_HCONFZK_DIR"
            PHP_ZOOKEEPER_INCDIR="$PHP_HCONFZK_DIR/zookeeper"
        else
            AC_MSG_ERROR([Can't find hconf headers under "$PHP_PHP_HCONFZK_DIR"])
        fi
    else
        AC_MSG_ERROR([Need arg --with-libhconfzk-dir"])
    fi

    PHP_ADD_INCLUDE($PHP_HCONFZK_INCDIR)
    PHP_ADD_INCLUDE($PHP_ZOOKEEPER_INCDIR)
    PHP_REQUIRE_CXX()  

    PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)

    PHP_NEW_EXTENSION(hconf_manager, hconf_manager.cc, $ext_shared)
fi
