package go_hconf

/*
#cgo LDFLAGS: -lhconf -lm
#include <stdlib.h>
#include <stdio.h>

struct string_vector
{
    int count;      // the number of services
    char **data;    // the array of services
};
typedef struct string_vector string_vector_t;

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

int hconf_init();
int hconf_destroy();
int init_string_vector(string_vector_t *nodes);
int destroy_string_vector(string_vector_t *nodes);
int init_hconf_batch_nodes(hconf_batch_nodes *bnodes);
int destroy_hconf_batch_nodes(hconf_batch_nodes *bnodes);

int hconf_get_conf(const char *path, char *buf, int buf_len, const char *idc);
int hconf_get_allhost(const char *path, string_vector_t *nodes, const char *idc);
int hconf_get_host(const char *path, char *buf, int buf_len, const char *idc);
int hconf_get_batch_conf(const char *path, hconf_batch_nodes *bnodes, const char *idc);
int hconf_get_batch_keys(const char *path, string_vector_t *nodes, const char *idc);


*/
import "C"

import (
    "fmt"
    "unsafe"
    "reflect"
)

type Errno int

func (e Errno) Error() string{
    s := errText[e]
    if s == "" {
        return fmt.Sprintf("unknown errno %d", int(e))
    }
    return s
}

var errText = map[Errno]string{
    -1:     "Execute failure!",
    1:      "Error parameter!",
    2:      "Failed to malloc memory!",
    3:      "Failed to set share memory!",
    4:      "Failed to get zookeeper host!",
    5:      "Failed to get idc!",
    6:      "Buffer not enough!",
    7:      "Illegal data type!",
    8:      "Illegal data format!",
    10:     "Failed to find key on given idc!",
    11:     "Failed to open dump file!",
    12:     "Failed to open tmp dump file!",
    13:     "Failed to find key in dump!",
    14:     "Failed to rename dump!",
    15:     "Failed to write dump!",
    16:     "Same with the value in share memory!",
    20:     "Configure item error : out of range!",
    21:     "Configure item error : not number!",
    22:     "Configure item error : further characters exists!",
    30:     "Configure item error : invalid ip!",
    31:     "Configure item error : invalid port!",
    40:     "No message exist in message queue!",
    41:     "Length of message in the queue is too large!",
    71:     "Error hostname!",
}

var (
        ErrOther                error = Errno(-1)
        ErrQconfParam           error = Errno(1)
        ErrQconfMem             error = Errno(2)
        ErrQconfTblSet          error = Errno(3)
        ErrQconfGetHost         error = Errno(4)
        ErrQconfGetIdc          error = Errno(5)
        ErrQconfBufNotEnough    error = Errno(6)
        ErrQconfDataType        error = Errno(7)
        ErrQconfDataFormat      error = Errno(8)
        ErrQconfNotFound        error = Errno(10)
        ErrQconfOpenDump        error = Errno(11)
        ErrQconfOpenTmpDump     error = Errno(12)
        ErrQconfNotInDump       error = Errno(13)
        ErrQconfRenameDump      error = Errno(14)
        ErrQconfWriteDump       error = Errno(15)
        ErrQconfSameValue       error = Errno(16)
        ErrQconfOutOfRange      error = Errno(20)
        ErrQconfNotNumber       error = Errno(21)
        ErrQconfOtherCharacter  error = Errno(22)
        ErrQconfInvalidIp       error = Errno(30)
        ErrQconfInvalidPort     error = Errno(31)
        ErrQconfNoMessage       error = Errno(40)
        ErrQconfE2Big           error = Errno(41)
        ErrQconfHostname        error = Errno(71)
)

const (
    HCONF_DRIVER_GO_VERSION         = "1.2.2"
    HCONF_CONF_BUF_INIT_MAX_LEN     = 2 *1024
    HCONF_CONF_BUF_MAX_LEN          = 1024 *1024
    HCONF_CONF_BUF_MULTIPLE         = 8
    HCONF_HOST_BUF_MAX_LEN          = 256

    HCONF_OK                        = 0
    HCONF_ERR_BUF_NOT_ENOUGH        = 6
)

func init(){
    ret := C.hconf_init()
    if HCONF_OK != ret {
        panic(ret)
    }
}

func convertToGoSlice(nodes *C.string_vector_t) []string{
    length := int((*nodes).count)
    hdr := reflect.SliceHeader{
        Data: uintptr(unsafe.Pointer((*nodes).data)),
        Len:  length,
        Cap:  length,
    }
    charp_nodes := *(*[]*C.char)(unsafe.Pointer(&hdr))
    go_nodes := []string{}
    for i := 0; i < length; i++ {
        go_host := C.GoString(charp_nodes[i])
        go_nodes = append(go_nodes, go_host)
    }
    return go_nodes
}

func convertToGoMap(bnodes *C.hconf_batch_nodes) map[string]string{
    length := int((*bnodes).count)
    hdr := reflect.SliceHeader{
        Data: uintptr(unsafe.Pointer((*bnodes).nodes)),
        Len:  length,
        Cap:  length,
    }
    hconf_nodes := *(*[]C.hconf_node)(unsafe.Pointer(&hdr))
    go_nodes := map[string]string{}
    for i := 0; i < length; i++ {
        go_key := C.GoString(hconf_nodes[i].key)
        go_value := C.GoString(hconf_nodes[i].value)
        go_nodes[go_key] = go_value;
    }
    return go_nodes
}


