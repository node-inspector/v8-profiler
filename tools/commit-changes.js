var exec = require('./exec');

module.exports = function commit(version) {
  var changed = [/*'ChangeLog.md',*/ 'package.json'].join(' ');
  exec('git commit -m "' + version + '" ' + changed);
};
