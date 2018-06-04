### The step to install php driver
- generate the hconf.so using the driver code sources, assume the path of php is '/usr/local/php'
change the directory to the driver source directory.
```
/path-to-php-install/bin/phpize
./configure --with-php-config=/path-to-php-install/bin/php-config \
            --with-libhconf-dir=/path-to-hconf-install/include \
            --enable-static LDFLAGS=/path-to-hconf-install/lib/libhconf.a
make
cp -v modules/hconf.so /path-to-php-extensions-dir/
```

- if php.ini doesn't include 'extension=hconf.so', then insert 'extension=hconf.so' into php.ini
- if current php has php-fpm service, then restart it `/path-to-php-install/sbin/php-fpm restart`
