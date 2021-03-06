/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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

void hconfzk_free_storage(void *object TSRMLS_DC)
{
    hconfzk_object *obj = (hconfzk_object *)object;
    delete obj->hconfzk;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value hconfzk_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    hconfzk_object *obj = (hconfzk_object *)emalloc(sizeof(hconfzk_object));
    memset(obj, 0, sizeof(hconfzk_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->properties_info,
            (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
            hconfzk_free_storage, NULL TSRMLS_CC);
    retval.handlers = &hconfzk_object_handlers;

    return retval;
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
    zval *instance = getThis();

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &host, &host_len) == FAILURE)
    {
        RETURN_NULL();
    }

    HConfZK *hconfZk = new HConfZK();
    hconfZk->zk_init(std::string(host, host_len));

    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(instance TSRMLS_CC);
    obj->hconfzk = hconfZk;
}
/* }}} */

/* {{{ HConfZK::__construct ( .. )
   Creates a HConfZK object */
static PHP_METHOD(HConfZK, __destruct)
{
    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        std::string value;
        if (HCONF_OK != hconfzk->zk_node_get(std::string(path, path_len), value)) RETURN_NULL();
        RETURN_STRINGL(const_cast<char*>(value.c_str()), value.size(), 1);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_node_delete(std::string(path, path_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}

static PHP_METHOD(HConfZK, servicesSet)
{
    char *path = NULL;
    zval *array_input = NULL;
    zval **z_item = NULL;
    int path_len = 0, service_count = 0, i = 0;
    std::map<std::string, char> mserv;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &path, &path_len, &array_input) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    service_count = zend_hash_num_elements(Z_ARRVAL_P(array_input));

    zend_hash_internal_pointer_reset(Z_ARRVAL_P(array_input));
    for (i = 0; i < service_count; i++)
    {
        char* key;
        unsigned long idx;
        zend_hash_get_current_data(Z_ARRVAL_P(array_input), (void**) &z_item);
        convert_to_long_ex(z_item);
        if (zend_hash_get_current_key(Z_ARRVAL_P(array_input), &key, &idx, 0) == HASH_KEY_IS_STRING)
        {
            if (!isascii(Z_LVAL_PP(z_item)))
                RETURN_LONG(HCONF_PHP_ERROR);
            mserv.insert(std::pair<std::string, char>(key, Z_LVAL_PP(z_item)));
        }
        else
        {
            RETURN_LONG(HCONF_PHP_ERROR);
        }
        zend_hash_move_forward(Z_ARRVAL_P(array_input));
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL) RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::set<std::string> servs;
    ret = hconfzk->zk_services_get(std::string(path, path_len), servs);
    if (HCONF_OK != ret) RETURN_NULL();

    zval *return_data = NULL;
    MAKE_STD_ZVAL(return_data);
    array_init(return_data);
    std::set<std::string>::const_iterator it;
    int i = 0;
    for (it = servs.begin(); it != servs.end(); ++it, ++i)
    {
        add_index_string(return_data, i, const_cast<char*>((*it).c_str()), 1);
    }
    *return_value = *return_data;
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::map<std::string, char> servs;
    ret = hconfzk->zk_services_get_with_status(std::string(path, path_len), servs);
    if (HCONF_OK != ret) RETURN_NULL();

    zval *return_data;
    MAKE_STD_ZVAL(return_data);
    array_init(return_data);

    std::map<std::string, char>::iterator it = servs.begin();
    for (int i = 0; it != servs.end(); ++it, ++i)
    {
        add_assoc_long(return_data, const_cast<char*>((it->first).c_str()), it->second);
    }
    *return_value = *return_data;
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL) RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::set<std::string> children;
    ret = hconfzk->zk_list(std::string(path, path_len), children);
    if (HCONF_OK != ret) RETURN_NULL();

    zval *return_data = NULL;
    MAKE_STD_ZVAL(return_data);
    array_init(return_data);
    std::set<std::string>::const_iterator it;
    int i = 0;
    for (it = children.begin(); it != children.end(); ++it, ++i)
    {
        add_index_string(return_data, i, const_cast<char*>((*it).c_str()), 1);
    }
    *return_value = *return_data;
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk == NULL)
        RETURN_NULL();

    int ret = HCONF_PHP_ERROR;
    std::map<std::string, std::string> children;
    ret = hconfzk->zk_list_with_values(std::string(path, path_len), children);
    if (HCONF_OK != ret) RETURN_NULL();

    zval *return_data = NULL;
    MAKE_STD_ZVAL(return_data);
    array_init(return_data);

    std::map<std::string, std::string>::iterator it = children.begin();
    for (int i = 0; it != children.end(); ++it, ++i)
    {
        add_assoc_string(return_data, const_cast<char*>((it->first).c_str()), (char*)(it->second).c_str(), 1);
    }
    *return_value = *return_data;
    return;
}

