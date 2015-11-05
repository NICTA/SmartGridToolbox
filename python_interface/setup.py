from distutils.core import setup, Extension

mod = Extension(
    'sgt_python', 
    sources = ['sgt_python.cc'],
    extra_link_args=['-lboost_python3']
)

setup (name = 'PackageName',
       version = '1.0',
       description = 'SmartGridToolbox + python3',
       ext_modules = [mod])
