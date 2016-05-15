'use strict';

const co = require('co');
const exec = require('./exec');

module.exports = co.wrap(function * () {
  const _lasttag = yield exec('git rev-list --tags --max-count=1');
  const _version = yield exec('git describe --tags --abbrev=0 ' + _lasttag);
  const version = _version ? ' ' + _version + '..' : '';

  return ' ' + (yield exec('git log --no-merges --pretty="format: * %s (%an) %H%n"' + version));
});
