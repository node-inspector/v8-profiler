var npm_argv = JSON.parse(process.env.npm_config_argv);
if (npm_argv.original[0] !== 'publish') process.exit(0);

var rimraf = require('rimraf');
var extend = require('util')._extend;
var gyp = require('node-pre-gyp');

rimraf.sync('./build');

var versions = ['0.10.0', '0.12.0', '4.0.0', '5.0.0'];
var matrix = {
  x64: ['win32', 'linux', 'darwin'],
  ia32: ['win32']
};

var targets = [];
Object.keys(matrix).forEach(function(arch) {
  matrix[arch].forEach(function(platform) {
    versions.forEach(function(version) {
      targets.push({
        target: version,
        target_platform: platform,
        target_arch: arch
      });
    });
  });
}, []);

iterate();

function iterate(err) {
  if (err) {
    console.log(err.message);
    return process.exit(1);
  }

  var target = targets.pop();

  if (!target) process.exit(0);

  var prog = extend(new gyp.Run(), {opts: target});

  prog.commands.install([], iterate);
}
