HConf Java Doc
=====
## Build
- Modify Makefile as your situation
>JNI_DIR: %JAVA_HOME%/include/
>
>JNI_MD_DIR:%JAVA_HOME%/include/linux
>
>HCONF_HEAD_DIR: location of hconf head file, it will be /usr/local/include/hconf by default
>
>LIBOBJS: location of libhconf.a, it will be /usr/local/lib/libhconf.a by default

- compile and generate jar file

```
make

```

the jar file and document will be generated under hconf_jar directory

- Use the jar file by adding it in the classpath, and import two package below in your program

```
import net.qihoo.hconf.Qconf;
import net.qihoo.hconf.QconfException;

```



## API Doc


### HConf access functions 

----

### getConf

`String getConf(String path, String idc)`

Description
>get configure value

Parameters
>path - key of configuration.
>
>idc - Optional，from which idc to get the value，get from local idc if  omit

Return Value
>value of the configuation, throw Exception net.qihoo.hconf.QconfException if failed 
 
Example 
 >String value = Qconf.getConf("demo/confs"); 

### getBatchKeys()

`ArrayList<String> getBatchKeys(String path, String idc);`

Description

>get all children nodes'key

Parameters
>path - key of configuration.
>
>idc - Optional，from which idc to get the keys，get from local idc if  omit

Return Value
>python list of the node's keys, throw Exception net.qihoo.hconf.QconfException if failed 
 
Example 
 > ArrayList<String> keys = Qconf.getBatchKeys("demo/confs");
 

### getBatchConf

`Map<String, String> getBatchConf(String path, String idc);`

Description
>get all children nodes' key and value

Parameters
>path - key of configuration.
>
>idc - Optional， from which idc to get the children configurations，get from local idc if  omit

Return Value
>python dictionary of the children configuration, throw Exception net.qihoo.hconf.QconfException if failed 
 
 Example 
 >Map<String, String> confs = Qconf.getBatchConf("demo/confs");
 

### getAllHost

`ArrayList<String> getAllHost(String key, String idc);`

Description
>get all available services under given key

Parameters
>path - key of configuration.
>idc - Optional， from which idc to get the services，get from local idc if  omit

Return Value
>python list of all available services, throw Exception net.qihoo.hconf.QconfException if failed 

Example 
>ArrayList<String> hosts = Qconf.getAllHost("demo/hosts");

### getHost

`String host = Qconf.getHost(String key, String idc);`

Description
>get one available service

Parameters
>path - key of configuration.
>
>idc - Optional，from which idc to get the host，get from local idc if  omit

Return Value
>available host, throw Exception net.qihoo.hconf.QconfException if failed 
 
Example 
>String host = Qconf.getHost("demo/hosts");

---
## Example

``` java

		  try
          {
              String value = Qconf.getConf(key_conf);
              System.out.println(value);
          }
          catch(QconfException e)
          {
              e.printStackTrace();
          }
          
          // get Batch keys
          try
          {
              ArrayList<String> keys = Qconf.getBatchKeys(key_conf);
              for(String one_key: keys)
              {
                  System.out.println(one_key);
              }
          }
          catch(QconfException e)
          {
              e.printStackTrace();
          }
          
 ```
