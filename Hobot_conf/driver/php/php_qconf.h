/* $ Id: $ */

#ifndef PHP_HCONF_H
#define PHP_HCONF_H

#include <php.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define HCONF_DRIVER_PHP_VERSION 		"1.2.2"
#define HCONF_WAIT                      0
#define HCONF_NOWAIT                    1 

extern zend_module_entry hconf_module_entry;
#define phpext_hconf_ptr &hconf_module_entry

#ifdef PHP_WIN32
#define PHP_HCONF_API __declspec(dllexport)
#else
#define PHP_HCONF_API
#endif

ZEND_BEGIN_MODULE_GLOBALS(php_hconf)
	HashTable callbacks;
	zend_bool session_lock;
	long sess_lock_wait;
	long max_times; 		// max_repeat_read_times
ZEND_END_MODULE_GLOBALS(php_hconf)

PHP_MINIT_FUNCTION(hconf);
PHP_MSHUTDOWN_FUNCTION(hconf);
PHP_RSHUTDOWN_FUNCTION(hconf);
PHP_MINFO_FUNCTION(hconf);

ZEND_EXTERN_MODULE_GLOBALS(php_hconf)

#ifdef ZTS
#define HCONF_G(v) TSRMG(php_hconf_globals_id, zend_php_hconf_globals *, v)
#else
#define HCONF_G(v) (php_hconf_globals.v)
#endif

#endif /* PHP_HCONF_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
