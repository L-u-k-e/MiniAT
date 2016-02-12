import os
import shlex
import sys
import SCons.Builder

scripts = list()

AddOption('--verbose',
	dest='verbose',
	action='store_true',
	default=False,
	metavar='BVERBOSE',
	help='Be verbose and show "raw" build commands')

AddOption('--debugging',
	dest='debugging',
	action='store_true',
	default=False,
	metavar='BDEBUGGING',
	help='Compile with debugging symbols')

vars = Variables()
vars.Add(ListVariable('buildmod', 'List of modules to build', 'none',
                    ['python']))

env = Environment(variables = vars, ENV = {'PATH' : os.environ['PATH'], 'BVERBOSE' : GetOption('verbose'), 'BDEBUGGING' : GetOption('debugging')})

# http://scons.tigris.org/issues/show_bug.cgi?id=2831
# Help(vars.GenerateHelpText(env))

rootdir = os.getcwd()
tooldir = os.path.join(rootdir, "tools")
builddir = os.path.join(tooldir, "build")
outdir = os.path.join(rootdir, "out")
exedir = os.path.join(outdir, "exe")
libdir = os.path.join(outdir, "lib")
bindir = os.path.join(outdir, "bin")
docdir = os.path.join(outdir, "doc")

env.PrependENVPath('PATH', exedir)
env.Append(LIBPATH=libdir)

VERBOSE = GetOption('verbose')
DEBUGGING = GetOption('debugging')

def vprint(msg):
	if VERBOSE:
		print(msg)

if not VERBOSE:
	env['CCCOMSTR'] = "CC $TARGET"
	env['LINKCOMSTR'] = "LD $TARGET"
	env['SHCCCOMSTR'] = "CC $TARGET"
	env['SHLINKCOMSTR'] = "LD $TARGET"
	env['INSTALLSTR'] = "CP $TARGET"
	env['LEMONCOMSTR'] = "LEMON $TARGET"
	env['FLEXCOMSTR'] = "FLEX $TARGET"
	env['MASHCOMSTR'] = "MASH $TARGET"
	env['SWIGCOMSTR'] = "SWIG $TARGET"

if DEBUGGING:
	env['CCFLAGS'] = "-g"

vprint("Verbose: " + str(VERBOSE))
vprint("Debugging: " + str(DEBUGGING))

env.Clean(".", [outdir,
	os.path.join(builddir, "packageparser.pyc"),
	])

def opts_to_dict(args):

	opts = {}
	for x in shlex.split(args):
		if x.startswith('-'):
			k = x[0:2]
			v = x[2:]
			opts[k] = v
	return opts

#
# Define custom emitters and generators
#
def _lemon_emitter(target, source, env):

	for s in source:
		(dirName, fileName) = os.path.split(str(s))
		(fileBaseName, fileExtension) = os.path.splitext(fileName)
		outHeaderName = os.path.join(dirName, fileBaseName + '.h')
		outSrcName = os.path.join(dirName, fileBaseName + '.c')
		target.append(outHeaderName)
		target.append(outSrcName)

	for t in target:
		s = str(t)
		ext = os.path.splitext(s)[1]
		if len(ext) == 0:
			target.remove(t)

	# Figure out where the lempar template is coming from
	# either from an EXPLICIT option string, or the hard-coded lempar.c default
	# that is hard-coded in lemon.c. If no -T option is specified, we add it
	# implicitly since lemon is expecting it relative to CWD and scons
	# runs lemon from the root of the project.
	args = env['LEMON_OPTIONS'] if 'LEMON_OPTIONS' in env else '-Tlempar.c'
	options = opts_to_dict(args)

	# Normalize the option handling any scons specific file stuff (like #/<paths>)
	# with the File class, also handle the case of no -T option in LEMON_OPTIONS
	dep = options['-T'] if '-T' in options else 'lempar.c'
	dep = File(dep)
	options['-T'] = dep.path

	# Rebuild the options as a string and set the environment with it
	options = ' '.join(['%s%s' % (key, value) for (key, value) in options.items()])
	env['LEMON_OPTIONS'] = options

	# We inform scons that our generated targets also implicitly depend on the lempar.c file
	env.Depends(target, dep)
	return (target, source)

def _swig_emitter(target, source, env):
	type = env['SWIGTYPE']
	if type == None:
		raise Exception("Must specify SWIGTYPE")

	return (type + "_wrap.c", source);

def __swig_gen(source, target, env, for_signature):

	swiginc=""
	for i in env['SWIGINCLUDEPATH']:
		swiginc += "-I"+i+" "

	swigout = ""
	if 'SWIGOUTDIR' in env:
		swigout = "-outdir " + env['SWIGOUTDIR']
		if not os.path.isdir(env['SWIGOUTDIR']):
			os.makedirs(env['SWIGOUTDIR'])

	return 'swig -$SWIGTYPE -o $TARGET '+swiginc+' ' + swigout + ' $SOURCE'

#
# Define custom build wrapper functions
#
def buildFlex(NAME, SRC):
	return env.Flex(NAME, SRC)

def buildLemon(*args, **kwargs):
	l = env.Lemon(*args, **kwargs)
	d = env.Depends(l, os.path.join(exedir, "lemon"
						 + env['PROGSUFFIX']))
	return [l, d]

def buildMash(SRC):
	m = env.Mash(SRC)
	env.Depends(m, os.path.join(exedir, "mash" + env['PROGSUFFIX']))
	return env.Install(bindir, m)


