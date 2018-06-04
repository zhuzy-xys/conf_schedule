QConf Go Doc
=====
## Usage
- Download the code and add its location to `GOPATH` environment variable.
- Install go_hconf
```shell
go install infra/go_hconf
```
- Import hconf package in your code file.
``` shell
	import "infra/go_hconf"
```


## API Doc


### QConf access functions 

----

### GetConf

`value, err := go_hconf.GetConf(path, idc)`

Description
>get configure value

Parameters
>path - key of configuration.
>
>idc - from which idc to get the value，empty string if want to get from local idc

Return Value
> value of the configuation
>
> err， shold be checked for nil before use of value
 
 
Example 
``` go
  value, err_conf := go_hconf.GetConf(key, "")
  if err_conf != nil{
       if err_conf == go_hconf.ErrQconfNotFound{
            fmt.Printf("Is ErrQconfNotFound\n")
       }
       fmt.Println(err_conf)
   }
```

### GetBatchKeys()

`batch_keys, err := go_hconf.GetBatchKeys(key, "")`

Description

>get all children nodes'key

Parameters
>path - key of configuration.
>
>idc - from which idc to get the keys，empty string if want to get from local idc

Return Value
>batch_keys: go slice of the node's keys
>
>err， shold be checked for nil before you use batch_keys 

Example 
``` go
    batch_keys, err_batch_keys := go_hconf.GetBatchKeys(key, "")
    if err_batch_keys != nil{
        fmt.Println(err_batch_keys)
    } 
```
 

### GetBatchConf

` batch_conf, err := go_hconf.GetBatchConf(key, "")`

Description
>get all children nodes' key and value

Parameters
>path - key of configuration.
>
>idc - from which idc to get the confs，empty string if want to get from local idc

Return Value
>batch_conf: go map of the children configuration
>
>err， shold be checked for nil before you use batch_conf 
 
 Example 
 ``` go
    batch_conf, err_batch_conf := go_hconf.GetBatchConf(key, "")
    if err_batch_conf != nil{
        fmt.Println(err_batch_conf)
    }
 ```
 

### GetAllHost

`hosts, err_hosts := go_hconf.GetAllHost(host_key, idc)`

Description
>get all available services under given key

Parameters
>path - key of configuration.
>
>idc - from which idc to get the hosts，empty string if want to get from local idc

Return Value
>go slice of all available services
>
>err， shold be checked for nil before you use batch_conf 

Example 
``` go
    hosts, err_hosts := go_hconf.GetAllHost(host_key, idc)
    if err_hosts != nil{
        fmt.Println(err_hosts)
    }
```

### GetHost

`host, err := go_hconf.GetHost(key, idc)`

Description
>get one available service

Parameters
>path - key of configuration.
>
>idc - from which idc to get the host，empty string if want to get from local idc

Return Value
>host, one available service
>
>err,shold be checked for nil before you use host

Example 
``` go
    host, err_host := go_hconf.GetHost(key, idc)
    if err_host != nil{
        fmt.Println(err_host)
    }
```

---
## Example

``` go

package main

import (
      "fmt"
      "infra/go_hconf"
)
func main(){
      value, err_conf := go_hconf.GetConf("/demo/test/confs/conf1/conf11", "")
      if err_conf != nil{
          fmt.Println(err_conf)
      } else {
          fmt.Printf("value is %v\n", value)
      }

      host, err_host := go_hconf.GetHost("/demo/test/hosts/host1", "")
      if err_host != nil{
          fmt.Println(err_host)
      } else {
          fmt.Printf("one host is %v\n", host)
      }

      hosts, err_hosts := go_hconf.GetAllHost("/demo/test/hosts/host1", "")
      if err_hosts != nil{
          fmt.Println(err_hosts)
      } else {
          for i := 0; i < len(hosts); i++ {
              cur := hosts[i]
              fmt.Println(cur)
          }
      }

      batch_conf, err_batch_conf := go_hconf.GetBatchConf("/demo/test/confs/conf1", "")
      if err_batch_conf != nil{
          fmt.Println(err_batch_conf)
      } else {
          fmt.Printf("%v\n", batch_conf)
      }

      batch_keys, err_batch_keys := go_hconf.GetBatchKeys("/demo/test/confs/conf1", "")
      if err_batch_keys != nil{
          fmt.Println(err_batch_keys)
      } else {
          fmt.Printf("%v\n", batch_keys)
      }

}
          
 ```
