<?php
require dirname(__FILE__).'/QconfConfig.php';

class Qconf
{
    public static function getHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getHost($name, $idc,$flags);
        case "0.3.1":
            return Qconf0_3_1::getHost($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getHost($name, $idc, $flags);
        default:
            return Qconf_old::getHost($name);
        }
    }

    public static function getAllHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getAllHost($name, $idc, $flags);
        case "0.3.1":
            return Qconf0_3_1::getAllHost($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getAllHost($name, $idc, $flags);
        default:
            return Qconf_old::getAllHost($name);
        }
    }

    public static function getConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getConf($name, $idc, $flags);
        case "0.3.1":
            return Qconf0_3_1::getConf($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getConf($name, $idc, $flags);
        default:
            return Qconf_old::getConf($name);
        }
    }

    public static function getMultConf($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getMultConf($names, $idc, $flags);
        case "0.3.1":
            return Qconf0_3_1::getMultConf($names, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getMultConf($names, $idc, $flags);
        default:
            return Qconf_old::getMultConf($names);
        }
    }

    public static function getBatchConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return Qconf0_4_0::getBatchConf($name, $idc, $flags);
    }

    public static function getBatchKeys($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return Qconf0_4_0::getBatchKeys($name, $idc, $flags);
    }

    public static function getHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getHostNative($name, $idc, $flags);
        case "0.3.1":
            return Qconf0_3_1::getHostNative($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getHostNative($name, $idc, $flags);
        default:
            return Qconf_old::getHostNative($name);
        }
    }

    public static function getAllHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "1.0.0":
        case "0.4.0":
            return Qconf0_4_0::getAllHostNative($name, $idc, $flags);
        case "0.3.1":
            return Qconf0_3_1::getAllHostNative($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getAllHostNative($name, $idc, $flags);
        default:
            return Qconf_old::getAllHostNative($name);
        }
    }

    public static function getConfNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "0.3.1":
            return Qconf0_3_1::getConfNative($name, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getConfNative($name, $idc, $flags);
        default:
            return Qconf_old::getConfNative($name, $idc, $flags);
        }
    }

    public static function getMultConfNative($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $version = phpversion("hconf");
        switch ($version)
        {
        case "0.3.1":
            return Qconf0_3_1::getMultConfNative($names, $idc, $flags);
        case "0.3.0":
            return Qconf0_3_0::getMultConfNative($names, $idc, $flags);
        default:
            return Qconf_old::getMultConfNative($names);
        }
    }

}

class Qconf0_4_0
{
    public static function getHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHost($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::GetChild(trim($name), $idc, $flags);
    }

    public static function getConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::Get(trim($name), $idc, $flags);
    }

    public static function getMultConf($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name)
        {
            if (strlen($name) > 0)
            {
                $result[] = self::getConf($name, $idc, $flags);
            }
            else
            {
                $result[] = "";
            }
        }
        return $result;
    }

    public static function getBatchConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::GetBatchConf(trim($name), $idc, $flags);
    }

    public static function getBatchKeys($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::GetBatchKeys(trim($name), $idc, $flags);
    }

    public static function getHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHostNative($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::GetBatchKeysNative(trim($name), $idc, $flags);
    }
}

class Qconf0_3_1
{
    private static function getIdc()
    {
        $uname = posix_uname();
        $hostname = $uname["nodename"];
        $words = explode(".", $hostname);
        $cluster = $words[count($words)-3];
        return $cluster;
    }

    public static function getHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHost($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $node = QconfConfig::getNodePath($name);
        return HConfig::GetChild($node, $idc, $flags);
    }

    public static function getConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $node = QconfConfig::getNodePath($name);
        return HConfig::Get($node, $idc, $flags);
    }

    public static function getMultConf($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name)
        {
            $name = trim($name);
            if (strlen($name) > 0)
            {
                $result[] = self::getConf($name, $idc, $flags);
            }
            else
            {
                $result[] = "";
            }
        }
        return $result;
    }

    public static function getHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHostNative($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::GetChild($name, $idc, $flags);
    }

    public static function getConfNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        return HConfig::Get($name, $idc, $flags);
    }

    public static function getMultConfNative($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name)
        {
            $name = trim($name);
            if (strlen($name) > 0)
            {
                $result[] = self::getConfNative($name, $idc, $flags);
            }
            else
            {
                $result[] = "";
            }
        }
        return $result;
    }
}


class Qconf0_3_0
{
    private static function getZk()
    {
        $uname = posix_uname();
        $hostname = $uname["nodename"];
        $words = explode(".", $hostname);
        $cluster = $words[count($words)-3];
        return QconfConfig::$hconfZkHost[$cluster];
    }

    private static function getIdc()
    {
        $uname = posix_uname();
        $hostname = $uname["nodename"];
        $words = explode(".", $hostname);
        $cluster = $words[count($words)-3];
        return $cluster;
    }

