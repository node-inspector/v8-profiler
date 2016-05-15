'use strict';

const exec = require('./exec');

module.exports =
  (version) => exec('npm publish');