func GetConf(key string, idc string) (string, error){
    c_key := C.CString(key)
    defer C.free(unsafe.Pointer(c_key))
    var ret int
    var c_ptr_value *C.char
    slice_length := HCONF_CONF_BUF_INIT_MAX_LEN

    for ret = HCONF_ERR_BUF_NOT_ENOUGH; ret == HCONF_ERR_BUF_NOT_ENOUGH && slice_length <= HCONF_CONF_BUF_MAX_LEN; slice_length *= HCONF_CONF_BUF_MULTIPLE{
        c_value := make([]C.char, slice_length)
        c_ptr_value = (*C.char)(unsafe.Pointer(&(c_value[0])))

        if idc == "" {
            ret = int(C.hconf_get_conf(c_key, c_ptr_value, C.int(slice_length), nil))
        } else {
            c_idc := C.CString(idc)
            defer C.free(unsafe.Pointer(c_idc))
            ret = int(C.hconf_get_conf(c_key, c_ptr_value, C.int(slice_length), c_idc))
        }
    }
    if HCONF_OK != ret {
        cur_err := Errno(ret)
        return "", cur_err
    }
    go_value := C.GoString(c_ptr_value)
    return go_value, nil
}

func GetHost(key string, idc string) (string, error){
    c_key := C.CString(key)
    defer C.free(unsafe.Pointer(c_key))
    var c_host[HCONF_HOST_BUF_MAX_LEN]C.char
    c_ptr_host := (*C.char)(unsafe.Pointer(&(c_host[0])))

    var ret int
    if idc == "" {
        ret = int(C.hconf_get_host(c_key, c_ptr_host, HCONF_HOST_BUF_MAX_LEN, nil))
    } else {
        c_idc := C.CString(idc)
        defer C.free(unsafe.Pointer(c_idc))
        ret = int(C.hconf_get_host(c_key, c_ptr_host, HCONF_HOST_BUF_MAX_LEN, c_idc))
    }
    if HCONF_OK != ret {
        cur_err := Errno(ret)
        return "", cur_err
    }
    go_host := C.GoString(c_ptr_host)
    return go_host, nil
}

func GetAllHost(key string, idc string) ([]string, error){
    c_key := C.CString(key)
    defer C.free(unsafe.Pointer(c_key))
    var nodes C.string_vector_t
    init_ret := C.init_string_vector(&nodes)
    if HCONF_OK != init_ret{
        cur_err := Errno(init_ret)
        return nil, cur_err
    }
    defer C.destroy_string_vector(&nodes)

    var ret int
    if idc == "" {
        ret = int(C.hconf_get_allhost(c_key, &nodes, nil))
    } else {
        c_idc := C.CString(idc)
        defer C.free(unsafe.Pointer(c_idc))
        ret = int(C.hconf_get_allhost(c_key, &nodes, c_idc))
    }
    if HCONF_OK != ret {
        cur_err := Errno(ret)
        return nil, cur_err
    }

    go_nodes := convertToGoSlice(&nodes)
    return go_nodes, nil
}

func GetBatchConf(key string, idc string) (map[string]string, error){
    c_key := C.CString(key)
    defer C.free(unsafe.Pointer(c_key))
    var bnodes C.hconf_batch_nodes
    init_ret := C.init_hconf_batch_nodes(&bnodes)
    if HCONF_OK != init_ret{
        cur_err := Errno(init_ret)
        return nil, cur_err
    }
    defer C.destroy_hconf_batch_nodes(&bnodes)

    var ret int
    if idc == "" {
        ret = int(C.hconf_get_batch_conf(c_key, &bnodes, nil))
    } else {
        c_idc := C.CString(idc)
        defer C.free(unsafe.Pointer(c_idc))
        ret = int(C.hconf_get_batch_conf(c_key, &bnodes, c_idc))
    }
    if HCONF_OK != ret {
        cur_err := Errno(ret)
        return nil, cur_err
    }

    go_nodes := convertToGoMap(&bnodes)
    return go_nodes, nil
}


func GetBatchKeys(key string, idc string) ([]string, error){
    c_key := C.CString(key)
    defer C.free(unsafe.Pointer(c_key))
    var nodes C.string_vector_t
    init_ret := C.init_string_vector(&nodes)
    if HCONF_OK != init_ret{
        cur_err := Errno(init_ret)
        return nil, cur_err
    }
    defer C.destroy_string_vector(&nodes)

    var ret int
    if idc == "" {
        ret = int(C.hconf_get_batch_keys(c_key, &nodes, nil))
    } else {
        c_idc := C.CString(idc)
        defer C.free(unsafe.Pointer(c_idc))
        ret = int(C.hconf_get_batch_keys(c_key, &nodes, c_idc))
    }
    if HCONF_OK != ret {
        cur_err := Errno(ret)
        return nil, cur_err
    }

    go_nodes := convertToGoSlice(&nodes)
    return go_nodes, nil
}

func Version() (string, error){
    return HCONF_DRIVER_GO_VERSION, nil;
}
