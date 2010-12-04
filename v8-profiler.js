var binding = require("./build/default/profiler");

function Snapshot() {}

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
    /*
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
        }
      }
      children[node.ptr.toString()] = entry;
    }//*/
  }
  result.lowlevels = lowLevels;
  result.entries = entries;
  result.children = children;
  return JSON.stringify(result);
}

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

CpuProfile.prototype.stringify = function() {
  return JSON.stringify(inspectorObjectFor(this.topRoot));
}

var heapCache = [];

exports.takeSnapshot = function(name, mode) {
  var type = (mode === 'full') ? 0 : 1;
  var snapshot = binding.takeSnapshot(name, type);
  snapshot.__proto__ = Snapshot.prototype;
  heapCache.push(snapshot);
  return snapshot;
}

exports.getSnapshot = function(index) {
  return heapCache[index];
}

exports.findSnapshot = function(uid) {
  return heapCache.filter(function(s) {return s.uid === uid;})[0];
}

exports.snapshotCount = function() {
  return heapCache.length;
}

var cpuCache = [];

exports.startProfiling = function(name) {
  binding.startProfiling(name);
}

exports.stopProfiling = function(name) {
  var profile = binding.stopProfiling(name);
  profile.__proto__ = CpuProfile.prototype;
  cpuCache.push(profile);
  return profile;
}

exports.getProfile = function(index) {
  return cpuCache[index];
}

exports.findProfile = function(uid) {
  return cpuCache.filter(function(s) {return s.uid === uid;})[0];
}

exports.profileCount = function() {
  return cpuCache.length;
}

process.profiler = exports;