var _exec = require('child_process').execSync;

module.exports = function(expression) {
  return String(_exec(expression)).trim();
};
