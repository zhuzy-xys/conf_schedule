tmp_file="/tmp/hconf_test_script"

cat /dev/null > $tmp_file
echo "hconf_path = $hconf_path" >> $tmp_file 
echo "hconf_idc = $hconf_idc" >> $tmp_file 
time=`date +"%y-%m-%d %H:%M:%S"`
echo "hconf_time = $time" >> $tmp_file 
echo "hconf_type = $hconf_type" >> $tmp_file 
