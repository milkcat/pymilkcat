from distutils.core import setup, Extension

milkcat_capi = Extension('_milkcat_capi',
	                       sources = ['milkcat_capi_wrap.c'],
	                       libraries = ['milkcat'])

setup (name = 'pymilkcat',
       version = '1.0',
       py_modules = ['pymilkcat', 'milkcat_capi'],
       description = 'Python interface for MilkCat - A Chinese NLP toolkit',
       ext_modules = [milkcat_capi])
