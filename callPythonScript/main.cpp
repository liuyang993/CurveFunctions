#define PY_SSIZE_T_CLEAN

#ifdef _DEBUG
  #undef _DEBUG
  #include <python.h>
  #define _DEBUG
#else
  #include <python.h>
#endif

//#include <Python.h>

/* Load a symbol from a module */
PyObject *import_name(const char *modname, const char *symbol) 
{ 
    PyObject *u_name, *module; 
    u_name = PyUnicode_FromString(modname); 

	
	
	module = PyImport_Import(u_name); 
	if (module == nullptr)
	{
		PyErr_Print();
		
	}

	Py_DECREF(u_name); 
      
    return PyObject_GetAttrString(module, symbol); 
} 

double call_func(PyObject *func, double x, double y) 
{ 
    PyObject *args; 
    PyObject *kwargs; 
    PyObject *result = 0; 
    double retval; 
      
    // Make sure we own the GIL 
    PyGILState_STATE state = PyGILState_Ensure(); 
      
      
    // Verify that func is a proper callable 
    if (!PyCallable_Check(func)) 
    { 
        fprintf(stderr, "call_func: expected a callable\n"); 
        goto fail; 
    } 
	    // Step3   这是有2个参数的写法
    args = Py_BuildValue("(dd)", x, y); 
    kwargs = NULL; 




      
    // Step 4 
    result = PyObject_Call(func, args, kwargs); 
    Py_DECREF(args); 
    Py_XDECREF(kwargs); 
      
    // Step 5 
    if (PyErr_Occurred()) 
    { 
        PyErr_Print(); 
        goto fail; 
    } 
      
    // Verify the result is a float object  
    if (!PyFloat_Check(result)) 
    { 
        fprintf(stderr, "call_func: callable didn't return a float\n"); 
        goto fail; 
    } 
      
    // Step 6 
    retval = PyFloat_AsDouble(result); 
    Py_DECREF(result); 
      
    // Step 7 
    PyGILState_Release(state); 
    return retval; 
    fail: 
        Py_XDECREF(result); 
        PyGILState_Release(state); 
        abort();  
} 




int main() 
{ 
    PyObject * pow_func; 
    double x; 

    Py_Initialize(); 
	//加这两句是为了增加路径到 sys path 为了能找到module
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
      
    // Get a reference to the math.pow function 
    //pow_func = import_name("math", "pow"); 

    pow_func = import_name("DtwForCPlusCalling", "callDTWBtwTwoArray"); 
      
    // Call it using our call_func() code  
    //for (x = 0.0; x < 10.0; x += 0.1) 
    //{ 
    //    printf("% 0.2f % 0.2f\n", x, call_func(pow_func, x, 2.0)); 
    //} 

	call_func(pow_func,1.0,1.0);
          
    Py_DECREF(pow_func); 
    Py_Finalize(); 

	getchar();

    return 0; 
} 
