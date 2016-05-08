var exec = require('./exec');

module.exports = function npm(version) {
  exec('npm version --git-tag-version=false "' + version + '"');
};
