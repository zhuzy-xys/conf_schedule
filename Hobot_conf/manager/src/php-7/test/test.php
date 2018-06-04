<?php
$zk = new HConfZK("127.0.0.1:2181");
$service_key = "/hconf_manager/services/2";

$services = array(
    "10.16.15.235:80" => HCONF_STATUS_UP,
    "10.16.15.235:81" => HCONF_STATUS_OFFLINE,
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
    "10.16.15.235:83" => HCONF_STATUS_UP
);

//nodeSet
$conf_key = "/hconf_manager/demo/3";

assert(0 === $zk->nodeDelete($conf_key));
assert(0 === $zk->nodeSet($conf_key,"value1"));
assert("value1" == $zk->nodeGet($conf_key));

assert(0 === $zk->nodeSet($conf_key,"value2"));
assert("value2" == $zk->nodeGet($conf_key));

assert(0 !== $zk->nodeSet("","value2"));
assert(0 !== $zk->nodeSet("/","value2"));

$large_str = str_repeat("a", 1024*1024 - 200);
assert(0 === $zk->nodeSet($conf_key, $large_str));
assert($large_str == $zk->nodeGet($conf_key));

//node Delete
assert(0 === $zk->nodeDelete($conf_key));
assert(0 !== $zk->nodeDelete("hconf_manager"));
assert(0 !== $zk->nodeDelete(""));

//serviceSet
$service_key = "/hconf_manager/services/2";

$services_empty = array();
assert(0 !== $zk->servicesSet($service_key, $services_empty));

$services_error1 = array(
    "10.16.15.235:80" => 1111111,
    "10.16.15.235:81" => HCONF_STATUS_OFFLINE,
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
    "10.16.15.235:83" => HCONF_STATUS_UP
);
assert(0 !== $zk->servicesSet($service_key, $services_error1));
$services_error2 = array(
    "10.16.15.235:80" => -1,
    "10.16.15.235:81" => HCONF_STATUS_OFFLINE,
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
    "10.16.15.235:83" => HCONF_STATUS_UP
);
assert(0 !== $zk->servicesSet($service_key, $services_error2));
assert(0 !== $zk->servicesSet("", $services));
assert(0 !== $zk->servicesSet("", $services));
assert(0 !== $zk->servicesSet("/", $services));
assert(0 === $zk->servicesSet($service_key, $services));
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

$services_mino = array(
    "10.16.15.235:80" => HCONF_STATUS_OFFLINE,
    "10.16.15.235:81" => HCONF_STATUS_UP,
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
);
assert(0 === $zk->servicesSet($service_key, $services_mino));
assert($services_mino === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services_mino) === $zk->servicesGet($service_key));

$services_more = array(
    "10.16.15.235:80" => HCONF_STATUS_OFFLINE,
    "10.16.15.235:81" => HCONF_STATUS_UP,
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
    "10.16.15.235:88" => HCONF_STATUS_UP,
);
assert(0 === $zk->servicesSet($service_key, $services_more));
assert($services_more === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services_more) === $zk->servicesGet($service_key));

$services_change = array(
    "10.16.15.235:82" => HCONF_STATUS_DOWN,
    "10.16.15.235:89" => HCONF_STATUS_UP,
);
assert(0 === $zk->servicesSet($service_key, $services_change));
assert($services_change === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services_change) === $zk->servicesGet($service_key));

assert(0 === $zk->servicesSet($service_key, $services));
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceUp
$cg_service = "10.16.15.235:81";
assert(0 !== $zk->serviceUp("", $cg_service));
assert(0 !== $zk->serviceUp("/", $cg_service));
assert(0 === $zk->serviceUp($service_key, $cg_service));
$services[$cg_service] = HCONF_STATUS_UP;
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceDown
$cg_service = "10.16.15.235:81";
assert(0 !== $zk->serviceDown("", $cg_service));
assert(0 !== $zk->serviceDown("/", $cg_service));
assert(0 === $zk->serviceDown($service_key, $cg_service));
$services[$cg_service] = HCONF_STATUS_DOWN;
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceOffline
$cg_service = "10.16.15.235:81";
assert(0 !== $zk->serviceOffline("", $cg_service));
assert(0 !== $zk->serviceOffline("/", $cg_service));
assert(0 === $zk->serviceOffline($service_key, $cg_service));
$services[$cg_service] = HCONF_STATUS_OFFLINE;
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceAdd
$cg_service = "10.16.15.235:87";
assert(0 === $zk->serviceAdd($service_key, $cg_service, HCONF_STATUS_OFFLINE));
assert(0 !== $zk->serviceAdd("", $cg_service, HCONF_STATUS_OFFLINE));
assert(0 !== $zk->serviceAdd("/", $cg_service, HCONF_STATUS_OFFLINE));
assert(0 !== $zk->serviceAdd($service_key, $cg_service, 3));
assert(0 !== $zk->serviceAdd($service_key, $cg_service, -1));
assert(0 !== $zk->serviceAdd($service_key, $cg_service, 11111111111111111113));
$services[$cg_service] = HCONF_STATUS_OFFLINE;
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceDelete
$cg_service = "10.16.15.235:87";
assert(0 !== $zk->serviceDelete("", $cg_service));
assert(0 !== $zk->serviceDelete("/", $cg_service));
assert(0 === $zk->serviceDelete($service_key, $cg_service));
unset($services[$cg_service]);
assert($services === $zk->servicesGetWithStatus($service_key));
assert(array_keys($services) === $zk->servicesGet($service_key));

