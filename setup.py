from distutils.core import setup, Extension

_pymilkcat = Extension('_pymilkcat', 
	                  sources = ['_pymilkcat.cc'], 
	                  libraries = ['milkcat'])

setup (name = 'pymilkcat',
       version = '1.0',
       py_modules = ['pymilkcat'],
       description = 'Python interface for MilkCat - A Chinese morphological analyzer',
       ext_modules = [_pymilkcat])