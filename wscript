srcdir  = '.'
blddir  = 'build'
VERSION = '0.1.0'

def set_options(ctx):
  ctx.tool_options('compiler_cxx')

def configure(ctx):
  ctx.check_tool('compiler_cxx')
  ctx.check_tool('node_addon')

def build(ctx):
  t = ctx.new_task_gen('cxx', 'shlib', 'node_addon')
  t.target = 'profiler'
  t.source = ctx.path.ant_glob('*.cc')
