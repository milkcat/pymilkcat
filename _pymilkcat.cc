//
// The MIT License (MIT)
//
// Copyright 2013-2014 The MilkCat Project Developers
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// _pymilkcat.cc --- Created at 2014-02-19
//

#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include "milkcat.h"

// --------------------------------- DataSet -----------------------------------

typedef struct {
  PyObject_HEAD
  milkcat::Model *model_;
} ModelObject;

static int Model_init(ModelObject *self, PyObject *args, PyObject *kwds) {
  char *model_path = NULL;
  
  if (!PyArg_ParseTuple(args, "z", &model_path)) 
    return -1;
  self->model_ = milkcat::Model::New(model_path);
  return 0;
}

static PyObject *Model_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return (PyObject *)type->tp_alloc(type, 0);
}


static int Model_dealloc(ModelObject *self) {
  delete self->model_;
  self->model_ = NULL;

  PyObject_Del(self);
  return 0;
}

static PyObject *Model_set_userdict(ModelObject* self, PyObject *args) {
  char *userdict_path;

  if (!PyArg_ParseTuple(args, "s", &userdict_path))
    return NULL;

  bool success = self->model_->SetUserDictionary(userdict_path);
  return PyBool_FromLong(success);
}

static PyMethodDef ModelType_methods[] = {
  {
    "set_userdict", 
    (PyCFunction)Model_set_userdict, 
    METH_VARARGS,
    ""
  }, {
     NULL
  }  /* Sentinel */
};

static PyTypeObject ModelType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "milkcat.Model",               /* tp_name */
  sizeof(ModelObject),           /* tp_basicsize */
  0,                             /* tp_itemsize */
  (destructor)Model_dealloc,     /* tp_dealloc */
  0,                             /* tp_print */
  0,                             /* tp_getattr */
  0,                             /* tp_setattr */
  0,                             /* tp_reserved */
  0,                             /* tp_repr */
  0,                             /* tp_as_number */
  0,                             /* tp_as_sequence */
  0,                             /* tp_as_mapping */
  0,                             /* tp_hash  */
  0,                             /* tp_call */
  0,                             /* tp_str */
  0,                             /* tp_getattro */
  0,                             /* tp_setattro */
  0,                             /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,            /* tp_flags */
  "Model object for milkcat",    /* tp_doc */
  0,                             /* tp_traverse */
  0,                             /* tp_clear */
  0,                             /* tp_richcompare */
  0,                             /* tp_weaklistoffset */
  0,                             /* tp_iter */
  0,                             /* tp_iternext */
  ModelType_methods,             /* tp_methods */
  0,                             /* tp_members */
  0,                             /* tp_getset */
  0,                             /* tp_base */
  0,                             /* tp_dict */
  0,                             /* tp_descr_get */
  0,                             /* tp_descr_set */
  0,                             /* tp_dictoffset */
  (initproc)Model_init,          /* tp_init */
  0,                             /* tp_alloc */
  Model_new,                     /* tp_new */
};

// --------------------------------- MilkCat -----------------------------------

typedef struct {
  PyObject_HEAD
  ModelObject *model_object_;
  milkcat::Parser *parser_;
  milkcat::Parser::Iterator *it_;
} ParserObject;

static int Parser_init(ParserObject *self, PyObject *args, PyObject *kwds) {
  ModelObject *model_object;
  int segmenter_type = 0;
  int tagger_type = 0;

  if (!PyArg_ParseTuple(args, "O!ii", &ModelType, &model_object,
                        &segmenter_type, &tagger_type)) {
    return -1;
  }

  milkcat::Parser::Options options;
  switch (segmenter_type) {
    case 1:
      options.UseCrfSegmenter();
      break;
    case 2:
      options.UseUnigramSegmenter();
      break;
    case 3:
      options.UseBigramSegmenter();
      break;
    case 4:
      options.UseMixedSegmenter();
      break;
  }
  switch (tagger_type) {
    case 0:
      options.NoPOSTagger();
      break;
    case 1:
      options.UseHmmPOSTagger();
      break;
    case 2:
      options.UseCrfPOSTagger();
      break;
    case 3:
      options.UseMixedPOSTagger();
      break;
  }
  options.SetModel(model_object->model_);

  self->parser_ = milkcat::Parser::New(options);
  self->it_ = new milkcat::Parser::Iterator();
  self->model_object_ = model_object;
  Py_INCREF(self->model_object_);

  if (self->parser_ == NULL) {
    PyErr_SetString(PyExc_RuntimeError, milkcat::LastError());
    return -1;
  }

  return 0;
}

static PyObject *Parser_new(PyTypeObject *type,
                            PyObject *args,
                            PyObject *kwds) {
  return (PyObject *)type->tp_alloc(type, 0);
}


