const expect  = require('chai').expect,
      binary = require('node-pre-gyp'),
      path = require('path'),
      binding_path = binary.find(path.resolve(path.join(__dirname,'../package.json'))),
      binding = require(binding_path);

const NODE_V_010 = /^v0\.10\.\d+$/.test(process.version);
const NODE_V_3 = /^v3\./.test(process.version);

describe('binding', function() {
  describe('Profiler container', function() {
    it('has expected structure', function() {
      var properties = ['cpu', 'heap'];

      properties.forEach(function(prop) {
        expect(binding).to.have.property(prop);
      });
    });
  });

  describe('CPU', function() {
    after(deleteAllProfiles);

    var cpu = binding.cpu;

    describe('Profiler', function() {

      it('has expected structure', function() {
        var properties = ['startProfiling', 'stopProfiling', 'profiles'];

        properties.forEach(function(prop) {
          expect(cpu).to.have.property(prop);
        });
      });
    });

    describe('Profile', function() {
      it('has expected structure', function() {
        cpu.startProfiling('', true);
        var profile = cpu.stopProfiling();
        var properties = NODE_V_010 ?
          ['delete', 'typeId', 'uid', 'title', 'head'] :
          ['delete', 'typeId', 'uid', 'title', 'head', 'startTime', 'endTime', 'samples', 'timestamps'];

        properties.forEach(function(prop) {
          expect(profile).to.have.property(prop);
        });
      });

      it('should delete itself from profiler cache', function() {
        cpu.startProfiling('', true);
        var profile = cpu.stopProfiling();
        var oldProfilesLength = Object.keys(cpu.profiles).length;
        profile.delete();
        expect(oldProfilesLength - Object.keys(cpu.profiles).length).to.equal(1);
      });
    });

    describe('Profile Node', function() {
      it('has expected structure', function() {
        cpu.startProfiling('P');
        var profile = cpu.stopProfiling();
        var mainProps = ['functionName', 'url', 'lineNumber', 'callUID', 'children',
          'bailoutReason', 'id', 'hitCount'];
        var extendedProps = NODE_V_010 ? [] : ['scriptId'];
        var properties = mainProps.concat(extendedProps);

        properties.forEach(function(prop) {
          expect(profile.head).to.have.property(prop);
        });
      });
    });

    function deleteAllProfiles() {
      cpu.profiles.slice().forEach(function(profile) {
        profile.delete();
      });
    }
  });

  describe('HEAP', function() {
    after(deleteAllSnapshots);

    var heap = binding.heap;

    describe('Profiler', function() {
      it('has expected structure', function() {
        var properties = [
          'increaseHeapLimit',
          'restoreHeapLimit',
          'takeSnapshot',
          'startTrackingHeapObjects',
          'stopTrackingHeapObjects',
          'getHeapStats',
          'snapshots'
        ];

        properties.forEach(function(prop) {
          expect(heap).to.have.property(prop);
        });
      });
    });

    describe('Snapshot', function() {
      it('has expected structure', function() {
        var snapshot = heap.takeSnapshot('', function() {});
        var properties = [
          'delete', 'serialize', 'getNode', 'root',
          'typeId', 'uid', 'title', 'nodesCount', 'maxSnapshotJSObjectId'
        ];

        properties.forEach(function(prop) {
          expect(snapshot).to.have.property(prop);
        });
      });
    });

    function deleteAllSnapshots() {
      Object.keys(binding.heap.snapshots).forEach(function(key) {
        binding.heap.snapshots[key].delete();
      });
    }
  });
});