    public static function getHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHost($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHost($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        if ($idc === null)
        {
            $idc = self::getIdc();
        }
        if ($idc == null) return null;
        HConfig::setIdc($idc);

        $node = QconfConfig::getNodePath($name);
        return HConfig::GetChild($node, $flags);
    }

    public static function getConf($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        if ($idc === null)
        {
            $idc = self::getIdc();
        }
        if ($idc == null) return null;
        HConfig::setIdc($idc);

        $node = QconfConfig::getNodePath($name);
        return HConfig::Get($node, $flags);
    }

    public static function getMultConf($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name)
        {
            $name = trim($name);
            if (strlen($name) > 0)
            {
                $result[] = self::getConf($name, $idc, $flags);
            }
            else
            {
                $result[] = "";
            }
        }
        return $result;
    }

    public static function getHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $hosts = self::getAllHostNative($name, $idc, $flags);
        if ($hosts != null)
        {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHostNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        if ($idc === null)
        {
            $idc = self::getIdc();
        }
        if ($idc == null || $name == null) return null;
        HConfig::setIdc($idc);

        return HConfig::GetChild($name, $flags);
    }

    public static function getConfNative($name, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        if ($idc === null)
        {
            $idc = self::getIdc();
        }
        if ($idc == null || $name == null) return null;
        HConfig::setIdc($idc);

        return HConfig::Get($name, $flags);
    }

    public static function getMultConfNative($names, $idc = null, $flags = QconfConfig::HCONF_WAIT)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name)
        {
            $name = trim($name);
            if (strlen($name) > 0)
            {
                $result[] = self::getConfNative($name, $idc, $flags);
            }
            else
            {
                $result[] = "";
            }
        }
        return $result;
    }

}


class Qconf_old {
    private static function getZk() 
    {
        if (defined("HCONF_TEST_ENV")) {
            return QconfConfig::$hconfZkHost["test"];     
        }   
        else {
            $uname = posix_uname();
            $hostname = $uname["nodename"];
            $words = explode(".", $hostname);
            $cluster = $words[count($words)-3];
            return QconfConfig::$hconfZkHost[$cluster];
        } 
    }

    public static function getHost($name) 
    {
	    $hosts = self::getAllHost($name);
        if ($hosts != null) {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHost($name) 
    {
	    $zk = self::getZk();
    	if ($zk == null) return null;
        HConfig::setFastGetHost($zk);

		$node = QconfConfig::$dir . $name;
        return HConfig::GetChild($node);
    }

    public static function getConf($name)
    {
        $zk = self::getZk();
	    if ($zk == null) return null;
        HConfig::setFastGetHost($zk);

        $node = QconfConfig::$dir . $name;
        return HConfig::Get($node);
    }

    public static function getMultConf($names)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name) {
    	    $name = trim($name); 
            if (strlen($name) > 0) { 
                $result[] = self::getConf($name);
            }
            else {
                $result[] = "";
            }
        }
        return $result;
    }

    public static function getHostNative($name, $zk = null) 
    {
	    $hosts = self::getAllHostNative($name, $zk);
        if ($hosts != null) {
            return $hosts[array_rand($hosts)];
        }
        return null;
    }

    public static function getAllHostNative($name, $zk = null) 
    {
        if ($zk == null)
        {
            $zk = self::getZk();
        }
    	if ($zk == null) return null;
        HConfig::setFastGetHost($zk);

        return HConfig::GetChild($name);
    }

    public static function getConfNative($name, $zk = null)
    {
        if ($zk == null)
        {
            $zk = self::getZk();
        }
	    if ($zk == null) return null;
        HConfig::setFastGetHost($zk);

        return HConfig::Get($name);
    }

    public static function getMultConfNative($names, $zk = null)
    {
        $result = array();
        $name_arr = explode(',', $names);
        foreach ($name_arr as $name) {
    	    $name = trim($name); 
            if (strlen($name) > 0) { 
                $result[] = self::getConfNative($name, $zk);
            }
            else {
                $result[] = "";
            }
        }
        return $result;
    }

    public static function createConf($name, $val)
    {
        $real_path = NULL;
        $zk = self::getZk();
        if ($zk == null) return null;
        HConfig::setFastGetHost($zk);
        $node = QconfConfig::$dir . $name;
        if ( ! HConfig::Exists($node)) {
            $real_path = HConfig::Create($node, $val);
        }
        else {
            $real_path = $node;
        }
        return $real_path;
    }

    public static function setConf($name, $val)
    {
	    $zk = self::getZk();
    	if ($zk == null) return null;
        HConfig::setFastGetHost($zk);
        $node = QconfConfig::$dir . $name;
        return HConfig::Set($node, $val);
    }

}
?>
