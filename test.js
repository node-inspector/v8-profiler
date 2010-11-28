var assert = require('assert'),
    heap = require('./build/default/heap-profiler');
    
var x = heap.takeSnapshot('hello world');

assert.equal(heap.snapshotCount(), 1);
assert.equal(x.title, 'hello world');
var root = x.root;
assert.notEqual(root, null);

assert.ok(x.root.childrenCount > 0);

for (var i = 0; i < root.childrenCount; i++) {
  assert.notEqual(root.getChild(i), undefined);
}

var y = heap.takeSnapshot();

var delta = x.compareWith(y);
assert.notEqual(delta.additions, null);

console.log('done');