static PHP_METHOD(HConfZK, grayBegin)
{
    zval *apath = NULL, *amachine = NULL;
    zval **z_item = NULL;
    int paths_count = 0, machine_count = 0, i = 0;
    std::map<std::string, std::string> mpaths;
    std::vector< std::string > mmachines;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &apath, &amachine) == FAILURE)
    {
        RETURN_NULL();
    }

    // paths value map
    paths_count = zend_hash_num_elements(Z_ARRVAL_P(apath));
    zend_hash_internal_pointer_reset(Z_ARRVAL_P(apath));
    for (i = 0; i < paths_count; i++)
    {
        char* key;
        unsigned long idx;
        zend_hash_get_current_data(Z_ARRVAL_P(apath), (void**) &z_item);
        convert_to_string_ex(z_item);
        if (zend_hash_get_current_key(Z_ARRVAL_P(apath), &key, &idx, 0) != HASH_KEY_IS_STRING)
            RETURN_NULL();
        mpaths.insert(std::pair<std::string, std::string>(key, Z_STRVAL_PP(z_item)));
        zend_hash_move_forward(Z_ARRVAL_P(apath));
    }

    // machine vector
    machine_count = zend_hash_num_elements(Z_ARRVAL_P(amachine));
    zend_hash_internal_pointer_reset(Z_ARRVAL_P(amachine));
    for (i = 0; i < machine_count; i++)
    {
        char* key;
        unsigned long idx;
        zend_hash_get_current_data(Z_ARRVAL_P(amachine), (void**) &z_item);
        convert_to_string_ex(z_item);
        if (zend_hash_get_current_key(Z_ARRVAL_P(amachine), &key, &idx, 0) == HASH_KEY_IS_STRING)
            RETURN_NULL()
        mmachines.push_back(Z_STRVAL_PP(z_item));
        zend_hash_move_forward(Z_ARRVAL_P(amachine));
    }

    HConfZK *hconfzk = NULL;
    std::string gray_id;
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        if (HCONF_OK == hconfzk->zk_gray_begin(mpaths, mmachines, gray_id))
            RETURN_STRINGL(const_cast<char*>(gray_id.c_str()), gray_id.size(), 1);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
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
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if (hconfzk != NULL) {
        RETURN_LONG(hconfzk->zk_gray_commit(std::string(gray_id, gray_id_len)));
    }

    RETURN_LONG(HCONF_PHP_ERROR);
}


static PHP_METHOD(HConfZK, grayContent)
{
    char *gray_id = NULL;
    int gray_id_len = 0;

    std::vector<std::pair<std::string, std::string> > nodes;
    std::vector<std::pair<std::string, std::string> >::iterator it;
    array_init(return_value);

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &gray_id, &gray_id_len) == FAILURE)
    {
        RETURN_LONG(HCONF_PHP_ERROR);
    }

    HConfZK *hconfzk = NULL;
    hconfzk_object *obj = (hconfzk_object *)zend_object_store_get_object(
            getThis() TSRMLS_CC);
    hconfzk = obj->hconfzk;
    if(hconfzk != NULL){
        if (HCONF_OK != hconfzk->zk_gray_get_content(std::string(gray_id, gray_id_len), nodes)) RETURN_LONG(HCONF_PHP_ERROR);
        for (it = nodes.begin(); it != nodes.end(); ++it)
        {
            add_assoc_string(return_value,(*it).first.c_str(),(char *)(*it).second.c_str(),1);
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
