const expect  = require('chai').expect,
      profiler = require('../');

const NODE_V_010 = /^v0\.10\.\d+$/.test(process.version);
const NODE_V_8x = process.version.indexOf("v8.") === 0;

describe('v8-profiler', function() {
  describe('CPU', function() {
    after(deleteAllProfiles);

    describe('Profiler', function() {

      it('should start profiling', function() {
        expect(profiler.startProfiling).to.not.throw();
      });

      it('should stop profiling', function() {
        expect(profiler.stopProfiling).to.not.throw();
      });

      it('should cache profiles', function() {
        expect(Object.keys(profiler.profiles)).to.have.length(1);
      });

      it('should replace profile title, if started with name argument', function() {
        profiler.startProfiling('P');
        var profile = profiler.stopProfiling();
        expect(profile.title).to.equal('P');
      });

      it('should record samples, if started with recsamples argument', function() {
        if (NODE_V_010) return;

        profiler.startProfiling(true);
        var profile = profiler.stopProfiling();
        expect(profile.samples.length > 0).to.equal(true);
      });

      it('should throw on setSamplingInterval if profile recording in progress', function() {
        profiler.startProfiling();
        expect(profiler.setSamplingInterval).to.throw(
          'setSamplingInterval must be called when there are no profiles being recorded.');
        profiler.stopProfiling();
      });

      it('should set sampling interval', function() {
        profiler.setSamplingInterval(1000);
      });
    });

    describe('Profile', function() {
      it('should export itself with callback', function() {
        profiler.startProfiling('', true);
        var profile = profiler.stopProfiling();
        profile.export(function(error, result) {
          var _result = JSON.parse(result);

          expect(result).to.be.a('string');
          expect(_result).to.be.an('object');
        });
      });

      it('should export itself with stream', function(done) {
        profiler.startProfiling('', true);
        var profile = profiler.stopProfiling();
        var fs = require('fs'),
            ws = fs.createWriteStream('profile.json');
        profile.export().pipe(ws);

        ws.on('finish', function() {
          fs.unlink('profile.json', done);
        });
      });
    });

    function deleteAllProfiles() {
      Object.keys(profiler.profiles).forEach(function(key) {
        profiler.profiles[key].delete();
      });
    }
  });

  describe('HEAP', function() {
    after(deleteAllSnapshots);

    describe('Profiler', function() {

      it('should take snapshot without arguments', function() {
        expect(profiler.takeSnapshot).to.not.throw();
      });

      it('should increase heap limit without arguments (node8 only)', function() {
        if (NODE_V_8x) {
          expect(profiler.increaseHeapLimit).to.not.throw();
        }
      });

      it('should restore heap limit without arguments (node8 only)', function() {
        if (NODE_V_8x) {
          expect(profiler.restoreHeapLimit).to.not.throw();
        }
      });

      it('should cache snapshots', function() {
        expect(Object.keys(profiler.snapshots)).to.have.length(1);
      });

      it('should replace snapshot title, if started with name argument', function() {
        var snapshot = profiler.takeSnapshot('S');
        expect(snapshot.title).to.equal('S');
      });

      it('should use control function, if started with function argument', function(done) {
        if (!NODE_V_8x) {
          // Fix for Windows
          var checked = false;
          profiler.takeSnapshot(function(progress, total) {
            if (progress === total) {
              if (checked) return;
              checked = true;
              done();
            }
          });
        } else {
          profiler.takeSnapshot();
          done();
        }
      });

      it('should write heap stats', function(done) {
        expect(profiler.startTrackingHeapObjects).to.not.throw();
        var lastSeenObjectId = profiler.getHeapStats(
          function(samples) {
            expect(samples).to.instanceof(Array);
          },
          function() {
            expect(profiler.stopTrackingHeapObjects).to.not.throw();
            done();
          }
        );
        expect(typeof lastSeenObjectId).to.be.equal('number');
      });

      it('should return undefined for wrong params in getObjectByHeapObjectId', function() {
        expect(profiler.getObjectByHeapObjectId('a')).to.be.equal(undefined);
      });

      it('should return id for object in getHeapObjectId', function() {
        var obj = {};
        var snapshot = profiler.takeSnapshot();
        expect(profiler.getHeapObjectId(obj)).to.be.gt(0);
      });

      it('should return id for undefined param in getHeapObjectId', function() {
        var snapshot = profiler.takeSnapshot();
        expect(profiler.getHeapObjectId(undefined)).to.be.gt(0);
      });

      it('should return undefined for wrong params in getHeapObjectId', function() {
        var snapshot = profiler.takeSnapshot();
        expect(profiler.getHeapObjectId()).to.be.equal(undefined);
      });
    });

    describe('Snapshot', function() {

      it('should delete itself from profiler cache', function() {
        var snapshot = profiler.takeSnapshot();
        var uid = snapshot.uid;

        var oldSnapshotsLength = Object.keys(profiler.snapshots).length;
        snapshot.delete();

        expect(Object.keys(profiler.snapshots).length == oldSnapshotsLength - 1).to.equal(true);
        expect(profiler.snapshots[uid]).to.be.equal(undefined);
      });

      it('should serialise itself', function(done) {
        var snapshot = profiler.takeSnapshot();
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

      it('should pipe itself to stream', function(done) {
        var snapshot = profiler.takeSnapshot();
        var fs = require('fs'),
            ws = fs.createWriteStream('snapshot.json')
                   .on('finish', function() {
                     fs.unlink('snapshot.json', done);
                   });

        snapshot.export().pipe(ws);
      });

      it('should export itself to callback', function(done) {
        var snapshot = profiler.takeSnapshot();

        snapshot.export(function(err, result) {
          expect(!err);
          expect(typeof result == 'string');
          done();
        });
      });

      it('should compare itself with other snapshot', function() {
        this.timeout(5000);
        var snapshot1 = profiler.takeSnapshot();
        var snapshot2 = profiler.takeSnapshot();

        expect(snapshot1.compare.bind(snapshot1, snapshot2)).to.not.throw();
      });
    });

    function deleteAllSnapshots() {
      Object.keys(profiler.snapshots).forEach(function(key) {
        profiler.snapshots[key].delete();
      });
    }
  });
});
