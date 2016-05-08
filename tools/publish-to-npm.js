var exec = require('./exec');

module.exports = function publish(version) {
  exec('npm publish');
};
