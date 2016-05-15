'use strict';

const co = require('co');
const rimraf = require('rimraf');
const gyp = require('node-pre-gyp');
const versions = ['0.10.0', '0.12.0', '4.0.0', '5.0.0', '6.0.0'];
const matrix = {
  x64: ['win32', 'linux', 'darwin'],
  ia32: ['win32']
};

class Target {
  constructor(arch, platform, version) {
    this.target = version;
    this.target_platform = platform;
    this.target_arch = arch;
  }
}

const install = target => new Promise((resolve, reject) => {
  const prog = Object.assign(new gyp.Run(), {opts: target});

  prog.commands.install([], error => error ? reject(error) : resolve());
});

module.exports = co.wrap(function * () {
  rimraf.sync('./build');

  const targets = [];
  Object.keys(matrix).forEach(
    (arch) => matrix[arch].forEach(
    (platform) => versions.forEach(
    (version) => targets.push(new Target(arch, platform, version))
  )));

  while (targets.length) yield install(targets.pop());
});
