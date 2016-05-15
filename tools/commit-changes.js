'use strict';

const exec = require('./exec');
const changed = [/*'ChangeLog.md',*/ 'package.json'].join(' ');

module.exports =
  (version) => exec('git commit -m "' + version + '" ' + changed);
