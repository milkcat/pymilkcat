#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include "milkcat.h"

// --------------------------------- DataSet -----------------------------------

typedef struct {
  PyObject_HEAD
  milkcat_model_t *model_;
} ModelObject;

static int Model_init(ModelObject *self, PyObject *args, PyObject *kwds) {
  int argc = PyTuple_GET_SIZE(args);
  char *model_path = NULL;
  
  if (!PyArg_ParseTuple(args, "z", &model_path)) 
    return NULL;
  self->model_ = milkcat_model_new(model_path);
  return 0;
}

static PyObject *Model_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return (PyObject *)type->tp_alloc(type, 0);
}


static int Model_dealloc(ModelObject *self) {
  milkcat_model_destroy(self->model_);
  self->model_ = NULL;

  PyObject_Del(self);
  return 0;
}

static PyObject *Model_set_userdict(ModelObject* self, PyObject *args) {
  char *userdict_path;

  if (!PyArg_ParseTuple(args, "s", &userdict_path))
    return NULL;

  milkcat_model_set_userdict(self->model_, userdict_path);
  Py_RETURN_NONE;
}

static PyMethodDef ModelType_methods[] = {
  {
    "set_userdict", 
    (PyCFunction)Model_set_userdict, 
    METH_VARARGS,
    "Set the user dictionary for model."
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
  milkcat_t *analyzer_;
  milkcat_cursor_t *cursor_;
} MilkCatObject;

static int MilkCat_init(MilkCatObject *self, PyObject *args, PyObject *kwds) {
  ModelObject *model_object;
  int analyzer_type = 0;

  if (!PyArg_ParseTuple(args, "O!i", &ModelType, &model_object, &analyzer_type))
    return -1;

  self->analyzer_ = milkcat_new(model_object->model_, analyzer_type);
  self->model_object_ = model_object;
  self->cursor_ = milkcat_cursor_new();
  Py_INCREF(self->model_object_);

  return 0;
}

static PyObject *MilkCat_new(PyTypeObject *type,
                             PyObject *args,
                             PyObject *kwds) {
  return (PyObject *)type->tp_alloc(type, 0);
}


static int MilkCat_dealloc(MilkCatObject *self) {
  milkcat_cursor_destroy(self->cursor_);
  self->cursor_ = NULL;

  milkcat_destroy(self->analyzer_);
  self->analyzer_ = NULL;

  Py_DECREF(self->model_object_);
  PyObject_Del(self);
  return 0;
}

static PyObject *MilkCat_analyze(MilkCatObject* self, 
                                 PyObject *args, 
                                 PyObject *kwds) {
  PyObject *bool_word = NULL, 
           *bool_pos_tag = NULL, 
           *bool_word_type = NULL;
  bool show_word = true,
       show_pos_tag = true,
       show_word_type = true;
  static const char *kwlist[] = {"text", "word", "pos_tag", "word_type", NULL};
  char *text = NULL;

  if (!PyArg_ParseTupleAndKeywords(args, 
                                   kwds, 
                                   "s|O!O!O!", 
                                   (char **)kwlist, 
                                   &text,
                                   &PyBool_Type,
                                   &bool_word,
                                   &PyBool_Type,
                                   &bool_pos_tag,
                                   &PyBool_Type,
                                   &bool_word_type)) {
    return NULL;
  }
  
  // Get show options from arguments
  int item_cell_count = 0;
  if (bool_word) show_word = (bool_word == Py_True);
  if (bool_pos_tag) show_pos_tag = (bool_pos_tag == Py_True);
  if (bool_word_type) show_word_type = (bool_word_type == Py_True);

  if (show_word) item_cell_count++;
  if (show_pos_tag) item_cell_count++;
  if (show_word_type) item_cell_count++;

  // If None of the result would be showed returns None
  if (item_cell_count == 0) {
    Py_RETURN_NONE;
  }

  PyObject *result_list = PyList_New(0);

  milkcat_item_t item;
  PyObject *word_object = NULL,
           *pos_tag_object = NULL,
           *word_type_object = NULL;
  milkcat_analyze(self->analyzer_, self->cursor_, text);
  int cell_num = 0;
  while (milkcat_cursor_get_next(self->cursor_, &item)) {
    if (item_cell_count == 1) {
      // If cell count is 1, tuple is not necessary
      if (show_word) {
        word_object = PyUnicode_FromString(item.word);
        PyList_Append(result_list, word_object);
        Py_DECREF(word_object);
      }
      if (show_pos_tag) {
        pos_tag_object = PyUnicode_FromString(item.part_of_speech_tag);
        PyList_Append(result_list, pos_tag_object);
        Py_DECREF(pos_tag_object);
      }
      if (show_word_type) {
        word_type_object = PyLong_FromLong(item.word_type);
        PyList_Append(result_list, word_type_object);
        Py_DECREF(word_type_object);
      }
    } else {
      // Put the result item into tuple
      cell_num = 0;
      PyObject *tuple = PyTuple_New(item_cell_count);
      if (show_word) {
        word_object = PyUnicode_FromString(item.word);
        PyTuple_SetItem(tuple, cell_num, word_object);
        cell_num++;
      }
      if (show_pos_tag) {
        pos_tag_object = PyUnicode_FromString(item.part_of_speech_tag);
        PyTuple_SetItem(tuple, cell_num, pos_tag_object);
        cell_num++;
      }
      if (show_word_type) {
        word_type_object = PyLong_FromLong(item.word_type);
        PyTuple_SetItem(tuple, cell_num, word_type_object);
      }
      PyList_Append(result_list, tuple);
      Py_DECREF(tuple);
    }
  }

  return result_list;
}

static PyMethodDef MilkCatType_methods[] = {
  {
    "analyze", 
    (PyCFunction)MilkCat_analyze, 
    METH_VARARGS | METH_KEYWORDS,
    "Analyze a chinese text."
  }, {
    NULL
  }  /* Sentinel */
};

static PyTypeObject MilkCatType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "milkcat.MilkCat",              /* tp_name */
  sizeof(MilkCatObject),          /* tp_basicsize */
  0,                              /* tp_itemsize */
  (destructor)MilkCat_dealloc,    /* tp_dealloc */
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
  "Analyzer object",              /* tp_doc */
  0,                              /* tp_traverse */
  0,                              /* tp_clear */
  0,                              /* tp_richcompare */
  0,                              /* tp_weaklistoffset */
  0,                              /* tp_iter */
  0,                              /* tp_iternext */
  MilkCatType_methods,            /* tp_methods */
  0,                              /* tp_members */
  0,                              /* tp_getset */
  0,                              /* tp_base */
  0,                              /* tp_dict */
  0,                              /* tp_descr_get */
  0,                              /* tp_descr_set */
  0,                              /* tp_dictoffset */
  (initproc)MilkCat_init,         /* tp_init */
  0,                              /* tp_alloc */
  MilkCat_new,                    /* tp_new */
};

// ----------------------------------- Python Module -----------------------------------

static PyModuleDef PyMilkCat_module = {
   PyModuleDef_HEAD_INIT,
   "pymilkcat",  /* name of module */
   NULL,         /* module documentation, may be NULL */
   -1,           /* size of per-interpreter state of the module,
                    or -1 if the module keeps state in global variables. */
   NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_pymilkcat(void) {
   PyObject* module;

   if (PyType_Ready(&ModelType) < 0)
      return NULL;

   if (PyType_Ready(&MilkCatType) < 0)
      return NULL;

   module = PyModule_Create(&PyMilkCat_module);
   if (module == NULL)
      return NULL;
   
   Py_INCREF(&ModelType);
   PyModule_AddObject(module, "Model", (PyObject *)&ModelType);
   Py_INCREF(&MilkCatType);
   PyModule_AddObject(module, "MilkCat", (PyObject *)&MilkCatType);

   return module;
}