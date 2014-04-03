var binding = require("./profiler");

function Snapshot() {}

Snapshot.prototype.compare = function (other) {
  var my_objects = this.nodeCounts(),
      their_objects = other.nodeCounts(),
      diff = {}, i, k, my_val, their_val;
      all_keys = Object.keys(my_objects).concat(Object.keys(their_objects)); //has dupes, oh well
  for (i = 0; i < all_keys.length; i++) {
    k = all_keys[i];
    my_val = my_objects[k] || 0;
    their_val = their_objects[k] || 0;
    diff[k] = their_val - my_val;
  }
  return diff;
};

Snapshot.prototype.hotPath = function() {
  var path = [], node = this.root, c, i = 0;
  c = this.children(node);
  while (c.length > 0 && i < 1000) {
    node = c[0].to;
    c = this.children(node);
    path.push(node);
    i++;
  }
  return path;
};

Snapshot.prototype.children = function(node) {
  var i, children = [];
  for(i = 0; i < node.childrenCount; i++) {
    children[i] = node.getChild(i);
  }
  children.sort(function (a, b){
    return b.to.retainedSize() - a.to.retainedSize();
  });
  return children;
};

Snapshot.prototype.topDominatorIds = function() {
  var doms = {}, arr;
  this.allNodes().forEach(function(node){
    var dom = node.dominatorNode || { id: "none"};
    if (doms[dom.id]) {
      doms[dom.id] += 1;
    }
    else {
      doms[dom.id] = 1;
    }
  });
  arr = Object.keys(doms).map(function(d){
    return {id: d, count: doms[d]};
  });
  arr.sort(function(a, b) {
    return b.count - a.count;
  });
  return arr;
};

Snapshot.prototype.topDominators = function() {
  var self = this;
  return this.topDominatorIds().map(function(d){
    return self.getNodeById(+d.id);
  });
};

Snapshot.prototype.allNodes = function() {
  var n = this.nodesCount, i, nodes = [];
  for (i = 0; i < n; i++) {
    nodes[i] = this.getNode(i);
  }
  return nodes;
};

Snapshot.prototype.nodeCounts = function() {
  var objects = {};
  this.allNodes().forEach(function(n){
    if(n.type === "Object") {
      if (objects[n.name]) {
        objects[n.name] += 1;
      }
      else {
        objects[n.name] = 1;
      }
    }
    else {
      if (objects[n.type]) {
        objects[n.type] += 1;
      }
      else {
        objects[n.type] = 1;
      }
    }
  });
  return objects;
};

//adapted from WebCore/bindings/v8/ScriptHeapSnapshot.cpp
Snapshot.prototype.stringify = function() {
  var root = this.root, i, j, count_i, count_j, node,
      lowLevels = {}, entries = {}, entry,
      children = {}, child, edge, result = {};
  for (i = 0, count_i = root.childrenCount; i < count_i; i++) {
    node = root.getChild(i).to;
    if (node.type === 'Hidden') {
      lowLevels[node.name] = {
        count: node.instancesCount,
        size: node.size,
        type: node.name
      };
    }
    else if (node.instancesCount > 0) {
      entries[node.name] = {
        constructorName: node.name,
        count: node.instancesCount,
        size: node.size
      };
    }
    // FIXME: the children portion is too slow and bloats the results
    //*
    else {
      entry = {
        constructorName: node.name
      };
      for(j = 0, count_j = node.childrenCount; j < count_j; j++) {
        edge = node.getChild(j);
        child = edge.to;
        entry[child.ptr.toString()] = {
          constructorName: child.name,
          count: parseInt(edge.name, 10)
        };
      }
      children[node.ptr.toString()] = entry;
    }//*/
  }
  result.lowlevels = lowLevels;
  result.entries = entries;
  result.children = children;
  return JSON.stringify(result);
};

function CpuProfile() {}

function inspectorObjectFor(node) {
  var i, count, child,
      result = {
        functionName: node.functionName,
        url: node.scriptName,
        lineNumber: node.lineNumber,
        totalTime: node.totalTime,
        selfTime: node.selfTime,
        numberOfCalls: 0,
        visible: true,
        callUID: node.callUid,
        children: []
      };
  for(i = 0, count = node.childrenCount; i < count; i++) {
    child = node.getChild(i);
    result.children.push(inspectorObjectFor(child));
  }
  return result;
}

CpuProfile.prototype.getTopDownRoot = function() {
  return inspectorObjectFor(this.topRoot);
};

CpuProfile.prototype.getBottomUpRoot = function() {
  return inspectorObjectFor(this.bottomRoot);
};

var heapCache = [];

exports.takeSnapshot = function(name, control) {
  if (typeof name == 'function') {
    control = name;
    name = '';
  }

  if (!name || !name.length) {
    name = 'org.nodejs.profiles.heap.user-initiated.' + (heapCache.length + 1);
  }

  var snapshot = binding.heapProfiler.takeSnapshot(name, control);
  snapshot.__proto__ = Snapshot.prototype;
  heapCache.push(snapshot);

  return snapshot;
};

exports.getSnapshot = function(index) {
  return heapCache[index];
};

exports.findSnapshot = function(uid) {
  return heapCache.filter(function(s) {return s.uid === uid;})[0];
};

exports.snapshotCount = function() {
  return heapCache.length;
};

exports.deleteAllSnapshots = function () {
	heapCache = [];
	binding.heapProfiler.deleteAllSnapshots();
};

var cpuCache = [];

exports.startProfiling = function(name) {
  if (!name || !name.length) {
    name = 'org.nodejs.profiles.cpu.user-initiated.' + (cpuCache.length + 1);
  }

  binding.cpuProfiler.startProfiling(name);
};

exports.stopProfiling = function(name) {
  name = name ? name : '';
  var profile = binding.cpuProfiler.stopProfiling(name);
  profile.__proto__ = CpuProfile.prototype;
  cpuCache.push(profile);
  return profile;
};

exports.getProfile = function(index) {
  return cpuCache[index];
};

exports.findProfile = function(uid) {
  return cpuCache.filter(function(s) {return s.uid === uid;})[0];
};

exports.profileCount = function() {
  return cpuCache.length;
};

exports.deleteAllProfiles = function() {
  cpuCache = [];
  binding.cpuProfiler.deleteAllProfiles();
};

process.profiler = exports;
