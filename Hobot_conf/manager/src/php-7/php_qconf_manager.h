/* $Id$ */

#ifndef PHP_HCONF_MANAGER_H
#define PHP_HCONF_MANAGER_H

extern "C"{
#include "php.h"
}

#define PHP_HCONF_MANAGER_VERSION "1.0.0" /* Replace with version number for your extension */

extern zend_module_entry hconf_manager_module_entry;
#define phpext_hconf_manager_ptr &hconf_manager_module_entry


#ifdef PHP_WIN32
#	define PHP_HCONF_MANAGER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_HCONF_MANAGER_API __attribute__ ((visibility("default")))
#else
#	define PHP_HCONF_MANAGER_API
#endif

PHP_MINIT_FUNCTION(hconf_manager);
PHP_MSHUTDOWN_FUNCTION(hconf_manager);
PHP_MINFO_FUNCTION(hconf_manager);

#ifdef ZTS
#include "TSRM.h"
#endif


#ifdef ZTS
#define HCONF_MANAGER_G(v) TSRMG(hconf_manager_globals_id, zend_hconf_manager_globals *, v)
#else
#define HCONF_MANAGER_G(v) (hconf_manager_globals.v)
#endif



#endif	/* PHP_HCONF_MANAGER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
