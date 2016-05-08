var exec = require('./exec');

module.exports = function tag(version) {
  var history = require('./history')(version);
  var tagname = 'v' + version;
  exec('git tag -a "' + tagname + '" -m "' + history + '"');
};
