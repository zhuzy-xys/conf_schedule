/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_hconf_manager.h"
#include <ctype.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "hconf_zk.h"

#define HCONF_PHP_ERROR -1

zend_object_handlers hconfzk_object_handlers;
struct hconfzk_object
{
    zend_object std;
    HConfZK *hconfzk;
};

void hconfzk_free_storage(zend_object *object TSRMLS_DC)
{
    hconfzk_object *obj = (hconfzk_object *)object;
    delete obj->hconfzk;
    // zend_hash_destroy(obj->std.properties);
    // FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

static inline struct hconfzk_object * php_custom_object_fetch_object(zend_object *obj) {
      return (struct hconfzk_object *)((char *)obj - XtOffsetOf(struct hconfzk_object , std));
}

#define Z_HCONFZK_OBJ_P(zv) php_custom_object_fetch_object(Z_OBJ_P(zv));

zend_object * hconfzk_create_handler(zend_class_entry *ce TSRMLS_DC) {
     // Allocate sizeof(custom) + sizeof(properties table requirements)
	struct hconfzk_object *intern = (struct hconfzk_object *)ecalloc(
		1,
		sizeof(struct hconfzk_object) +
		zend_object_properties_size(ce));
     // Allocating:
     // struct custom_object {
	 //    void *custom_data;
	 //    zend_object std;
	 // }
     // zval[ce->default_properties_count-1]
     zend_object_std_init(&intern->std, ce TSRMLS_CC);
     hconfzk_object_handlers.offset = XtOffsetOf(struct hconfzk_object, std);
     hconfzk_object_handlers.free_obj = hconfzk_free_storage;

     intern->std.handlers = &hconfzk_object_handlers;

     return &intern->std;
}

/****************************************
  Method implementations
 ****************************************/
zend_class_entry *hconfzk_ce;

/* {{{ HConfZK::__construct ( .. )
   Creates a HConfZK object */
static PHP_METHOD(HConfZK, __construct)
{
    char *host = NULL;
    int host_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &host, &host_len) == FAILURE)
    {
        RETURN_NULL();
    }

    HConfZK *hconfZk = new HConfZK();
    hconfZk->zk_init(std::string(host, host_len));

    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    obj->hconfzk = hconfZk;
}
/* }}} */

/* {{{ HConfZK::__construct ( .. )
   Creates a HConfZK object */
static PHP_METHOD(HConfZK, __destruct)
{
    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL)
    {
        hconfzk->zk_close();
    }
}
/* }}} */

/* {{{ Method implementations
*/
static PHP_METHOD(HConfZK, nodeSet)
{
    char *path = NULL, *value = NULL;
    int path_len = 0, value_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &value, &value_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL)
    {
        RETURN_LONG(hconfzk->zk_node_set(std::string(path, path_len), std::string(value, value_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, nodeGet)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_NULL();
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        std::string value;
        if (HCONF_OK != hconfzk->zk_node_get(std::string(path, path_len), value)) RETURN_NULL();
        RETURN_STRINGL(const_cast<char*>(value.c_str()), value.size());
    }
    RETURN_NULL();
}

static PHP_METHOD(HConfZK, nodeDelete)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_node_delete(std::string(path, path_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, servicesSet)
{
    char *path = NULL;
    zval *array_input, *val;
	ulong num_key;
    int path_len = 0;
    std::map<std::string, char> mserv;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &path, &path_len, &array_input) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

	HashTable *arr_hash = Z_ARRVAL_P(array_input);
	zend_string *key;
	ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
		if (!key || !isascii(Z_LVAL_P(val))) {
			RETURN_LONG(HCONF_PHP_ERROR);
		}
		mserv.insert(std::make_pair(std::string(key->val, key->len), Z_LVAL_P(val)));
	} ZEND_HASH_FOREACH_END();

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL)
    {
        RETURN_LONG(hconfzk->zk_services_set(std::string(path, path_len), mserv));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, servicesGet)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_NULL();
    }

    // Get C++ object
    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL) RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::set<std::string> servs;
    ret = hconfzk->zk_services_get(std::string(path, path_len), servs);
    if (HCONF_OK != ret) RETURN_NULL();

    array_init(return_value);
    std::set<std::string>::const_iterator it;
    int i = 0;
    for (it = servs.begin(); it != servs.end(); ++it, ++i)
    {
        add_index_string(return_value, i, const_cast<char*>((*it).c_str()));
    }
    return;
}

