/* $ Id: $ */

/* TODO
 * parse client Id in constructor
 * add version to MINFO
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#ifdef ZTS
#include "TSRM.h"
#endif

#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <zend_extensions.h>
#if PHP_VERSION_ID >= 70000
#include <ext/standard/php_smart_string.h>
#else
#include <ext/standard/php_smart_str.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "php_hconf.h"
#include <hconf.h>

FILE *zk_log_fp = NULL;
static zend_class_entry *hconfig_ce = NULL;
#ifndef HCONF_COMPATIBLE
static zend_class_entry *hconf_ce = NULL;
#endif

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 3)
const zend_fcall_info empty_fcall_info = { 0, NULL, NULL, NULL, NULL, 0, NULL, NULL, 0 };
#undef ZEND_BEGIN_ARG_INFO_EX
#define ZEND_BEGIN_ARG_INFO_EX(name, pass_rest_by_reference, return_reference, required_num_args)   \
    static zend_arg_info name[] = {                                                                       \
        { NULL, 0, NULL, 0, 0, 0, pass_rest_by_reference, return_reference, required_num_args },
#endif

ZEND_DECLARE_MODULE_GLOBALS(php_hconf)

#ifdef COMPILE_DL_HCONF
ZEND_GET_MODULE(hconf)
#endif

/****************************************
  Method implementations
****************************************/

#ifndef HCONF_COMPATIBLE

/* {{{ Qconf::__construct ( .. )
   Creates a HConfig object */
static PHP_METHOD(Qconf, __construct)
{
}
/* }}} */

/* {{{ Qconf::getConf( .. )
   */
static PHP_METHOD(Qconf, getConf)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	long get_flags = HCONF_WAIT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}

	char buf[HCONF_CONF_BUF_MAX_LEN];
	int buf_len = 0;
	int ret = 0;

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_conf(path, buf, sizeof(buf), idc);
	}
	else
	{
		ret = hconf_get_conf(path, buf, sizeof(buf), idc);
	}
	buf_len = strlen(buf);

	if (HCONF_OK == ret)
	{
#if PHP_VERSION_ID >= 70000
		RETVAL_STRINGL(buf, buf_len);
#else
		RETVAL_STRINGL(buf, buf_len, 1);
#endif
	}
	else
	{
		RETURN_NULL();
	}
	return;
}
/* }}} */

/* {{{ Qconf::getHost( .. )
   */
static PHP_METHOD(Qconf, getHost)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	long get_flags = HCONF_WAIT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}

	char buf[HCONF_HOST_BUF_MAX_LEN];
	int buf_len = 0;
	int ret = 0;

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_host(path, buf, sizeof(buf), idc);
	}
	else
	{
		ret = hconf_get_host(path, buf, sizeof(buf), idc);
	}
	buf_len = strlen(buf);

	if (HCONF_OK == ret)
	{
#if PHP_VERSION_ID >= 70000
		RETVAL_STRINGL(buf, buf_len);
#else
		RETVAL_STRINGL(buf, buf_len, 1);
#endif
	}
	else
	{
		RETURN_NULL();
	}
	return;
}
/* }}} */

/* {{{ Qconf::getAllHost( .. )
   */
