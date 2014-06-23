const binding = require('../build/Release/profiler'),
      expect  = require('chai').expect;

const NODE_V_10 = /^v0\.10\.\d+$/.test(process.version),
      NODE_V_11 = /^v0\.11\.\d+$/.test(process.version);
    
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
  
  describe('Profiler', function() {
    
    it('has expected structure', function() {
      var properties = ['startProfiling', 'stopProfiling', 'profiles'];
      
      properties.forEach(function(prop) {
        expect(binding.cpu).to.have.property(prop);
      });
    });
    
    it('should start profiling without arguments', function() {
      expect(binding.cpu.startProfiling).to.not.throw();
    });
    
    it('should stop profiling', function() {
      expect(binding.cpu.stopProfiling).to.not.throw();
    });
    
    it('cashes profiles', function() {
      expect(binding.cpu.profiles).to.have.length(1);
    });
    
    it('should throw on start profiling with wrong arguments', function() {
      expect(function() {
        binding.cpu.startProfiling(1);
      }).to.throw();
      expect(function() {
        binding.cpu.startProfiling(true, 'name');
      }).to.throw();
    });
    
    it('should replace profile title, if started with name argument', function() {
      binding.cpu.startProfiling('P');
      var profile = binding.cpu.stopProfiling();
      expect(profile.title).to.equal('P');
    });
    
    it('should record samples, if started with recsamples argument', function() {
      if (NODE_V_10) return;
      
      binding.cpu.startProfiling(true);
      var profile = binding.cpu.stopProfiling();
      expect(profile.samples.length > 0).to.equal(true);
    });

  });

  describe('Profile', function() {

    it('has expected structure', function() {
      binding.cpu.startProfiling();
      var profile = binding.cpu.stopProfiling();
      var properties = NODE_V_11 ?
        ['delete', 'typeId', 'uid', 'title', 'head', 'startTime', 'endTime', 'samples'] :
        ['delete', 'typeId', 'uid', 'title', 'head', 'bottomRoot'];
        
      properties.forEach(function(prop) {
        expect(profile).to.have.property(prop);
      });
    });
    
    it('can delete itself from profiler cache', function() {
      binding.cpu.startProfiling();
      var profile = binding.cpu.stopProfiling();
      var oldProfilesLength = binding.cpu.profiles.length;
      profile.delete();
      expect(binding.cpu.profiles.length == oldProfilesLength - 1).to.equal(true);
    });

  });

  describe('Profile Node', function() {
    
    it('has expected structure', function() {
      binding.cpu.startProfiling('P');
      var profile = binding.cpu.stopProfiling();
      var mainProps = ['functionName', 'url', 'lineNumber', 'callUID', 'children'];
      var extendedProps = NODE_V_11 ? 
        ['bailoutReason', 'id', 'scriptId', 'hitCount'] :
        ['totalTime', 'selfTime', 'totalSamplesCount', 'selfSamplesCount'];
      var properties = mainProps.concat(extendedProps);
      
      properties.forEach(function(prop) {
        expect(profile.head).to.have.property(prop);
      });
    });
    
  });
  
  function deleteAllProfiles() {
    binding.cpu.profiles.slice().forEach(function(profile) {
      profile.delete();
    });
  }
});

describe('HEAP', function() {
  after(deleteAllSnapshots);
  
  describe('Profiler', function() {
  
    it('has expected structure', function() {
      var properties = [
        'takeSnapshot', 
        'startTrackingHeapObjects',
        'stopTrackingHeapObjects',
        'getHeapStats',
        'snapshots'
      ];
      
      properties.forEach(function(prop) {
        expect(binding.heap).to.have.property(prop);
      });
    });
    
    it('should take snapshot without arguments', function() {
      expect(binding.heap.takeSnapshot).to.not.throw();
    });
  
    it('cashes snapshots', function() {
      expect(binding.heap.snapshots).to.have.length(1);
    });
    
    it('should throw on take snapshot with wrong arguments', function() {
      expect(function() {
        binding.heap.takeSnapshot(1);
      }).to.throw();
      expect(function() {
        binding.heap.takeSnapshot('name', 1);
      }).to.throw();
    });
    
    it('should replace snapshot title, if started with name argument', function() {
      var snapshot = binding.heap.takeSnapshot('S');
      expect(snapshot.title).to.equal('S');
    });
    
    it('should use control function, if started with function argument', function(done) {
      binding.heap.takeSnapshot(function(progress, total) {
        if (progress === total) done();
      });
    });
    
    it('should write heap stats', function(done) {
      expect(binding.heap.startTrackingHeapObjects).to.not.throw();
      binding.heap.getHeapStats(
        function(samples) {
          expect(samples).to.instanceof(Array);
        },
        function() {
          expect(binding.heap.stopTrackingHeapObjects).to.not.throw();
          done();
        }
      );
    });
    
  });
  
  describe('Snapshot', function() {
    
    it('has expected structure', function() {
      var snapshot = binding.heap.takeSnapshot();
      var properties = [
        'delete', 'serialize', 'getNode', 'root', 
        'typeId', 'uid', 'title', 'nodesCount', 'maxSnapshotJSObjectId'
      ];
        
      properties.forEach(function(prop) {
        expect(snapshot).to.have.property(prop);
      });
    });
    
    it('can delete itself from profiler cache', function() {
      var snapshot = binding.heap.takeSnapshot();
      var oldSnapshotsLength = binding.heap.snapshots.length;
      snapshot.delete();
      expect(binding.heap.snapshots.length == oldSnapshotsLength - 1).to.equal(true);
    });
    
    it('can serialise itself', function(done) {
      var snapshot = binding.heap.takeSnapshot();
      var buffer = '';
      snapshot.serialize(
        function iterator(data, length) {
          buffer += data;
        },
        function callback() {
          expect(JSON.parse.bind(JSON, buffer)).to.not.throw();
          done();
        }
      );
    });
    
  });
  
  function deleteAllSnapshots() {
    binding.heap.snapshots.slice().forEach(function(snapshot) {
      snapshot.delete();
    });
  }
});