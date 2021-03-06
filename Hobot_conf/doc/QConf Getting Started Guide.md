HConf Getting Started Guide
=====

## Introduction
HConf is a Distrubuted Configuration Management System!
A better replacement of the traditional configuration file. As designed, configuration items which is constantly accessed and modified should be completely separated with application code, and HConf is where it should be.

 - Changes to any configuration will be synchronised across all client
   machines, in seconds.
 - High query efficiency.
 - Convenient deployment and simple interface.
 - High robustness and  fault-tolerance.


## Install
The HConf is built using CMake (version 2.6 or newer).

On most systems you can build the library using the following commands:
``` shell
mkdir build && cd build
cmake ..
make
make install
```
Alternatively you can also point the CMake GUI tool to the CMakeLists.txt file.

To install the HConf you can specify the install prefix by setting:
``` shell
cmake .. -DCMAKE_INSTALL_PREFIX=/install/prefix
```

## Usage

- **Set up Zookeeper servers**, create or modify znode with Zookeeper Client

	 More information about Zookeper: [ZooKeeper Getting Started Guide](http://zookeeper.apache.org/doc/r3.3.3/zookeeperStarted.html)
	 
- **Register** the Zookeeper server address with HConf

``` shell
vi HCONF_INSTALL_PREFIX/conf/idc.conf
```
``` php
  # all the zookeeper host configuration.
  #[zookeeper]
  zookeeper.test=127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183 #zookeeper of idc 'test'
```
 - **Assign** local idc
``` shell
echo test > HCONF_INSTALL_PREFIX/conf/localidc #assign local idc to test
```
 - **Run** HConf

``` shell
cd HCONF_INSTALL_PREFIX/bin && sh agent-cmd.sh start
```
 - **Code** to access HConf
## Component
* **ZooKeeper** as the server, restore all configurations, so the limit data size of single configuration item is 1MB, since the size limit of Znode.
* **hconf_agent** is a daemon run on client machine, maintain data in share memory.
* **share memory**,  HConf use share memory as the local cache of all configuration items needed by current client.
* **interface** in different programming languages, chose your language for accessing of HConf.  


![enter image description here](http://ww2.sinaimg.cn/bmiddle/69a9c739jw1eqhuo29639j20iq0i8402.jpg "Component")


## Document
* [C Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20C%5CC%2B%2B%20Doc.md) - the C reference to HConf APIs
* [Java Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20Java%20Doc.md) - the Java reference to HConf APIs
* [Python Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20Python%20Doc.md) - the Python reference to HConf APIs，Python 2.4,  2.5,  2.6 or 2.7 is required. Python 3.x is not yet supported.
* [PHP Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20PHP%20Doc.md) -  the PHP reference to HConf APIs
* [Shell Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20C%5CC%2B%2B%20Doc.md) - the command hconf can be use in command line
* [LuaJit Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20LuaJit%20Doc.md) - the LuaJit reference to HConf APIs
* [Go Doc](https://github.com/Qihoo360/HConf/blob/master/doc/HConf%20Go%20Doc.md) - the Go reference to HConf APIs


## Example
* **C Exmaple**
``` c
	  // Init the hconf env
      ret = hconf_init();
      assert(HCONF_OK == ret);

      // Get Conf value
      char value[HCONF_CONF_BUF_MAX_LEN];
      ret = hconf_get_conf("/demo/node1", value, sizeof(value), NULL);
      assert(HCONF_OK == ret);

      // Get Batch keys
      string_vector_t bnodes_key;
      init_string_vector(&bnodes_key);

      ret = hconf_get_batch_keys("/demo/node2", &bnodes_key, NULL);
      assert(HCONF_OK == ret);

      int i = 0;
      for (i = 0; i < bnodes_key.count; i++)
      {
          // get keys from bnodes_key.data[i]
      }
      destroy_string_vector(&bnodes_key);

      // Destroy hconf env
      hconf_destroy();
```

* **Shell Exmaple**
``` shell
	   usage: hconf command key [idc]
       command: can be one of below commands:
                get_conf        : get configure value
                get_host        : get one service
                get_allhost     : get all services available
                get_batch_keys  : get all children keys
       key    : the path of your configure items
       idc    : query from current idc if be omitted
example:
       hconf get_conf "demo/conf"
       hconf get_conf "demo/conf" "test"
```
