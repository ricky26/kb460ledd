env = Environment()

prefix = Dir(ARGUMENTS.get('PREFIX', '/'))

env.Append(CXXFLAGS='-std=gnu++0x')

kb460ledd = env.Program('kb460ledd', 'kb460ledd.cpp', LIBS=['X11'])
env.Install(prefix.Dir('usr/bin'), kb460ledd)

env.Alias('install', prefix.Dir('usr'))