static int Parser_dealloc(ParserObject *self) {
  delete self->it_;
  self->it_ = NULL;

  delete self->parser_;
  self->parser_ = NULL;

  Py_DECREF(self->model_object_);
  PyObject_Del(self);
  return 0;
}

static PyObject *Parser_parse(ParserObject* self, PyObject *args) {
  char *text = NULL;

  if (!PyArg_ParseTuple(args, "s", &text)) {
    return NULL;
  }

  PyObject *result_list = PyList_New(0);
  self->parser_->Parse(text, self->it_);
  while (!self->it_->End()) {
    // Put the result item into tuple
    PyObject *tuple = PyTuple_New(3);
#if PY_MAJOR_VERSION == 3
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(self->it_->word()));
    PyTuple_SetItem(tuple, 1, PyUnicode_FromString(self->it_->part_of_speech_tag()));
#else
    PyTuple_SetItem(tuple, 0, PyString_FromString(self->it_->word()));
    PyTuple_SetItem(tuple, 1, PyString_FromString(self->it_->part_of_speech_tag()));
#endif
    PyTuple_SetItem(tuple, 2, PyLong_FromLong(self->it_->type()));

    PyList_Append(result_list, tuple);
    Py_DECREF(tuple);
    self->it_->Next();
  }

  return result_list;
}

static PyMethodDef ParserType_methods[] = {
  {
    "parse", 
    (PyCFunction)Parser_parse, 
    METH_VARARGS,
    ""
  }, {
    NULL
  }  /* Sentinel */
};

static PyTypeObject ParserType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "milkcat.Parser",               /* tp_name */
  sizeof(ParserObject),           /* tp_basicsize */
  0,                              /* tp_itemsize */
  (destructor)Parser_dealloc,     /* tp_dealloc */
  0,                              /* tp_print */
  0,                              /* tp_getattr */
  0,                              /* tp_setattr */
  0,                              /* tp_reserved */
  0,                              /* tp_repr */
  0,                              /* tp_as_number */
  0,                              /* tp_as_sequence */
  0,                              /* tp_as_mapping */
  0,                              /* tp_hash  */
  0,                              /* tp_call */
  0,                              /* tp_str */
  0,                              /* tp_getattro */
  0,                              /* tp_setattro */
  0,                              /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,             /* tp_flags */
  "Parser object",                /* tp_doc */
  0,                              /* tp_traverse */
  0,                              /* tp_clear */
  0,                              /* tp_richcompare */
  0,                              /* tp_weaklistoffset */
  0,                              /* tp_iter */
  0,                              /* tp_iternext */
  ParserType_methods,             /* tp_methods */
  0,                              /* tp_members */
  0,                              /* tp_getset */
  0,                              /* tp_base */
  0,                              /* tp_dict */
  0,                              /* tp_descr_get */
  0,                              /* tp_descr_set */
  0,                              /* tp_dictoffset */
  (initproc)Parser_init,          /* tp_init */
  0,                              /* tp_alloc */
  Parser_new,                     /* tp_new */
};

// -------------------------------- Python Module ------------------------------

#if PY_MAJOR_VERSION == 3

static PyModuleDef PyMilkCat_module = {
   PyModuleDef_HEAD_INIT,
   "_pymilkcat",  /* name of module */
   NULL,         /* module documentation, may be NULL */
   -1,           /* size of per-interpreter state of the module,
                    or -1 if the module keeps state in global variables. */
   NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit__pymilkcat(void) {
   PyObject* module;

   if (PyType_Ready(&ModelType) < 0)
      return NULL;

   if (PyType_Ready(&ParserType) < 0)
      return NULL;

   module = PyModule_Create(&PyMilkCat_module);
   if (module == NULL)
      return NULL;
   
   Py_INCREF(&ModelType);
   PyModule_AddObject(module, "Model", (PyObject *)&ModelType);
   Py_INCREF(&ParserType);
   PyModule_AddObject(module, "Parser", (PyObject *)&ParserType);

   return module;
}

#elif PY_MAJOR_VERSION == 2

static PyMethodDef _pymilkcat_methods[] = {
    {NULL}  /* Sentinel */
};

PyMODINIT_FUNC
init_pymilkcat(void) {
   PyObject* module;

   if (PyType_Ready(&ModelType) < 0)
      return;

   if (PyType_Ready(&ParserType) < 0)
      return;

   module = Py_InitModule("_pymilkcat", _pymilkcat_methods);
   if (module == NULL)
      return;
   
   Py_INCREF(&ModelType);
   PyModule_AddObject(module, "Model", (PyObject *)&ModelType);
   Py_INCREF(&ParserType);
   PyModule_AddObject(module, "Parser", (PyObject *)&ParserType);
}

#endif  // PY_MAJOR_VERSION == 3
