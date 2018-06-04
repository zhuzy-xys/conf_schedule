#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cgic.h"
#include "hconf_page.h"
#include "hconf_zk.h"
#include "hconf_config.h"

#include <string>
#include <vector>
using namespace std;

string path, idc, value;
vector<string> idcs, children;
HConfZK hconfZk;
const int HCONF_VALUE_BUFFER_SIZE = 1024*1024;

static void HandleDeleteNode();
static void HandleModifyNode();
static void ShowPage();
static void InitialPage();
static void InitialZk();
static int Initial();
static int GetNodeValue();
static int GetChildList();
static void PrintInfo(string err);

int cgiMain() {
	cgiHeaderContentType("text/html");
    if (0 != Initial()) return -1;
        	
    // Check path and format it
    string tmp_path;
    if (!path.empty() && HCONF_OK != hconfZk.zk_path(path, tmp_path))
    {
        PrintInfo("Incorrect path!");
    }
    path.assign(tmp_path);

    if (!path.empty())
    {
        InitialZk();
        // If a submit button has already been clicked, act on the submission of the form.
        if ((cgiFormSubmitClicked("modify_node") == cgiFormSuccess))
        {
            HandleModifyNode();
        }
        else if ((cgiFormSubmitClicked("delete_node") == cgiFormSuccess))
        {
            HandleDeleteNode();
        }

        InitialPage();
    }

    // Now show the form
    ShowPage();

    hconf_destroy_conf_map();
    hconfZk.zk_close();
	return 0;
}

static void HandleDeleteNode()
{
    // Delete node
    switch (hconfZk.zk_node_delete(path))
    {
        case HCONF_OK:
            break;
        case HCONF_ERR_ZOO_NOTEMPTY:
            PrintInfo("Failed! Children already exist under:" + path);
            return;
        default:
            PrintInfo("Failed to delete node: " + path);
            return;
    }
    // Show parent node
    path.resize(path.find_last_of("/"));
    PrintInfo("Success!");
}

static void HandleModifyNode()
{
	char *raw_buffer = (char *)malloc((HCONF_VALUE_BUFFER_SIZE + 1) * sizeof(char));
	cgiFormString("value", raw_buffer, HCONF_VALUE_BUFFER_SIZE + 1);
    string new_value(raw_buffer);
    free(raw_buffer);

    // Modify node value
    if (HCONF_OK != hconfZk.zk_node_set(path, new_value))
    {
        PrintInfo("Failed to add or modify node: " + path );
        return;
    }
    value.assign(new_value);
    PrintInfo("Success!");
}

static void PrintInfo(string err)
{
	fprintf(cgiOut, "<script>alert(\"%s\");</script>\n", err.c_str());
}

static void InitialZk()
{
	char raw[256] = {0};
    // Current idc
	cgiFormStringNoNewlines("idc", raw, sizeof(raw));
    idc.assign(raw);

    // Initial zookeeper
    string host;
    if (HCONF_OK != get_host(idc, host)) 
    {
        PrintInfo("Failed find idc :" + idc);
        return;
    }
    if(HCONF_OK != hconfZk.zk_init(host))
        PrintInfo("Failed to init hconf server environment!");
}

static void InitialPage()
{
    // Get value from ZooKeeper
    if (0 != GetNodeValue())
        return;

    // Get children list
    GetChildList();
}


static int Initial()
{
    // Load idc
    if (HCONF_OK != hconf_load_conf())
    {
        PrintInfo("Failed to load idc file!");
        return -1;
    }

    const map<string, string> idc_hosts = get_idc_map(); 
    map<string, string>::const_iterator it = idc_hosts.begin();
    for (; it != idc_hosts.end(); ++it)
    {
       idcs.push_back(it->first);
    }

    // Current path
	char raw[1024] = {0};
    cgiFormStringNoNewlines("path", raw, sizeof(raw));
    path.assign(raw);
    return 0;
}

static int GetNodeValue()
{
    string buf = "";
    int ret = 0;
    switch(ret = hconfZk.zk_node_get(path, buf))
    {
        case HCONF_OK:
            value.assign(buf);
            break;
        case HCONF_ERR_ZOO_NOT_EXIST:
            PrintInfo("Failed to get value, Node not exist: " + path);
            break;
        default:
            PrintInfo("Failed to get node value for node: " + path);
    }
    return (HCONF_OK == ret) ? 0 : -1;
}

static int GetChildList()
{
    
    set<string> list;
    set<string>::iterator it;
    int ret = 0;
    switch (ret = hconfZk.zk_list(path, list))
    {
        case HCONF_OK:
            for (it = list.begin(); it != list.end(); ++it)
            {
                children.push_back(path + "/" + (*it));
            }
            break;
        case HCONF_ERR_ZOO_NOT_EXIST:
            PrintInfo("Failed to list children Node not exist: " + path);
            break;
        default:
            PrintInfo("Failed to list children for node: " + path);
    }
    return (HCONF_OK == ret) ? 0 : -1;
}

static void ShowPage()
{
    // Page is defined in hconf_page.cc
    generate(path, value, idc, idcs, children, cgiScriptName);
}
