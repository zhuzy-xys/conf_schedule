{
    "targets": [
        {
            "target_name": "hconf",
            "sources": [ "hconf.cc" ],
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "libraries": ["-lhconf"],
            "link_settings": {
            "libraries": ["-L<!(echo $HCONF_INSTALL)/lib"]
            },
            "include_dirs": [
                "<!(echo $HCONF_INSTALL)/include",
                "/usr/local/hconf/include",
                "/usr/local/include/hconf"
            ],
            "copies": [
                {
                    'destination': '/usr/local/hconf/lib',
                    'files': [
                        './build/Release/hconf.node'
                    ]
                }
            ]
        }
    ]
}
