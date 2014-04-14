{
  'targets': [
    {
      'target_name': 'profiler',
      'sources': [
        'src/cpu_profiler.cc',
        'src/heap_profiler.cc',
        'src/profile.cc',
        'src/profile_node.cc',
        'src/profiler.cc',
        'src/snapshot.cc',
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ],
    }
  ]
}