def buildLibrary(*args, **kwargs):
	l = env.Library(*args, **kwargs)
	x = env.Install(libdir, l)
	return [l, x]

def buildModule(module):
	return module in env["buildmod"]

def getModuleLocation(module):
	if module != "python":
		raise TypeError("Invalid module: " + str(module))

	# Install langauage specific stuff in:
	# out/<language>/<module>/...
	# thus Python things defined in vm/SConscript will go to:
	#   out/python/vm/...
	base = Dir('.').path
	base = base.split(os.path.sep)[-1]
	return os.path.join(outdir, module, base)

def buildSharedLibrary(*args, **kwargs):

	module = kwargs["MODULE"] if "MODULE" in kwargs else None
	if(module != None):
		ignore = not buildModule(module)
		if ignore:
			vprint('buildSharedLibrary ignoring target: "' + args[0] + ' module: "' + module +'"')
			return
		installdir = getModuleLocation(kwargs["MODULE"])
	else:
		installdir = libdir

	l = env.SharedLibrary(*args, **kwargs)
	x = env.Install(installdir, l)
	return [l, x]

def installFile(*args, **kwargs):

	files = args
	module = kwargs["MODULE"] if "MODULE" in kwargs else None

	ignore = not buildModule(module)
	if ignore:
		vprint('installFile ignoring target: "' + str(files) + ' module: "' + str(module) +'"')
		return

	installdir = getModuleLocation(module)
	x = env.Install(installdir, files)
	return x

def buildProgram(*args, **kwargs):
	print args
	l = env.Program(*args, **kwargs)
	x = env.Install(exedir, l)
	return [l, x]

def addDoc(name, path):
	return env.InstallAs(os.path.join(docdir, name), path)

def addPrebuiltTool(name, path, layout='', prog='', comstr=None):

	install_path = os.path.join(exedir, name)
	program = prog + ' ' + '"' + install_path + '"' + ' ' + layout
	bld = SCons.Builder.Builder(
		action =	SCons.Action.Action(
			program,
			comstr
		)
	)
	env.Append(BUILDERS={name: bld})

	return env.InstallAs(install_path, path)

def useTool(name, target, src):
	return getattr(env, name)(target, src)

def buildSwig(*args, **kwargs):

	module = kwargs["MODULE"] if "MODULE" in kwargs else None
	if(module != None):
		ignore = not buildModule(module)
		if ignore:
			vprint('buildSwig ignoring target: "' + args[0] + ' module: "' + module +'"')
			return

	swigoutdir = kwargs['SWIGOUTDIR'] if 'SWIGOUTDIR' in kwargs else getModuleLocation(module)
	kwargs['SWIGOUTDIR'] = swigoutdir

	if 'SWIGINCLUDEPATH' in kwargs:
		o = kwargs['SWIGINCLUDEPATH']
		if isinstance(o, str):
			o = [ o ]
		newpaths = list()
		for p in Flatten(o):
			p = Dir(p).path
			newpaths.append(p)

		kwargs['SWIGINCLUDEPATH'] = newpaths

	s = env.Swig(*args, **kwargs)
	return s

#
# Set up custom builders in environment
#
bld = SCons.Builder.Builder(
	action=SCons.Action.Action(
		'flex -i -o $TARGET $SOURCE',
		'$FLEXCOMSTR'),
	src_suffix='.flex'
)
env.Append(BUILDERS={'Flex': bld})

bld = SCons.Builder.Builder(
	action=SCons.Action.Action(
		"lemon" + env['PROGSUFFIX'] + ' $LEMON_OPTIONS $SOURCE',
		'$LEMONCOMSTR'),
	emitter=_lemon_emitter
)
env.Append(BUILDERS={'Lemon': bld})

bld = SCons.Builder.Builder(
	action=SCons.Action.Action(
		"mash" +  env['PROGSUFFIX'] + ' $SOURCE',
		'$MASHCOMSTR'),
	src_suffix='.asm',
	suffix='.bin'
)
env.Append(BUILDERS={'Mash': bld})

bld = SCons.Builder.Builder(
	action=SCons.Action.CommandGeneratorAction(
		__swig_gen,
		{'cmdstr':'$SWIGCOMSTR'}),
	src_suffix='.i',
	emitter=_swig_emitter
)
env.Append(BUILDERS={'Swig': bld})

# Find and execute all the SConanscript files
for root, dirnames, filenames in os.walk(rootdir):
	for filename in filenames:

		# If it is a SConscript file, we append it to the list of
		# scripts found in  tuple format. The format is
		# ( SConscript path, module name). The module name is derived
		# derived from the directory structure between the rootdir and the
		# first found SConscript file. The "Module name" is used
		# as the variant directory.
		if outdir not in root and filename == "SConscript":
			t = os.path.join(root, filename)
			m = root.replace(rootdir, "")
			m = os.path.join(outdir, m.lstrip(os.sep))
			scripts.append((t, m))
			vprint("Including: " + t)

exports = [ 'env', 'buildProgram', 'buildSharedLibrary', 'buildLibrary', 'buildFlex', 'buildLemon', 'buildMash', 'addDoc',
		 'addPrebuiltTool', 'useTool', 'VERBOSE', 'libdir', 'installFile', 'buildSwig' ]

for (script, module) in scripts:
	q = os.path.join(rootdir, "out", module)
	vprint("Executing script: " + q)
	SConscript(script, variant_dir=q, duplicate=1, exports = exports)
