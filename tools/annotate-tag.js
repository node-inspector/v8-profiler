'use strict';

const co = require('co');
const exec = require('./exec');

module.exports = co.wrap(function * (version) {
  const history = yield require('./history')(version);
  const tagname = 'v' + version;

  return yield exec('git tag -a "' + tagname + '" -m "' + history + '"');
});