//serviceClear
assert(0 !== $zk->serviceClear(""));
assert(0 !== $zk->serviceClear("/"));
assert(0 === $zk->serviceClear($service_key));
assert($services_empty == $zk->servicesGetWithStatus($service_key));

//list listWithValue
$children = array(
    "child1" => "value1",
    "child2" => "value2",
    "child3" => "value3",
    "child4" => "value4",
    "child5" => "value5"
);
$parent_path = "hconf_manager/demo/1";
foreach ($children as $c_key => $c_val)
{
    assert(0 === $zk->nodeSet("$parent_path/$c_key", $c_val));
}
assert($children === $zk->listWithValue($parent_path));
assert(array_keys($children) === $zk->list($parent_path));

$children = array(
    "child1" => "value1",
    "child2" => 1,
    "child3" => "value3",
    "child4" => "value4",
    "child5" => "value5",
);
$parent_path = "hconf_manager/demo/1";
foreach ($children as $c_key => $c_val)
{
    assert(0 === $zk->nodeSet("$parent_path/$c_key", $c_val));
}
$children["child2"] = "1";
assert($children === $zk->listWithValue($parent_path));
assert(array_keys($children) === $zk->list($parent_path));

//gray rollback
$gray_nodes = array(
    "hconf_manager/gray/1" => "value1",
    "/hconf_manager/gray/2/" => "value2",
    "hconf_manager/gray/3/" => "value3",
    "hconf_manager/gray/4" => "value4",
    "hconf_manager/gray/5" => "value5"
);

foreach ($gray_nodes as $path => $val)
{
    assert(0 === $zk->nodeSet("$path", "value"));
}
$machines = array(
    "HelloDawndeMacBook-Pro.local",
    "HelloDawndeMacBook-Pro.local1"
);

$gray_empty = array();

$gray_noexit_node = array(
    "hconf_manager/gray/1" => "value1",
    "/hconf_manager/gray/2/" => "value2",
    "hconf_manager/gray/3/" => "value3",
    "hconf_manager/gray/6" => "value6"
);

$gray_index_key = array(
    "hconf_manager/gray/1",
    "/hconf_manager/gray/2/",
    "hconf_manager/gray/3/",
    "hconf_manager/gray/3"
);


$gray_id = $zk->grayBegin($gray_empty, $machines);
assert(NULL === $gray_id);

$gray_id = $zk->grayBegin($gray_noexit_node, $machines);
assert(NULL === $gray_id);

$gray_id = $zk->grayBegin($gray_index_key, $machines);
assert(NULL === $gray_id);

$machines_empty = array();

$machines_string = array(
    "hconf_manager/gray/1" => "1",
    "/hconf_manager/gray/2/" => "2",
);

$gray_id = $zk->grayBegin($gray_nodes, $machines_empty);
assert(NULL === $gray_id);

$gray_id = $zk->grayBegin($gray_nodes, $machines_string);
assert(NULL === $gray_id);

$gray_id = $zk->grayBegin($gray_nodes, $machines);
assert(NULL != $gray_id);

$gray_id_tmp = $zk->grayBegin($gray_nodes, $machines); //machine already in gray
assert(NULL === $gray_id_tmp);

$backlink = "";
foreach ($machines as $m)
{
    $backlink .= "$m;";
    assert($gray_id == $zk->nodeGet("/hconf/__hconf_notify/client/$m"));
}
assert($backlink == $zk->nodeGet("/hconf/__hconf_notify/backlink/$gray_id"));

assert(0 === $zk->grayRollback($gray_id));
foreach ($machines as $m)
{
    assert(NULL === $zk->nodeGet("/hconf/__hconf_notify/client/$m"));
}
assert(NULL === $zk->nodeGet("/hconf/__hconf_notify/backlink/$gray_id"));
foreach ($gray_nodes as $path => $val)
{
    assert($val != $zk->nodeGet("$path"));
}


//gray commit
$gray_nodes = array(
    "/hconf_manager/gray/1" => "value1",
    "/hconf_manager/gray/2" => "value2",
    "/hconf_manager/gray/3" => "value3",
    "/hconf_manager/gray/4" => "value4"
);

$machines = array(
    "HelloDawndeMacBook-Pro.local",
    "HelloDawndeMacBook-Pro.local1"
);
$gray_id = $zk->grayBegin($gray_nodes, $machines);
$backlink = "";
foreach ($machines as $m)
{
    $backlink .= "$m;";
    assert($gray_id == $zk->nodeGet("/hconf/__hconf_notify/client/$m"));
}
assert($backlink == $zk->nodeGet("/hconf/__hconf_notify/backlink/$gray_id"));

assert(0 === $zk->grayCommit($gray_id));
foreach ($machines as $m)
{
    assert(NULL === $zk->nodeGet("/hconf/__hconf_notify/client/$m"));
}
assert(NULL === $zk->nodeGet("/hconf/__hconf_notify/backlink/$gray_id"));
foreach ($gray_nodes as $path => $val)
{
    assert($val == $zk->nodeGet("$path"));
}

$gray_id = "GRAYID1499757386-759608";
$gray_nodes = $zk->grayContent($gray_id);
assert(-1 == $gray_nodes);

echo "FINISHED!" . PHP_EOL;
?>
