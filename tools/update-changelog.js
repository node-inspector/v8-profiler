var fs = require('fs');
var exists = fs.existsSync;
var read = fs.readFileSync;
var write = fs.writeFileSync;

var history = require('./history');

module.exports = function changelog(filename) {
  filename = filename || 'CHANGELOG.md';

  var _changelog = exists(filename) ? read(filename) : '';
  var _history = history();

  write(filename, _history + '\n' + _changelog);
};

//test -n "$EDITOR" && $EDITOR $CHANGELOG
