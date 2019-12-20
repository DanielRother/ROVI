Import("env")

print(env)
print(type(env))
print(type(env['CXXFLAGS']))
print("Before")
print(str(env['CXXFLAGS']))
env['CXXFLAGS'].remove("-fno-rtti")
env['CXXFLAGS'].append("-frtti")
print("After")
print(str(env['CXXFLAGS']))

env.ProcessUnFlags("-fno-rtti")