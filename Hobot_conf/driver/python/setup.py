from distutils.core import setup, Extension
setup(name = 'hconf_py', version = '1.2.2', ext_modules = [Extension('hconf_py', ['lib/python_hconf.cc'],
     include_dirs=['/usr/local/include/hconf'],
     extra_objects=['/usr/local/hconf/lib/libhconf.a']
     )])