static PHP_METHOD(HConfZK, servicesGetWithStatus)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_NULL();
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::map<std::string, char> servs;
    ret = hconfzk->zk_services_get_with_status(std::string(path, path_len), servs);
    if (HCONF_OK != ret) RETURN_NULL();

    array_init(return_value);

    std::map<std::string, char>::iterator it = servs.begin();
    for (int i = 0; it != servs.end(); ++it, ++i)
    {
        add_assoc_long(return_value, const_cast<char*>((it->first).c_str()), it->second);
    }
    return;
}

static PHP_METHOD(HConfZK, serviceAdd)
{
    char *path, *serv = NULL;
    int path_len = 0, serv_len = 0;
    long status = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &path, &path_len, &serv, &serv_len, &status) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    if (!isascii(status)) RETURN_LONG(HCONF_PHP_ERROR);

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_LONG(HCONF_PHP_ERROR);
    RETURN_LONG(hconfzk->zk_service_add(std::string(path, path_len), std::string(serv, serv_len), status));
}

static PHP_METHOD(HConfZK, serviceDelete)
{
    char *path = NULL, *serv = NULL;
    int path_len = 0, serv_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &serv, &serv_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_LONG(HCONF_PHP_ERROR);

    RETURN_LONG(hconfzk->zk_service_delete(std::string(path, path_len), std::string(serv, serv_len)));
}

static PHP_METHOD(HConfZK, serviceUp)
{
    char *path = NULL, *serv = NULL;
    int path_len = 0, serv_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &serv, &serv_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_LONG(HCONF_PHP_ERROR);

    RETURN_LONG(hconfzk->zk_service_up(std::string(path, path_len), std::string(serv, serv_len)));
}

static PHP_METHOD(HConfZK, serviceDown)
{
    char *path = NULL, *serv = NULL;
    int path_len = 0, serv_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &serv, &serv_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_LONG(HCONF_PHP_ERROR);

    RETURN_LONG(hconfzk->zk_service_down(std::string(path, path_len), std::string(serv, serv_len)));
}

static PHP_METHOD(HConfZK, serviceOffline)
{
    char *path = NULL, *serv = NULL;
    int path_len = 0, serv_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &serv, &serv_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_LONG(HCONF_PHP_ERROR);

    RETURN_LONG(hconfzk->zk_service_offline(std::string(path, path_len), std::string(serv, serv_len)));
}

static PHP_METHOD(HConfZK, serviceClear)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_service_clear(std::string(path, path_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, list)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_NULL();
    }

    // Get C++ object
    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL) RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::set<std::string> children;
    ret = hconfzk->zk_list(std::string(path, path_len), children);
    if (HCONF_OK != ret) RETURN_NULL();

    array_init(return_value);
    std::set<std::string>::const_iterator it;
    int i = 0;
    for (it = children.begin(); it != children.end(); ++it, ++i)
    {
        add_index_string(return_value, i, const_cast<char*>((*it).c_str()));
    }
    return;
}

static PHP_METHOD(HConfZK, listWithValue)
{
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE)
    {
        RETURN_NULL();
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::map<std::string, std::string> children;
    ret = hconfzk->zk_list_with_values(std::string(path, path_len), children);
    if (HCONF_OK != ret) RETURN_NULL();

    array_init(return_value);

    std::map<std::string, std::string>::iterator it = children.begin();
    for (int i = 0; it != children.end(); ++it, ++i)
    {
        add_assoc_string(return_value, const_cast<char*>((it->first).c_str()), (char*)(it->second).c_str());
    }
    return;
}

