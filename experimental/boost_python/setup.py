from distutils.core import setup, Extension

mod = Extension(
    'sgt_python', 
    sources = ['sgt_python.cc'],
    extra_compile_args=['-std=c++11'],
    extra_link_args=['-lboost_python3', '-lSgtCore', '-lyaml-cpp',
                     '-lboost_date_time', '-larmadillo', '-lklu', '-lamd',
                     '-lcolamd', '-lbtf'],
)

setup (name = 'PackageName',
       version = '1.0',
       description = 'SmartGridToolbox + python3',
       ext_modules = [mod])
