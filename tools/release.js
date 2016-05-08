var args = process.argv.slice(2);
var version = args.splice(0, 1);

var npm = require('./update-npm-version');
var changelog = require('./update-changelog');
var tag = require('./annotate-tag');
var commit = require('./commit-changes');
var push = require('./push-to-githab');

var prepublish = require('./prepublish-to-npm');
var publish = require('./publish-to-npm');

var EXAMPLE = ' Example:\n' +
  'node release.js 1.0.0 --build\n' +
  'node release.js 1.0.0 --publish'

var SEMVER = /^\d+(\.\d+(\.\d+(-.*)?)?(-.*)?)?(-.*)?$/;

console.assert(version, 'Wrong usage.' + EXAMPLE);
console.assert(SEMVER.test(version), version + ' is not correct semver');

var BUILD = args.some(function(arg) {
  return /^(-b|--build)$/.test(arg);
});

var PUBLISH = args.some(function(arg) {
  return /^(-p|--publish)$/.test(arg);
});

console.assert(BUILD || PUBLISH, 'No mode selected.' + EXAMPLE);

if (BUILD) {
  console.log('--Update the version in package.json--');
  npm(version);

  // TODO: enable changelog on 1.0 version
  // console.log('--Update ChangeLog.md--');
  // changelog();

  console.log('--Commit the changes--');
  commit(version);

  console.log('--Tag the release--')
  tag(version);

  console.log('--Push to github--');
  push(version);

} else if (PUBLISH) {
  console.log('--Download prebuilt binaries--');
  prepublish();

  console.log('--Publish to npm--');
  publish();
}
