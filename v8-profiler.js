var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var binding = require(binding_path);

function Snapshot() {}

Snapshot.prototype.getHeader = function() {
  return {
    typeId: this.typeId,
    uid: this.uid,
    title: this.title
  }
}

/**
 * @param {Snapshot} other
 * @returns {Object}
 */
Snapshot.prototype.compare = function(other) {
  var selfHist = nodesHist(this),
      otherHist = nodesHist(other),
      keys = Object.keys(selfHist).concat(Object.keys(otherHist)),
      diff = {};
  
  keys.forEach(function(key) {
    if (key in diff) return;

    var selfCount = selfCounts[key] || 0,
        otherCount = otherCounts[key] || 0;

    diff[key] = otherCount - selfCount;
  });

  return diff;
};

function nodes(snapshot) {
  var n = snapshot.nodesCount, i, nodes = [];
  for (i = 0; i < n; i++) {
    nodes[i] = snapshot.getNode(i);
  }
  return nodes;
};

function nodesHist(snapshot) {
  var objects = {};
  nodes(snapshot).forEach(function(node){
    var key = node.type === "Object" ? node.name : node.type;
    objects[key] = objects[node.name] || 0;
    objects[key]++;
  });
  return objects;
};

function CpuProfile() {}

CpuProfile.prototype.getHeader = function() {
  return {
    typeId: this.typeId,
    uid: this.uid,
    title: this.title
  }
}

var startTime, endTime;

var profiler = {
  /*HEAP PROFILER API*/

  get snapshots() { return binding.heap.snapshots; },
  
  takeSnapshot: function(name, control) {
    var snapshot = binding.heap.takeSnapshot.apply(null, arguments);
    snapshot.__proto__ = Snapshot.prototype;
    return snapshot;
  },
  
  getSnapshot: function(index) {
    var snapshot = binding.heap.snapshots[index];
    if (!snapshot) return;
    snapshot.__proto__ = Snapshot.prototype;
    return snapshot;
  },
  
  findSnapshot: function(uid) {
    var snapshot = binding.heap.snapshots.filter(function(snapshot) {
      return snapshot.uid == uid;
    })[0];
    if (!snapshot) return;
    snapshot.__proto__ = Snapshot.prototype;
    return snapshot;
  },
  
  deleteAllSnapshots: function () {
    binding.heap.snapshots.forEach(function(snapshot) {
      snapshot.delete();
    });
  },
  
  startTrackingHeapObjects: binding.heap.startTrackingHeapObjects,
  
  stopTrackingHeapObjects: binding.heap.stopTrackingHeapObjects,
  
  getHeapStats: binding.heap.getHeapStats,
  
  getObjectByHeapObjectId: binding.heap.getObjectByHeapObjectId,
  
  /*CPU PROFILER API*/
  
  get profiles() { return binding.cpu.profiles; },
  
  startProfiling: function(name, recsamples) {
    startTime = Date.now();
    binding.cpu.startProfiling(name, recsamples);
  },
  
  stopProfiling: function(name) {
    var profile = binding.cpu.stopProfiling(name);
    endTime = Date.now();
    profile.__proto__ = CpuProfile.prototype;
    if (!profile.startTime) profile.startTime = startTime;
    if (!profile.endTime) profile.endTime = endTime;
    return profile;
  },
  
  getProfile: function(index) {
    return binding.cpu.profiles[index];
  },
  
  findProfile: function(uid) {
    var profile = binding.cpu.profiles.filter(function(profile) {
      return profile.uid == uid;
    })[0];
    return profile;
  },
  
  deleteAllProfiles: function() {
    binding.cpu.profiles.forEach(function(profile) {
      profile.delete();
    });
  }
};

module.exports = profiler;
process.profiler = profiler;
