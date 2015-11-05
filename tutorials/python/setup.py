from distutils.core import setup, Extension

libSgtPythonModule = Extension(
    'libSgtPython', 
    sources = ['libSgtPython.cc'],
    extra_link_args=['-lboost_python3']
)

setup (name = 'PackageName',
       version = '1.0',
       description = 'SmartGridToolbox + python3',
       ext_modules = [libSgtPythonModule])
