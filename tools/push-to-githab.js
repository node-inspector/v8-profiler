var exec = require('./exec');

module.exports = function push(version) {
  var tag = 'v' + version;
  exec('git push && git push origin "' + tag + '"');
};
