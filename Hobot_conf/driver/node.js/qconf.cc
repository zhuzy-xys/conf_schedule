#include <node.h>
#include <hconf.h>
#include <string>
#include <v8.h>

using namespace v8;  

//gte version
Handle<Value> HConf_version(const Arguments& args) {
    HandleScope scope;
    return scope.Close(String::New(HCONF_DRIVER_CC_VERSION));
}

//get_conf
Handle<Value> HConf_get_conf(const Arguments& args) {
    HandleScope scope;

    const char *path = NULL;
    const char *idc = NULL;
    char value[HCONF_CONF_BUF_MAX_LEN];
    int ret;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    String::Utf8Value v8Path(args[0]);
    path = *v8Path;

    std::string temp;
    if (args.Length() >= 2) {
        String::Utf8Value v8Idc(args[1]);
        temp = std::string(*v8Idc);
    }
    idc = temp.c_str();

    ret = hconf_get_conf(path, value, sizeof(value), idc);

    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("Get conf failed")));
        return scope.Close(Undefined());
    }
    return scope.Close(String::New(value));
}

//get children
Handle<Value> HConf_get_batch_keys(const Arguments& args) {
    HandleScope scope;

    const char *path = NULL;
    const char *idc = NULL;
    string_vector_t keys;
    int ret;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    String::Utf8Value v8Path(args[0]);
    path = *v8Path;

    std::string temp;
    if (args.Length() >= 2) {
        String::Utf8Value v8Idc(args[1]);
        temp = std::string(*v8Idc);
    }
    idc = temp.c_str();

    init_string_vector(&keys);
    ret = hconf_get_batch_keys(path, &keys, idc);

    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("Get children failed")));
        return scope.Close(Undefined());
    }

    Handle<Array> v8Arr = Array::New(keys.count);
    for (int i = 0; i < keys.count; ++i) {
        v8Arr->Set(i, String::New(keys.data[i]));
    }
    destroy_string_vector(&keys);
    return scope.Close(v8Arr);
}

//get children and their confs
Handle<Value> HConf_get_batch_conf(const Arguments& args) {
    HandleScope scope;

    const char *path = NULL;
    const char *idc = NULL;
    hconf_batch_nodes nodes;
    int ret;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    String::Utf8Value v8Path(args[0]);
    path = *v8Path;

    std::string temp;
    if (args.Length() >= 2) {
        String::Utf8Value v8Idc(args[1]);
        temp = std::string(*v8Idc);
    }
    idc = temp.c_str();

    init_hconf_batch_nodes(&nodes);
    ret = hconf_get_batch_conf(path, &nodes, idc);

    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("Get children and confs failed")));
        return scope.Close(Undefined());
    }

    Handle<Object> v8Obj = Object::New();
    for (int i = 0; i < nodes.count; ++i) {
        v8Obj->Set(String::New(nodes.nodes[i].key), String::New(nodes.nodes[i].value));
    }
    destroy_hconf_batch_nodes(&nodes);
    return scope.Close(v8Obj);
}

//get_host
Handle<Value> HConf_get_host(const Arguments& args) {
    HandleScope scope;

    const char *path = NULL;
    const char *idc = NULL;
    char value[HCONF_HOST_BUF_MAX_LEN];
    int ret;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    String::Utf8Value v8Path(args[0]);
    path = *v8Path;

    std::string temp;
    if (args.Length() >= 2) {
        String::Utf8Value v8Idc(args[1]);
        temp = std::string(*v8Idc);
    }
    idc = temp.c_str();
    ret = hconf_get_host(path, value, sizeof(value), idc);

    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("Get host failed")));
        return scope.Close(Undefined());
    }
    return scope.Close(String::New(value));
}

//get_allhost
Handle<Value> HConf_get_allhost(const Arguments& args) {
    HandleScope scope;

    const char *path = NULL;
    const char *idc = NULL;
    string_vector_t nodes;
    int ret;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    String::Utf8Value v8Path(args[0]);
    path = *v8Path;

    std::string temp;
    if (args.Length() >= 2) {
        String::Utf8Value v8Idc(args[1]);
        temp = std::string(*v8Idc);
    }
    idc = temp.c_str();
    
    init_string_vector(&nodes);
    ret = hconf_get_allhost(path, &nodes, idc);

    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("Get all host failed")));
        return scope.Close(Undefined());
    }

    Handle<Array> v8Arr = Array::New(nodes.count);
    for (int i = 0; i < nodes.count; ++i) {
        v8Arr->Set(i, String::New(nodes.data[i]));
    }
    destroy_string_vector(&nodes);
    return scope.Close(v8Arr);
}

void init(Handle<Object> target) {
    int ret = hconf_init();
    if (ret != HCONF_OK) {
        ThrowException(Exception::TypeError(String::New("HConf init failed")));
        return;
    }
    target->Set(String::NewSymbol("version"),
            FunctionTemplate::New(HConf_version)->GetFunction());

    target->Set(String::NewSymbol("get_conf"),
            FunctionTemplate::New(HConf_get_conf)->GetFunction());

    target->Set(String::NewSymbol("get_batch_keys"),
            FunctionTemplate::New(HConf_get_batch_keys)->GetFunction());

    target->Set(String::NewSymbol("get_batch_conf"),
            FunctionTemplate::New(HConf_get_batch_conf)->GetFunction());

    target->Set(String::NewSymbol("get_host"),
            FunctionTemplate::New(HConf_get_host)->GetFunction());

    target->Set(String::NewSymbol("get_allhost"),
            FunctionTemplate::New(HConf_get_allhost)->GetFunction());
}
NODE_MODULE(hconf, init)
