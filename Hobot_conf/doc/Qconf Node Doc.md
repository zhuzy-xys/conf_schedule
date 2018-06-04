# node-hconf
Qihoo360 HConf(https://github.com/Qihoo360/HConf) Node.js addon.

[![NPM version][npm-image]][npm-url]
[![NPM downloads][downloads-image]][npm-url]
[![Node.js dependencies][david-image]][david-url]

## Installation

```
$ npm install node-hconf --save
```

Notes:

* You need set the Environment Variable HCONF_INSTALL before install this addon.

```
$ export HCONF_INSTALL=/home/work/local/hconf
$ npm install node-hconf --save
```

## Useage

```
var hconf = require('node-hconf');

console.log('version:', hconf.version())
console.log('getConf:', hconf.getConf('/demo'))
console.log('getBatchKeys:', hconf.getBatchKeys('/backend', 'test'))
console.log('getBatchConf:', hconf.getBatchConf('/backend/umem/users'))
console.log('getAllHost:', hconf.getAllHost('/backend/umem/users'))
console.log('getHost:', hconf.getHost('/backend/umem/users'))

```


[npm-image]: https://img.shields.io/npm/v/node-hconf.svg?style=flat-square
[npm-url]: https://npmjs.org/package/node-hconf
[downloads-image]: https://img.shields.io/npm/dm/node-hconf.svg?style=flat-square
[david-image]: https://img.shields.io/david/bluedapp/node-hconf.svg?style=flat-square
[david-url]: https://david-dm.org/bluedapp/node-hconf
