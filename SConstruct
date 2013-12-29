env = Environment()

prefix = Dir(ARGUMENTS.get('PREFIX', ''))

kb460ledd = env.program('kb460ledd', 'kb460ledd.c', LIBS=['X11'])
env.Install(prefix+'usr/bin', kb460ledd)

env.Alias('install', prefix + 'usr')
