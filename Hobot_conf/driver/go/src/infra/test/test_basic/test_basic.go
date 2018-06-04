package main

import (
    "fmt"
    "infra/go_hconf"
)

func main(){
    key := "/demo/test_my/test/test3"
    idc := "corp"
    value, err_conf := go_hconf.GetConf(key, "")
    if err_conf != nil{
        if err_conf == go_hconf.ErrQconfNotFound{
            fmt.Printf("Is ErrQconfNotFound\n")
        }
        fmt.Println(err_conf)
    } else {
        fmt.Printf("value of %v is %v\n", key, value)
    }

    host, err_host := go_hconf.GetHost(key, idc)
    if err_host != nil{
        fmt.Println(err_host)
    } else {
        fmt.Printf("one host of %v is %v\n", key, host)
    }

    hosts, err_hosts := go_hconf.GetAllHost(key, "")
    if err_hosts != nil{
        fmt.Println(err_hosts)
    } else {
        for i := 0; i < len(hosts); i++ {
            cur := hosts[i]
            fmt.Println(cur)
        }
    }

    batch_conf, err_batch_conf := go_hconf.GetBatchConf(key, "")
    if err_batch_conf != nil{
        fmt.Println(err_batch_conf)
    } else {
        fmt.Printf("%v\n", batch_conf)
    }

    batch_keys, err_batch_keys := go_hconf.GetBatchKeys(key, "")
    if err_batch_keys != nil{
        fmt.Println(err_batch_keys)
    } else {
        fmt.Printf("%v\n", batch_keys)
    }

    
    version, err_version := go_hconf.Version()
    if err_version != nil{
        fmt.Println(err_version)
    } else {
        fmt.Printf("Version : %v\n", version)
    }
        
}


