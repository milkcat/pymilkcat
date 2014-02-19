from distutils.core import setup, Extension

pymilkcat = Extension('pymilkcat', sources = ['pymilkcat.cc'], libraries = ['milkcat'])

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [pymilkcat])