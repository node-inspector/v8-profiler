'use strict';

const co = require('co');
const args = process.argv.slice(2);
const version = args.splice(0, 1);

const npm = require('./update-npm-version');
const changelog = require('./update-changelog');
const tag = require('./annotate-tag');
const commit = require('./commit-changes');
const push = require('./push-to-githab');

const prepublish = require('./prepublish-to-npm');
const publish = require('./publish-to-npm');

const EXAMPLE = ' Example:\n' +
  'node release.js 1.0.0 --build\n' +
  'node release.js 1.0.0 --publish'

const SEMVER = /^\d+(\.\d+(\.\d+(-.*)?)?(-.*)?)?(-.*)?$/;

console.assert(version, 'Wrong usage.' + EXAMPLE);
console.assert(SEMVER.test(version), version + ' is not correct semver');

const BUILD = args.some(
  (arg) => /^(-b|--build)$/.test(arg));

const PUBLISH = args.some(
  (arg) => /^(-p|--publish)$/.test(arg));

console.assert(BUILD || PUBLISH, 'No mode selected.' + EXAMPLE);

return co(function * () {
  if (BUILD) {
    console.log('--Update the version in package.json--');
    yield npm(version);

    // TODO: enable changelog on 1.0 version
    // console.log('--Update ChangeLog.md--');
    // changelog();

    console.log('--Commit the changes--');
    yield commit(version);

    console.log('--Tag the release--')
    yield tag(version);

    console.log('--Push to github--');
    yield push(version);
  } else if (PUBLISH) {
    console.log('--Download prebuilt binaries--');
    yield prepublish();

    console.log('--Publish to npm--');
    yield publish();
  }
}).catch((error) => {
  console.error(error.stack);
  return process.exit(1);
});
