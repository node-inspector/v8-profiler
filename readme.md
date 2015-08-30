[![Build Status](https://secure.travis-ci.org/node-inspector/v8-profiler.png?branch=master)](http://travis-ci.org/node-inspector/v8-profiler)
[![Build status](https://ci.appveyor.com/api/projects/status/hhgloy5smkl5i8fd/branch/master?svg=true)](https://ci.appveyor.com/project/3y3/v8-profiler/branch/master)
[![npm version](https://badge.fury.io/js/v8-profiler.svg)](http://badge.fury.io/js/v8-profiler)

v8-profiler provides [node](http://github.com/ry/node) bindings for the v8
profiler and integration with [node-inspector](http://github.com/dannycoates/node-inspector)

## Installation

    npm install v8-profiler

## Usage

    var profiler = require('v8-profiler');

## API

    var snapshot = profiler.takeSnapshot([name])      //takes a heap snapshot

    profiler.startProfiling([name])                   //begin cpu profiling
    // ... do your program's work ...
    var cpuProfile = profiler.stopProfiling([name])   //finish cpu profiling

## node-inspector

CPU profiles can be viewed and heap snapshots may be taken and viewed from the
profiles panel.  The profiles returned are JSONable objects in the `.cpuprofile`
format that node-inspector's `Profiles` panel loads.

    var serialized = JSON.stringify(cpuProfile, null, 2);
    require('fs').writeFileSync('myprogram.cpuprofile', serialized, 'utf8');

The `myprogram.cpuprofile` file written can then be loaded into the profiles panel and viewed as a flamechart.
