'use strict';

const exec = require('./exec');

module.exports =
  (version) => exec('git push && git push origin "v' + version + '"');
