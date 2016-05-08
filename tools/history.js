var exec = require('./exec');

module.exports = function history() {
  var _lasttag = exec('git rev-list --tags --max-count=1');
  var _version = exec('git describe --tags --abbrev=0 ' + _lasttag);
  var version = _version ? ' ' + _version + '..' : '';
  return ' ' + exec('git log --no-merges --pretty="format: * %s (%an) %H%n"' + version);
}
