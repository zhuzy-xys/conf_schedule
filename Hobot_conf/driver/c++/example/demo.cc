#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <hconf/hconf.h>

using namespace std;

int main()
{
    int ret = 0;
    int i = 0;

    // Init the hconf env
    ret = hconf_init();
    if (HCONF_OK != ret)
    {
        cout << "hconf init error! ret:" << ret << endl;
        return ret;
    }

    const char *path = "/demo/test/confs/conf1/conf11";
    char value[HCONF_CONF_BUF_MAX_LEN];
    char host[HCONF_HOST_BUF_MAX_LEN] = {0};

    // Get the conf
    ret = hconf_get_conf(path, value, sizeof(value), NULL);
    if (HCONF_OK != ret)
    {
        cout << "hconf get conf failed " << endl;
        hconf_destroy();
        return ret;
    }
    cout << "path: " << path << endl;
    cout << "value: " << value << endl << endl;

    /***********************************/
    // Get Batch conf with key and value
    /***********************************/
    path = "/demo/test/confs/conf1";
    hconf_batch_nodes bnodes;

    init_hconf_batch_nodes(&bnodes);
    ret = hconf_get_batch_conf(path, &bnodes, NULL);
    if (HCONF_OK != ret)
    {
        cout << "hconf get batch conf failed! " << endl;
        hconf_destroy();
        return ret;
    }
    cout << "children of parent path: " << path << endl;
    if (bnodes.count == 0)
        cout << "children's number is 0" << endl;

    for (i = 0; i < bnodes.count; i++)
    {
        cout << bnodes.nodes[i].key << " : " << bnodes.nodes[i].value;
        if (strcmp(bnodes.nodes[i].value, "") == 0)
        {
            cout << "\033[35;1mempty\033[0m" << endl;
        }
        else
        {
            cout << endl;
        }
    }
    destroy_hconf_batch_nodes(&bnodes);
    cout << endl;

    /***********************************/
    // Get Batch keys
    /***********************************/
    string_vector_t bnodes_key;

    init_string_vector(&bnodes_key);
    ret = hconf_get_batch_keys(path, &bnodes_key, NULL);
    if (HCONF_OK != ret)
    {
        cout << "hconf get batch conf's key failed! " << endl;
        hconf_destroy();
        return ret;
    }
    cout << "children of parent path: " << path << endl;
    if (bnodes_key.count == 0)
        cout << "children's number is 0" << endl;

    for (i = 0; i < bnodes_key.count; i++)
    {
        cout << bnodes_key.data[i] << endl;
    }
    destroy_string_vector(&bnodes_key);
    cout << endl;

    /***********************************/
    // Get the services
    /***********************************/
    path = "/demo/test/hosts/host1";
    string_vector_t nodes = {0};
    ret = init_string_vector(&nodes);
    if (HCONF_OK != ret)
    {
        cout << "init string vector failed" << endl;
        hconf_destroy();
        return ret;
    }
    //ret = hconf_get_allhost(path, &nodes, NULL);
    ret = hconf_get_allhost(path, &nodes, NULL);
    if (HCONF_OK != ret)
    {
        cout << "hconf get services failed! " << endl;
        hconf_destroy();
        return ret;
    }
    cout << "number of services of path: " << path << " is " << nodes.count << "; services are: " << endl;
    for (i = 0; i < nodes.count; i++)
    {
        cout << nodes.data[i] << endl;
    }
    destroy_string_vector(&nodes);
    cout << endl;
    
    /***********************************/
    // Get one service of path
    /***********************************/
    //ret = hconf_get_host(path, host, sizeof(host), NULL);
    ret = hconf_get_host(path, host, sizeof(host), NULL);
    if (HCONF_OK != ret)
    {
        cout << "hconf get service failed!" << endl;
        hconf_destroy();
        return ret;
    }
    cout << "service of path: " << path << " is below: " << endl;
    cout << host << endl << endl;

    // Get the hconf version
    const char *qv = hconf_version();
    cout << "hconf version: " << qv << endl;

    // Destroy the hconf env
    ret = hconf_destroy();
    if (HCONF_OK != ret)
    {
        cout << "hconf destroy failed" << endl;
        return -1;
    }

    return 0;
}
