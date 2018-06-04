QConf C\C++ Doc
=====

## C Interface
### **Environment initialisation and destroy functions.**

-------

### **hconf_init** 

`int hconf_init();`

Description
> Initial hconf environment

Parameters

Return Value**

> HCONF_OK if success,  others if failed. 

Example
> int ret = hconf_init();

### **hconf_destroy**
`int hconf_destroy();`

Description

>destroy hconf environment

Parameters

Return Value

Example
>hconf_destroy();

---

### **QConf access functions wait version, retry sometime if configure not exist**

----

### **hconf_get_conf**

`int hconf_get_conf(const char *path, char *buf, unsigned int buf_len, const char *idc);`

Description
>get configure value

Parameters
>path - key of configuration.
>
>buf - out parameter, buffer for value
>
>buf_len - lenghth of value buffer
>
>idc - from which idc to get the value，get from local idc if idc is NULL

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_NOT_FOUND if configuration is not exists
 
Example 
>char value[HCONF_CONF_BUF_MAX_LEN];
>
>int ret = hconf_get_conf("demo/conf1", value, sizeof(value), NULL);
>
>assert(HCONF_OK == ret);   

### **hconf_get_batch_keys**

`int hconf_get_batch_keys(const char *path, string_vector_t *nodes, const char *idc);`

Description
>get all children nodes'key

Parameters
>path - key of configuration.
>
>nodes - out parameter, keep all children nodes'key
>
>idc - from which idc to get the keys，get from local idc if idc is NULL

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_NOT_FOUND if not exists
 
Example
>string_vector_t bnodes_key;
>
>init_string_vector(&bnodes_key);
> 
>int ret = hconf_get_batch_keys(path, &bnodes_key, NULL);
>
>assert(HCONF_OK == ret);
> 
>for (i = 0; i < bnodes_key.count; i++)
>
>{ cout << bnodes_key.data[i] << endl;}
> 
>destroy_string_vector(&bnodes_key); 


### **hconf_get_batch_conf**

`int hconf_get_batch_conf(const char *path, string_vector_t *nodes, const char *idc);`

Description
>get all children nodes' key and value

Parameters
> path - key of configuration.
>
> nodes - out parameter, keep all children nodes' key and value
>
> idc - from which idc to get the children configurations，get from local idc if idc is NULL

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_NOT_FOUND if not exists
 
Example
 >hconf_batch_nodes bnodes;
 >
 >init_hconf_batch_nodes(&bnodes);
 > 
 >int ret = hconf_get_batch_conf(path, &bnodes, NULL);
 >
 >assert(HCONF_OK == ret);
 > 
 >for (i = 0; i < bnodes.count; i++)
 >
 >{cout << bnodes.nodes[i].key << " : " << bnodes.nodes[i].value;}
 > 
 >destroy_hconf_batch_nodes(&bnodes); 

### **hconf_get_allhost**

`int hconf_get_allhost(const char *path, string_vector_t *nodes, const char *idc);`

Description
>get all available services under given path

Parameters
 > path - key of configuration.
 >
 > nodes - out parameter, keep all available services
 >
 > idc - from which idc to get the services，get from local idc if idc is NULL

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_NOT_FOUND if not exists
 
Example 
>string_vector_t nodes;
>
>init_string_vector(&nodes);
> 
>int ret = hconf_get_batch_conf(path, &nodes, NULL);
>
>assert(HCONF_OK == ret);
> 
>for (i = 0; i < nodes.count; i++)
>
>{cout << nodes.data[i] << endl;}
> 
>destroy_string_vector(&nodes_key); 

### **hconf_get_host**

`int hconf_get_host(const char *path, char *buf, unsigned int buf_len, const char *idc);`

Description
>get one available service

Parameters
>path - key of configuration.
>
>buf - out parameter, keep the service
>
>buf_len - lenghth of buf
>
>idc - from which idc to get the value，get from local idc if idc is NULL

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_NOT_FOUND if configuration is not exists
 
Example 
>char host[HCONF_HOST_BUF_MAX_LEN] = {0};
>
>int ret = hconf_get_host(path, host, sizeof(host), NULL);
>
>assert(HCONF_OK == ret);   

---
### **Data structure related functions**

----
``` c
typedef struct
  {
      int count;      // the number of services
      char **data;    // the array of services
  } string_vector_t;
```
### **init_string_vector**

`int init_string_vector(string_vector_t *nodes);`

Description
>initial array for keeping services
>
>**Tips:** the function should be called before calling hconf_get_batchkeys or hconf_get_allhosts

Parameters
>nodes - out parameter,  the array for keeping batch keys or services

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_PARAM if nodes is null
 
Example 
>string_vector_t bnodes_key;
>
>init_string_vector(&bnodes_key);

### **destroy_string_vector**

`int destroy_string_vector(string_vector_t *nodes);`

Description
>destroy the array for keeping batch keys or services
>
>**Tips:** remember to call this function after the last use of string_vector_t

Parameters
>nodes - out parameter,  hconf_batch_nodes keeping batch keys or services

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_PARAM if nodes is null
 
Example 
>hconf_batch_nodes nodes;
>
>destroy_string_vector(&nodes);


----

``` c
typedef struct hconf_node
  {
      char *key;
      char *value;
  } hconf_node;

  typedef struct hconf_batch_nodes
  {
      int count;
      hconf_node *nodes;
  } hconf_batch_nodes;
```

### **init_hconf_batch_nodes**

`int init_hconf_batch_nodes(hconf_batch_nodes *bnodes);`

Description
>initial nodes array for keeping batch conf
>
>**Tips:** the function should be called before calling hconf_get_batchconf

Parameters
>bnodes - out parameter,  hconf_batch_nodes to keeping batch nodes

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_PARAM if nodes is null
 
Example 
>hconf_batch_nodes bnodes;
>
>init_hconf_batch_nodes(&bnodes);  

### **destroy_hconf_batch_nodes**

`int destroy_hconf_batch_nodes(hconf_batch_nodes *bnodes);`

Description
>destroy the nodes array for keeping batch conf
>
>**Tips:** remember to call this function after the last use of hconf_batch_nodes

Parameters
>bnodes - out parameter,  hconf_batch_nodes keeping batch nodes

Return Value
>HCONF_OK if success,  others if failed. HCONF_ERR_PARAM if nodes is null
 
Example
>hconf_batch_nodes bnodes;
>
>destroy_hconf_batch_nodes(&bnodes);




## Shell Command

### Usage:

``` c
hconf command key [idc]
command: can be one of below commands:
   get_conf        : get configure value
   get_host        : get one service
   get_allhost     : get all services available
   get_batch_keys  : get all children keys
   
key    : the path of your configure items
idc    : query from current idc if be omitted
```
### Example:

``` shell
       hconf get_conf "demo/conf"
       hconf get_conf "demo/conf" "test"
       
       hconf get_host "demo/hosts"
       hconf get_host "demo/hosts" "test"
       
       hconf get_allhost "demo/hosts"
       hconf get_allhost "demo/hosts" "test"

	   hconf get_conf "demo/batch"
       hconf get_conf "demo/batch" "test"
```