static PHP_METHOD(Qconf, getAllHost)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	int ret = 0;
	string_vector_t nodes;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_allhost(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_allhost(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

/* {{{ Qconf::getBatchConf( .. )
   */
static PHP_METHOD(Qconf, getBatchConf)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	hconf_batch_nodes bnodes;
	long get_flags = HCONF_WAIT;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_hconf_batch_nodes(&bnodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_conf(path, &bnodes, idc);
	}
	else
	{
		ret = hconf_get_batch_conf(path, &bnodes, idc); 
	}

	if (HCONF_OK == ret && (bnodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < bnodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_assoc_string(return_value, bnodes.nodes[i].key, bnodes.nodes[i].value);
#else
			add_assoc_string(return_value, bnodes.nodes[i].key, bnodes.nodes[i].value, 1);
#endif
		}

		destroy_hconf_batch_nodes(&bnodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

/* {{{ Qconf::getBatchKeys( .. )
   */
static PHP_METHOD(Qconf, getBatchKeys)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	string_vector_t nodes;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_keys(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_batch_keys(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

#ifdef HCONF_INTERNAL
/* {{{ Qconf::getHostNative( .. )
   */
static PHP_METHOD(Qconf, getHostNative)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	int ret = 0;
	string_vector_t nodes;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	ret = init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_keys_native(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_batch_keys_native(path, &nodes, idc); 
	}

	if (HCONF_OK == ret)
	{
		if (nodes.count == 0)
		{
#if PHP_VERSION_ID >= 70000
			RETVAL_STRINGL("", 0);
#else
			RETVAL_STRINGL("", 0, 1);
#endif
		}
		else 
		{
			unsigned int r = rand() % nodes.count;
			size_t node_len = strlen(nodes.data[r]);
#if PHP_VERSION_ID >= 70000
			RETVAL_STRINGL(nodes.data[r], node_len);
#else
			RETVAL_STRINGL(nodes.data[r], node_len, 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

/* {{{ Qconf::getAllHostNative( .. )
   */
static PHP_METHOD(Qconf, getAllHostNative)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	int ret = 0;
	string_vector_t nodes;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_keys_native(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_batch_keys_native(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */
#endif

#endif

/* {{{ HConfig::__construct ( .. )
   Creates a HConfig object */
static PHP_METHOD(HConfig, __construct)
{
}
/* }}} */

/* {{{ HConfig::Get( .. )
   */
static PHP_METHOD(HConfig, Get)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	long get_flags = HCONF_WAIT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}

	char buf[HCONF_CONF_BUF_MAX_LEN];
	int buf_len = 0;
	int ret = 0;

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_conf(path, buf, sizeof(buf), idc);
	}
	else
	{
		ret = hconf_get_conf(path, buf, sizeof(buf), idc);
	}
	buf_len = strlen(buf);

	if (HCONF_OK == ret)
	{
#if PHP_VERSION_ID >= 70000
		RETVAL_STRINGL(buf, buf_len);
#else
		RETVAL_STRINGL(buf, buf_len, 1);
#endif
	}
	else
	{
		RETURN_NULL();
	}
	return;
}
/* }}} */

/* {{{ HConfig::GetChild( .. )
   */
static PHP_METHOD(HConfig, GetChild)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	int ret = 0;
	string_vector_t nodes;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_allhost(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_allhost(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

/* {{{ HConfig::GetBatchConf( .. )
   */
static PHP_METHOD(HConfig, GetBatchConf)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	hconf_batch_nodes bnodes;
	long get_flags = HCONF_WAIT;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_hconf_batch_nodes(&bnodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_conf(path, &bnodes, idc);
	}
	else
	{
		ret = hconf_get_batch_conf(path, &bnodes, idc); 
	}

	if (HCONF_OK == ret && (bnodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < bnodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_assoc_string(return_value, bnodes.nodes[i].key, bnodes.nodes[i].value);
#else
			add_assoc_string(return_value, bnodes.nodes[i].key, bnodes.nodes[i].value, 1);
#endif
		}

		destroy_hconf_batch_nodes(&bnodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

/* {{{ HConfig::GetBatchKeys( .. )
   */
static PHP_METHOD(HConfig, GetBatchKeys)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	string_vector_t nodes;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_keys(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_batch_keys(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */

#ifdef HCONF_INTERNAL
/* {{{ HConfig::GetBatchKeysNative( .. )
   */
static PHP_METHOD(HConfig, GetBatchKeysNative)
{
	char *path, *idc = NULL;
	size_t path_len, idc_len = 0;
	int i;
	long get_flags = HCONF_WAIT;
	int ret = 0;
	string_vector_t nodes;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &path, &path_len, &idc, &idc_len, &get_flags) == FAILURE) 
	{
		RETURN_NULL();
	}
    
	init_string_vector(&nodes);
	if (HCONF_OK != ret)
	{
		RETVAL_NULL();
	}

	if (HCONF_NOWAIT == get_flags)
	{
		ret = hconf_aget_batch_keys_native(path, &nodes, idc);
	}
	else
	{
		ret = hconf_get_batch_keys_native(path, &nodes, idc); 
	}

	if (HCONF_OK == ret && (nodes.count >= 0))
	{
		array_init(return_value);
		for (i = 0; i < nodes.count; i++) 
		{
#if PHP_VERSION_ID >= 70000
			add_next_index_string(return_value, nodes.data[i]);
#else
			add_next_index_string(return_value, nodes.data[i], 1);
#endif
		}

		destroy_string_vector(&nodes);
	}
	else
	{
		RETVAL_NULL();
	}

	return;
}
/* }}} */
#endif

/****************************************
  Internal support code
****************************************/

/* {{{ php_zk_get_ce 
 *    */
PHP_HCONF_API
zend_class_entry *php_zk_get_ce(void)
{
	return hconfig_ce;
}

/* }}} */

/* {{{ methods arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_Get, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_GetChild, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()
	
ZEND_BEGIN_ARG_INFO(arginfo_GetBatchConf, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_GetBatchKeys, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_GetBatchKeysNative, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

#ifndef HCONF_COMPATIBLE
ZEND_BEGIN_ARG_INFO_EX(argconfinfo___construct, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(argconfinfo_getConf, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(argconfinfo_getHost, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()
	
ZEND_BEGIN_ARG_INFO(argconfinfo_getAllHost, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(argconfinfo_getBatchConf, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(argconfinfo_getBatchKeys, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

#ifdef HCONF_INTERNAL
ZEND_BEGIN_ARG_INFO(argconfinfo_getHostNative, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(argconfinfo_getAllHostNative, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()
#endif
#endif
/* }}} */

/* {{{ hconfig_class_methods */
#define QC_ME(class, name, args) PHP_ME(class, name, args, ZEND_ACC_PUBLIC)
#define QC_ME_STATIC(class, name, args) PHP_ME(class, name, args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
static zend_function_entry hconfig_class_methods[] = {
    QC_ME(HConfig, __construct,        arginfo___construct)
    QC_ME_STATIC(HConfig, Get,            arginfo_Get)
    QC_ME_STATIC(HConfig, GetChild,       arginfo_GetChild)
    QC_ME_STATIC(HConfig, GetBatchConf,   arginfo_GetBatchConf)
    QC_ME_STATIC(HConfig, GetBatchKeys,   arginfo_GetBatchKeys)
#ifdef HCONF_INTERNAL
    QC_ME_STATIC(HConfig, GetBatchKeysNative,       arginfo_GetBatchKeysNative)
#endif

    { NULL, NULL, NULL }
};

#ifndef HCONF_COMPATIBLE
static zend_function_entry hconf_class_methods[] = {
    QC_ME(Qconf, __construct,        	 argconfinfo___construct)
    QC_ME_STATIC(Qconf, getConf,         argconfinfo_getConf)
    QC_ME_STATIC(Qconf, getHost,         argconfinfo_getHost)
    QC_ME_STATIC(Qconf, getAllHost,      argconfinfo_getAllHost)
    QC_ME_STATIC(Qconf, getBatchConf,    argconfinfo_getBatchConf)
    QC_ME_STATIC(Qconf, getBatchKeys,    argconfinfo_getBatchKeys)
#ifdef HCONF_INTERNAL
	QC_ME_STATIC(Qconf, getHostNative,   argconfinfo_getHostNative)
	QC_ME_STATIC(Qconf, getAllHostNative,argconfinfo_getAllHostNative)
#endif

    { NULL, NULL, NULL }
};
#endif

#undef QC_ME
#undef QC_ME_STATIC

/* }}} */

/* {{{ hconf_module_entry
 */
zend_module_entry hconf_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
    STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL,
#else
    STANDARD_MODULE_HEADER,
#endif
	"hconf",
	NULL,
	PHP_MINIT(hconf),
	PHP_MSHUTDOWN(hconf),
	NULL,
	PHP_RSHUTDOWN(hconf),
	PHP_MINFO(hconf),
	HCONF_DRIVER_PHP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ PHP_INI_BEGIN */
PHP_INI_BEGIN()
//	STD_PHP_INI_ENTRY("hconf.log_dir", "", PHP_INI_ALL, OnUpdateString, log_dir, zend_php_hconf_globals, php_hconf_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(hconf)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HConfig", hconfig_class_methods);
	hconfig_ce = zend_register_internal_class(&ce TSRMLS_CC);

#ifndef HCONF_COMPATIBLE
	zend_class_entry qce;
	INIT_CLASS_ENTRY(qce, "Qconf", hconf_class_methods);
	hconf_ce = zend_register_internal_class(&qce TSRMLS_CC);
#endif

	/* set debug level to warning by default */
	// zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
#ifdef ZTS
    ts_allocate_id(&php_hconf_globals_id, sizeof(zend_php_hconf_globals), NULL, NULL);
#endif
	REGISTER_INI_ENTRIES();
	
	// const char *logfile = "/dev/null";
	// zk_log_fp = fopen(logfile, "a");
	// if (zk_log_fp)  {
	// 	zoo_set_log_stream(zk_log_fp);
	// }

	hconf_init();

	srand(time(NULL));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(hconf)
{
    hconf_destroy();
	// if (zk_log_fp){
	// 	fclose(zk_log_fp);
	// 	zk_log_fp = NULL;
	// }
	UNREGISTER_INI_ENTRIES();	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(hconf)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(hconf)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(hconf)
{
//	char buf[32];

	php_info_print_table_start();

	php_info_print_table_header(2, "hconf support", "enabled");
	php_info_print_table_row(2, "hconf version", HCONF_DRIVER_PHP_VERSION);

//	snprintf(buf, sizeof(buf), "%ld.%ld.%ld", ZOO_MAJOR_VERSION, ZOO_MINOR_VERSION, ZOO_PATCH_VERSION);
//	php_info_print_table_row(2, "libzookeeper version", buf);

	php_info_print_table_end();
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: noet sw=4 ts=4 fdm=marker:
 */