static PHP_METHOD(HConfZK, grayBegin)
{
    zval *apath = NULL, *amachine = NULL;
	zval *val;
	zend_string* key;
	ulong num_key;
    std::map<std::string, std::string> mpaths;
    std::vector<std::string> mmachines;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &apath, &amachine) == FAILURE)
    {
        RETURN_NULL();
    }

    // paths value map
	HashTable *path_arr = Z_ARRVAL_P(apath);
	ZEND_HASH_FOREACH_KEY_VAL(path_arr, num_key, key, val) {
		if (!key) {
			RETURN_NULL();
		}
		mpaths.insert(std::make_pair(std::string(key->val, key->len),
									 std::string(Z_STR_P(val)->val,
												 Z_STR_P(val)->len)));
	} ZEND_HASH_FOREACH_END();

    // machine vector
	HashTable *machine_arr = Z_ARRVAL_P(amachine);
	ZEND_HASH_FOREACH_KEY_VAL(machine_arr, num_key, key, val) {
		if (key) { // HASH_KEY_IS_STRING
			RETURN_NULL();
		}
		mmachines.push_back(std::string(Z_STR_P(val)->val, Z_STR_P(val)->len));
	} ZEND_HASH_FOREACH_END();

    HConfZK *hconfzk = NULL;
    std::string gray_id;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        if (HCONF_OK == hconfzk->zk_gray_begin(mpaths, mmachines, gray_id))
            RETURN_STRINGL(const_cast<char*>(gray_id.c_str()), gray_id.size());
    }

    RETURN_NULL();
}

static PHP_METHOD(HConfZK, grayRollback)
{
    char *gray_id = NULL;
    int gray_id_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &gray_id, &gray_id_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_gray_rollback(std::string(gray_id, gray_id_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, grayCommit)
{
    char *gray_id = NULL;
    int gray_id_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &gray_id, &gray_id_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_gray_commit(std::string(gray_id, gray_id_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, grayContent)
{
    zend_string *gray_id;
    std::vector<std::pair<std::string, std::string> > nodes;
    std::vector<std::pair<std::string, std::string> >::iterator it;
    array_init(return_value);

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &gray_id) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = Z_HCONFZK_OBJ_P(getThis());
    hconfzk = obj->hconfzk;
    if(hconfzk != NULL){
        if (HCONF_OK != hconfzk->zk_gray_get_content(ZSTR_VAL(gray_id), nodes)) RETURN_LONG(HCONF_PHP_ERROR);
        for (it = nodes.begin(); it != nodes.end(); ++it)
        {
            add_assoc_string(return_value,(*it).first.c_str(),(char *)(*it).second.c_str());
        }
        RETURN_ZVAL(return_value,0,0);
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}
/* }}} */



/****************************************
  Internal support code
 ****************************************/

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hconf_manager)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hconf_manager)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "hconf_manager support", "enabled");
	php_info_print_table_row(2, "hconf_manager version", PHP_HCONF_MANAGER_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ HConfZK_functions[]
 *
 * Every user visible function must have an entry in hconf_manager_functions[].
 */
zend_function_entry hconfzk_methods[] = {
    PHP_ME(HConfZK,  __construct,           NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(HConfZK,  __destruct,            NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(HConfZK,  nodeGet,               NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  nodeSet,               NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  nodeDelete,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  servicesGet,           NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  servicesSet,           NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  servicesGetWithStatus, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceAdd,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceDelete,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceUp,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceDown,           NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceOffline,        NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  serviceClear,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  list,                  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  listWithValue,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  grayBegin,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  grayRollback,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  grayCommit,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HConfZK,  grayContent,           NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}	/* Must be the last line in hconf_manager_functions[] */
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hconf_manager)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "HConfZK", hconfzk_methods);
    hconfzk_ce = zend_register_internal_class(&ce TSRMLS_CC);
    hconfzk_ce->create_object = hconfzk_create_handler;
    memcpy(&hconfzk_object_handlers,
    zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    hconfzk_object_handlers.clone_obj = NULL;

    //Const value
    REGISTER_LONG_CONSTANT("HCONF_STATUS_UP", 0, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("HCONF_STATUS_OFFLINE", 1, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("HCONF_STATUS_DOWN", 2, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

/* }}} */
/* {{{ hconf_manager_module_entry
 */
zend_module_entry hconf_manager_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
	"hconf_manager",
	NULL,
	PHP_MINIT(hconf_manager),
	PHP_MSHUTDOWN(hconf_manager),
	NULL,
	NULL,
	PHP_MINFO(hconf_manager),
	PHP_HCONF_MANAGER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HCONF_MANAGER
extern "C"{
ZEND_GET_MODULE(hconf_manager)
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
