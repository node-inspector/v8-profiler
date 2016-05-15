'use strict';

const exec = require('./exec');

module.exports =
  (version) => exec('npm version --git-tag-version=false "' + version + '"');
