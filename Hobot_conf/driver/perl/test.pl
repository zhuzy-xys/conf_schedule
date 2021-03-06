use HConf;
use strict;
# init
my $err = &HConf::hconf_init();
if ($err != 0){
	printf "init fail.\n";
	printf "$err\n";
}


#hconf_get_conf
	my $ret = "";
	my $errCode = &HConf::get_conf("/zookeeper",$ret,"","true");
	if ($errCode == 0){  
		printf "$ret\n";	
	}else{
		printf "fail,err: %d\n",$errCode;
	}

#hconf_get_batch_conf
	my %ret = ();
	my $errCode = &HConf::get_batch_conf("/zookeeper",\%ret,"","false");
	if ($errCode == 0){  		
		while (my ($k, $v) = each %ret){
    		print "$k : $v\n";
    	}
	}else{
		printf "fail,err: %d\n",$errCode;
	}

#hconf_get_host
	my $ret = "";
	my $errCode = &HConf::get_host("/zookeeper",$ret,"","false");
	if ($errCode == 0){  
		printf "$ret\n";	
	}else{
		printf "fail,err: %d\n",$errCode;
	}

#hconf_get_batch_keys
	my @ret = ();
	my $errCode = &HConf::get_batch_keys("/zookeeper",\@ret,"","false");
	if ($errCode == 0){  		
		foreach(@ret){
			printf "$_\n";
		}
	}else{
		printf "fail,err: %d\n",$errCode;
	}

#finalize
&HConf::hconf_destroy();

