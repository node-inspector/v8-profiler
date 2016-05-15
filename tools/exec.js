'use strict';

const exec = require('child_process').exec;

module.exports =
  (expression) => new Promise(
  (resolve, reject) => exec(expression,
  (error, result) => error ? reject(error) : resolve(String(result).trim())
));
