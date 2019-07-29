// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.hpp"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 1 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 1; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 1 * sizeof(PyObject *) );
            memcpy( python_pars + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 1 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 1 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 1; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 1 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 1 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 1 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 1 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 2 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 2; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 2 * sizeof(PyObject *) );
            memcpy( python_pars + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 2 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 2 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 2; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 2 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 2 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 2 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 2 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        Nuitka_FunctionObject *function = (Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 3 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 3; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 3 * sizeof(PyObject *) );
            memcpy( python_pars + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 3 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        Nuitka_MethodObject *method = (Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 3 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 3; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 3 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 3 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called );

        if ( flags & METH_NOARGS )
        {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 3 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 3 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.hpp"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
MOD_INIT_DECL( __main__ );
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    { (char *)"__main__", MOD_INIT_NAME( __main__ ), NULL, 0, NUITKA_COMPILED_MODULE },
    { (char *)"_bsddb", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_cn", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_hk", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_iso2022", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_jp", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_kr", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_tw", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_csv", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ctypes", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses_panel", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_elementtree", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hashlib", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hotshot", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_json", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lsprof", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multibytecodec", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multiprocessing", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_sqlite3", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ssl", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"bz2", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"dbm", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"mmap", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"parser", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pyexpat", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"readline", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"resource", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"termios", NULL, NULL, 0, NUITKA_SHLIB_FLAG },
    { (char *)"BaseHTTPServer", NULL, &constant_bin[ 2148 ], 21682, NUITKA_BYTECODE_FLAG },
    { (char *)"Bastion", NULL, &constant_bin[ 23830 ], 6622, NUITKA_BYTECODE_FLAG },
    { (char *)"CGIHTTPServer", NULL, &constant_bin[ 30452 ], 10984, NUITKA_BYTECODE_FLAG },
    { (char *)"Canvas", NULL, &constant_bin[ 41436 ], 15396, NUITKA_BYTECODE_FLAG },
    { (char *)"ConfigParser", NULL, &constant_bin[ 56832 ], 25087, NUITKA_BYTECODE_FLAG },
    { (char *)"Cookie", NULL, &constant_bin[ 81919 ], 22574, NUITKA_BYTECODE_FLAG },
    { (char *)"Dialog", NULL, &constant_bin[ 104493 ], 1902, NUITKA_BYTECODE_FLAG },
    { (char *)"DocXMLRPCServer", NULL, &constant_bin[ 106395 ], 9791, NUITKA_BYTECODE_FLAG },
    { (char *)"FileDialog", NULL, &constant_bin[ 116186 ], 9672, NUITKA_BYTECODE_FLAG },
    { (char *)"FixTk", NULL, &constant_bin[ 125858 ], 2078, NUITKA_BYTECODE_FLAG },
    { (char *)"HTMLParser", NULL, &constant_bin[ 127936 ], 13656, NUITKA_BYTECODE_FLAG },
    { (char *)"MimeWriter", NULL, &constant_bin[ 141592 ], 7338, NUITKA_BYTECODE_FLAG },
    { (char *)"Queue", NULL, &constant_bin[ 148930 ], 9360, NUITKA_BYTECODE_FLAG },
    { (char *)"ScrolledText", NULL, &constant_bin[ 158290 ], 2646, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleDialog", NULL, &constant_bin[ 160936 ], 4309, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleHTTPServer", NULL, &constant_bin[ 165245 ], 7980, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleXMLRPCServer", NULL, &constant_bin[ 173225 ], 22782, NUITKA_BYTECODE_FLAG },
    { (char *)"SocketServer", NULL, &constant_bin[ 196007 ], 23948, NUITKA_BYTECODE_FLAG },
    { (char *)"StringIO", NULL, &constant_bin[ 219955 ], 11434, NUITKA_BYTECODE_FLAG },
    { (char *)"Tix", NULL, &constant_bin[ 231389 ], 95379, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkconstants", NULL, &constant_bin[ 326768 ], 2236, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkdnd", NULL, &constant_bin[ 329004 ], 12765, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkinter", NULL, &constant_bin[ 341769 ], 199053, NUITKA_BYTECODE_FLAG },
    { (char *)"UserDict", NULL, &constant_bin[ 540822 ], 9613, NUITKA_BYTECODE_FLAG },
    { (char *)"UserList", NULL, &constant_bin[ 550435 ], 6501, NUITKA_BYTECODE_FLAG },
    { (char *)"UserString", NULL, &constant_bin[ 556936 ], 14720, NUITKA_BYTECODE_FLAG },
    { (char *)"_LWPCookieJar", NULL, &constant_bin[ 571656 ], 5513, NUITKA_BYTECODE_FLAG },
    { (char *)"_MozillaCookieJar", NULL, &constant_bin[ 577169 ], 4445, NUITKA_BYTECODE_FLAG },
    { (char *)"__future__", NULL, &constant_bin[ 581614 ], 4272, NUITKA_BYTECODE_FLAG },
    { (char *)"_abcoll", NULL, &constant_bin[ 585886 ], 25466, NUITKA_BYTECODE_FLAG },
    { (char *)"_osx_support", NULL, &constant_bin[ 611352 ], 11752, NUITKA_BYTECODE_FLAG },
    { (char *)"_pyio", NULL, &constant_bin[ 623104 ], 64318, NUITKA_BYTECODE_FLAG },
    { (char *)"_strptime", NULL, &constant_bin[ 687422 ], 15121, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata", NULL, &constant_bin[ 702543 ], 271, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata_nd", NULL, &constant_bin[ 702814 ], 20800, NUITKA_BYTECODE_FLAG },
    { (char *)"_threading_local", NULL, &constant_bin[ 723614 ], 6578, NUITKA_BYTECODE_FLAG },
    { (char *)"_weakrefset", NULL, &constant_bin[ 730192 ], 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"abc", NULL, &constant_bin[ 739766 ], 6113, NUITKA_BYTECODE_FLAG },
    { (char *)"aifc", NULL, &constant_bin[ 745879 ], 30327, NUITKA_BYTECODE_FLAG },
    { (char *)"anydbm", NULL, &constant_bin[ 776206 ], 2786, NUITKA_BYTECODE_FLAG },
    { (char *)"argparse", NULL, &constant_bin[ 778992 ], 64033, NUITKA_BYTECODE_FLAG },
    { (char *)"ast", NULL, &constant_bin[ 843025 ], 12898, NUITKA_BYTECODE_FLAG },
    { (char *)"asynchat", NULL, &constant_bin[ 855923 ], 8751, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncore", NULL, &constant_bin[ 864674 ], 18714, NUITKA_BYTECODE_FLAG },
    { (char *)"atexit", NULL, &constant_bin[ 883388 ], 2183, NUITKA_BYTECODE_FLAG },
    { (char *)"audiodev", NULL, &constant_bin[ 885571 ], 8407, NUITKA_BYTECODE_FLAG },
    { (char *)"bdb", NULL, &constant_bin[ 893978 ], 18971, NUITKA_BYTECODE_FLAG },
    { (char *)"binhex", NULL, &constant_bin[ 912949 ], 15349, NUITKA_BYTECODE_FLAG },
    { (char *)"bisect", NULL, &constant_bin[ 928298 ], 3053, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb", NULL, &constant_bin[ 931351 ], 12368, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"bsddb.db", NULL, &constant_bin[ 943719 ], 582, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbobj", NULL, &constant_bin[ 944301 ], 18601, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbrecio", NULL, &constant_bin[ 962902 ], 5269, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbshelve", NULL, &constant_bin[ 968171 ], 12910, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbtables", NULL, &constant_bin[ 981081 ], 24402, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbutils", NULL, &constant_bin[ 1005483 ], 1615, NUITKA_BYTECODE_FLAG },
    { (char *)"cProfile", NULL, &constant_bin[ 1007098 ], 6272, NUITKA_BYTECODE_FLAG },
    { (char *)"calendar", NULL, &constant_bin[ 1013370 ], 27624, NUITKA_BYTECODE_FLAG },
    { (char *)"cgi", NULL, &constant_bin[ 1040994 ], 32417, NUITKA_BYTECODE_FLAG },
    { (char *)"cgitb", NULL, &constant_bin[ 1073411 ], 12140, NUITKA_BYTECODE_FLAG },
    { (char *)"chunk", NULL, &constant_bin[ 1085551 ], 5571, NUITKA_BYTECODE_FLAG },
    { (char *)"cmd", NULL, &constant_bin[ 1091122 ], 13989, NUITKA_BYTECODE_FLAG },
    { (char *)"code", NULL, &constant_bin[ 1105111 ], 10294, NUITKA_BYTECODE_FLAG },
    { (char *)"codeop", NULL, &constant_bin[ 1115405 ], 6569, NUITKA_BYTECODE_FLAG },
    { (char *)"collections", NULL, &constant_bin[ 1121974 ], 26051, NUITKA_BYTECODE_FLAG },
    { (char *)"colorsys", NULL, &constant_bin[ 1148025 ], 3967, NUITKA_BYTECODE_FLAG },
    { (char *)"commands", NULL, &constant_bin[ 1151992 ], 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"compileall", NULL, &constant_bin[ 1154441 ], 6997, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler", NULL, &constant_bin[ 1161438 ], 1287, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"compiler.ast", NULL, &constant_bin[ 1162725 ], 71339, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.consts", NULL, &constant_bin[ 1234064 ], 727, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.future", NULL, &constant_bin[ 1234791 ], 3018, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.misc", NULL, &constant_bin[ 1237809 ], 3687, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pyassem", NULL, &constant_bin[ 1241496 ], 25831, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pycodegen", NULL, &constant_bin[ 1267327 ], 56161, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.symbols", NULL, &constant_bin[ 1323488 ], 17557, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.syntax", NULL, &constant_bin[ 1341045 ], 1862, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.transformer", NULL, &constant_bin[ 1342907 ], 47372, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.visitor", NULL, &constant_bin[ 1390279 ], 4159, NUITKA_BYTECODE_FLAG },
    { (char *)"contextlib", NULL, &constant_bin[ 1394438 ], 4422, NUITKA_BYTECODE_FLAG },
    { (char *)"cookielib", NULL, &constant_bin[ 1398860 ], 54567, NUITKA_BYTECODE_FLAG },
    { (char *)"copy", NULL, &constant_bin[ 1453427 ], 12140, NUITKA_BYTECODE_FLAG },
    { (char *)"csv", NULL, &constant_bin[ 1465567 ], 13454, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes", NULL, &constant_bin[ 1479021 ], 20192, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes._endian", NULL, &constant_bin[ 1499213 ], 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.util", NULL, &constant_bin[ 1501500 ], 8518, NUITKA_BYTECODE_FLAG },
    { (char *)"curses", NULL, &constant_bin[ 1510018 ], 1539, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"curses.ascii", NULL, &constant_bin[ 1511557 ], 5040, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.has_key", NULL, &constant_bin[ 1516597 ], 5925, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.panel", NULL, &constant_bin[ 1522522 ], 267, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.textpad", NULL, &constant_bin[ 1522789 ], 6842, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.wrapper", NULL, &constant_bin[ 1529631 ], 1206, NUITKA_BYTECODE_FLAG },
    { (char *)"dbhash", NULL, &constant_bin[ 1530837 ], 706, NUITKA_BYTECODE_FLAG },
    { (char *)"decimal", NULL, &constant_bin[ 1531543 ], 170849, NUITKA_BYTECODE_FLAG },
    { (char *)"difflib", NULL, &constant_bin[ 1702392 ], 61837, NUITKA_BYTECODE_FLAG },
    { (char *)"dircache", NULL, &constant_bin[ 1764229 ], 1560, NUITKA_BYTECODE_FLAG },
    { (char *)"dis", NULL, &constant_bin[ 1765789 ], 6204, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils", NULL, &constant_bin[ 1771993 ], 405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.archive_util", NULL, &constant_bin[ 1772398 ], 7432, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.bcppcompiler", NULL, &constant_bin[ 1779830 ], 7856, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.ccompiler", NULL, &constant_bin[ 1787686 ], 36748, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cmd", NULL, &constant_bin[ 1824434 ], 16722, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command", NULL, &constant_bin[ 1841156 ], 655, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.command.bdist", NULL, &constant_bin[ 1841811 ], 5199, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_dumb", NULL, &constant_bin[ 1847010 ], 5025, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_msi", NULL, &constant_bin[ 1852035 ], 23971, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_rpm", NULL, &constant_bin[ 1876006 ], 17724, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_wininst", NULL, &constant_bin[ 1893730 ], 10950, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build", NULL, &constant_bin[ 1904680 ], 5125, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_clib", NULL, &constant_bin[ 1909805 ], 6405, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_ext", NULL, &constant_bin[ 1916210 ], 19417, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_py", NULL, &constant_bin[ 1935627 ], 11483, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_scripts", NULL, &constant_bin[ 1947110 ], 4512, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.check", NULL, &constant_bin[ 1951622 ], 6236, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.clean", NULL, &constant_bin[ 1957858 ], 3158, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.config", NULL, &constant_bin[ 1961016 ], 12642, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install", NULL, &constant_bin[ 1973658 ], 17971, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_data", NULL, &constant_bin[ 1991629 ], 3142, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_egg_info", NULL, &constant_bin[ 1994771 ], 4421, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_headers", NULL, &constant_bin[ 1999192 ], 2274, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_lib", NULL, &constant_bin[ 2001466 ], 6752, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_scripts", NULL, &constant_bin[ 2008218 ], 2976, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.register", NULL, &constant_bin[ 2011194 ], 10188, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.sdist", NULL, &constant_bin[ 2021382 ], 16657, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.upload", NULL, &constant_bin[ 2038039 ], 6308, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.config", NULL, &constant_bin[ 2044347 ], 3548, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.core", NULL, &constant_bin[ 2047895 ], 7634, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cygwinccompiler", NULL, &constant_bin[ 2055529 ], 9793, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.debug", NULL, &constant_bin[ 2065322 ], 244, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dep_util", NULL, &constant_bin[ 2065566 ], 3164, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dir_util", NULL, &constant_bin[ 2068730 ], 6764, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dist", NULL, &constant_bin[ 2075494 ], 39429, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.emxccompiler", NULL, &constant_bin[ 2114923 ], 7441, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.errors", NULL, &constant_bin[ 2122364 ], 6237, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.extension", NULL, &constant_bin[ 2128601 ], 7396, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.fancy_getopt", NULL, &constant_bin[ 2135997 ], 11908, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.file_util", NULL, &constant_bin[ 2147905 ], 6732, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.filelist", NULL, &constant_bin[ 2154637 ], 10714, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.log", NULL, &constant_bin[ 2165351 ], 2754, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvc9compiler", NULL, &constant_bin[ 2168105 ], 21439, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvccompiler", NULL, &constant_bin[ 2189544 ], 17465, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.spawn", NULL, &constant_bin[ 2207009 ], 6377, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.sysconfig", NULL, &constant_bin[ 2213386 ], 15100, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.text_file", NULL, &constant_bin[ 2228486 ], 9229, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.unixccompiler", NULL, &constant_bin[ 2237715 ], 8028, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.util", NULL, &constant_bin[ 2245743 ], 14315, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.version", NULL, &constant_bin[ 2260058 ], 7170, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.versionpredicate", NULL, &constant_bin[ 2267228 ], 5520, NUITKA_BYTECODE_FLAG },
    { (char *)"doctest", NULL, &constant_bin[ 2272748 ], 83440, NUITKA_BYTECODE_FLAG },
    { (char *)"dumbdbm", NULL, &constant_bin[ 2356188 ], 6538, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_thread", NULL, &constant_bin[ 2362726 ], 5356, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_threading", NULL, &constant_bin[ 2368082 ], 1275, NUITKA_BYTECODE_FLAG },
    { (char *)"email", NULL, &constant_bin[ 2369357 ], 2852, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email._parseaddr", NULL, &constant_bin[ 2372209 ], 13763, NUITKA_BYTECODE_FLAG },
    { (char *)"email.base64mime", NULL, &constant_bin[ 2385972 ], 5305, NUITKA_BYTECODE_FLAG },
    { (char *)"email.charset", NULL, &constant_bin[ 2391277 ], 13499, NUITKA_BYTECODE_FLAG },
    { (char *)"email.encoders", NULL, &constant_bin[ 2404776 ], 2210, NUITKA_BYTECODE_FLAG },
    { (char *)"email.errors", NULL, &constant_bin[ 2406986 ], 3491, NUITKA_BYTECODE_FLAG },
    { (char *)"email.feedparser", NULL, &constant_bin[ 2410477 ], 11516, NUITKA_BYTECODE_FLAG },
    { (char *)"email.generator", NULL, &constant_bin[ 2421993 ], 10334, NUITKA_BYTECODE_FLAG },
    { (char *)"email.header", NULL, &constant_bin[ 2432327 ], 13622, NUITKA_BYTECODE_FLAG },
    { (char *)"email.iterators", NULL, &constant_bin[ 2445949 ], 2348, NUITKA_BYTECODE_FLAG },
    { (char *)"email.message", NULL, &constant_bin[ 2448297 ], 28576, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime", NULL, &constant_bin[ 2476873 ], 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email.mime.application", NULL, &constant_bin[ 2476993 ], 1570, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.audio", NULL, &constant_bin[ 2478563 ], 2893, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.base", NULL, &constant_bin[ 2481456 ], 1102, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.image", NULL, &constant_bin[ 2482558 ], 2035, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.message", NULL, &constant_bin[ 2484593 ], 1434, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.multipart", NULL, &constant_bin[ 2486027 ], 1655, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.nonmultipart", NULL, &constant_bin[ 2487682 ], 874, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.text", NULL, &constant_bin[ 2488556 ], 1294, NUITKA_BYTECODE_FLAG },
    { (char *)"email.parser", NULL, &constant_bin[ 2489850 ], 3804, NUITKA_BYTECODE_FLAG },
    { (char *)"email.quoprimime", NULL, &constant_bin[ 2493654 ], 8816, NUITKA_BYTECODE_FLAG },
    { (char *)"email.utils", NULL, &constant_bin[ 2502470 ], 9082, NUITKA_BYTECODE_FLAG },
    { (char *)"encodings.mbcs", NULL, &constant_bin[ 2511552 ], 2019, NUITKA_BYTECODE_FLAG },
    { (char *)"filecmp", NULL, &constant_bin[ 2513571 ], 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"fileinput", NULL, &constant_bin[ 2523145 ], 14436, NUITKA_BYTECODE_FLAG },
    { (char *)"fnmatch", NULL, &constant_bin[ 2537581 ], 3594, NUITKA_BYTECODE_FLAG },
    { (char *)"formatter", NULL, &constant_bin[ 2541175 ], 19016, NUITKA_BYTECODE_FLAG },
    { (char *)"fpformat", NULL, &constant_bin[ 2560191 ], 4648, NUITKA_BYTECODE_FLAG },
    { (char *)"fractions", NULL, &constant_bin[ 2564839 ], 19647, NUITKA_BYTECODE_FLAG },
    { (char *)"ftplib", NULL, &constant_bin[ 2584486 ], 34434, NUITKA_BYTECODE_FLAG },
    { (char *)"genericpath", NULL, &constant_bin[ 2618920 ], 3487, NUITKA_BYTECODE_FLAG },
    { (char *)"getopt", NULL, &constant_bin[ 2622407 ], 6626, NUITKA_BYTECODE_FLAG },
    { (char *)"getpass", NULL, &constant_bin[ 2629033 ], 4722, NUITKA_BYTECODE_FLAG },
    { (char *)"gettext", NULL, &constant_bin[ 2633755 ], 15628, NUITKA_BYTECODE_FLAG },
    { (char *)"glob", NULL, &constant_bin[ 2649383 ], 2919, NUITKA_BYTECODE_FLAG },
    { (char *)"gzip", NULL, &constant_bin[ 2652302 ], 15126, NUITKA_BYTECODE_FLAG },
    { (char *)"hashlib", NULL, &constant_bin[ 2667428 ], 7041, NUITKA_BYTECODE_FLAG },
    { (char *)"heapq", NULL, &constant_bin[ 2674469 ], 14520, NUITKA_BYTECODE_FLAG },
    { (char *)"hmac", NULL, &constant_bin[ 2688989 ], 4514, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot", NULL, &constant_bin[ 2693503 ], 3454, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"hotshot.log", NULL, &constant_bin[ 2696957 ], 5507, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stats", NULL, &constant_bin[ 2702464 ], 3374, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stones", NULL, &constant_bin[ 2705838 ], 1144, NUITKA_BYTECODE_FLAG },
    { (char *)"htmlentitydefs", NULL, &constant_bin[ 2706982 ], 6357, NUITKA_BYTECODE_FLAG },
    { (char *)"htmllib", NULL, &constant_bin[ 2713339 ], 20119, NUITKA_BYTECODE_FLAG },
    { (char *)"httplib", NULL, &constant_bin[ 2733458 ], 36745, NUITKA_BYTECODE_FLAG },
    { (char *)"ihooks", NULL, &constant_bin[ 2770203 ], 21226, NUITKA_BYTECODE_FLAG },
    { (char *)"imaplib", NULL, &constant_bin[ 2791429 ], 45155, NUITKA_BYTECODE_FLAG },
    { (char *)"imghdr", NULL, &constant_bin[ 2836584 ], 4798, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib", NULL, &constant_bin[ 2841382 ], 1488, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"imputil", NULL, &constant_bin[ 2842870 ], 15539, NUITKA_BYTECODE_FLAG },
    { (char *)"inspect", NULL, &constant_bin[ 2858409 ], 39936, NUITKA_BYTECODE_FLAG },
    { (char *)"io", NULL, &constant_bin[ 2898345 ], 3570, NUITKA_BYTECODE_FLAG },
    { (char *)"json", NULL, &constant_bin[ 2901915 ], 13937, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"json.decoder", NULL, &constant_bin[ 2915852 ], 11928, NUITKA_BYTECODE_FLAG },
    { (char *)"json.encoder", NULL, &constant_bin[ 2927780 ], 13668, NUITKA_BYTECODE_FLAG },
    { (char *)"json.scanner", NULL, &constant_bin[ 2941448 ], 2215, NUITKA_BYTECODE_FLAG },
    { (char *)"json.tool", NULL, &constant_bin[ 2943663 ], 1282, NUITKA_BYTECODE_FLAG },
    { (char *)"keyword", NULL, &constant_bin[ 2944945 ], 2093, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3", NULL, &constant_bin[ 2947038 ], 117, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.btm_matcher", NULL, &constant_bin[ 2947155 ], 5800, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.btm_utils", NULL, &constant_bin[ 2952955 ], 7529, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_base", NULL, &constant_bin[ 2960484 ], 7216, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_util", NULL, &constant_bin[ 2967700 ], 14607, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes", NULL, &constant_bin[ 2982307 ], 123, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.fixes.fix_apply", NULL, &constant_bin[ 2982430 ], 1869, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_asserts", NULL, &constant_bin[ 2984299 ], 1547, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_basestring", NULL, &constant_bin[ 2985846 ], 793, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_buffer", NULL, &constant_bin[ 2986639 ], 950, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_callable", NULL, &constant_bin[ 2987589 ], 1493, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_dict", NULL, &constant_bin[ 2989082 ], 3753, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_except", NULL, &constant_bin[ 2992835 ], 2993, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exec", NULL, &constant_bin[ 2995828 ], 1418, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_execfile", NULL, &constant_bin[ 2997246 ], 2059, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exitfunc", NULL, &constant_bin[ 2999305 ], 2739, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_filter", NULL, &constant_bin[ 3002044 ], 2256, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_funcattrs", NULL, &constant_bin[ 3004300 ], 1114, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_future", NULL, &constant_bin[ 3005414 ], 919, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_getcwdu", NULL, &constant_bin[ 3006333 ], 926, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_has_key", NULL, &constant_bin[ 3007259 ], 3184, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_idioms", NULL, &constant_bin[ 3010443 ], 4515, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_import", NULL, &constant_bin[ 3014958 ], 3233, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports", NULL, &constant_bin[ 3018191 ], 5352, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports2", NULL, &constant_bin[ 3023543 ], 622, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_input", NULL, &constant_bin[ 3024165 ], 1134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_intern", NULL, &constant_bin[ 3025299 ], 1605, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_isinstance", NULL, &constant_bin[ 3026904 ], 1838, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools", NULL, &constant_bin[ 3028742 ], 1791, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools_imports", NULL, &constant_bin[ 3030533 ], 2016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_long", NULL, &constant_bin[ 3032549 ], 841, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_map", NULL, &constant_bin[ 3033390 ], 3040, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_metaclass", NULL, &constant_bin[ 3036430 ], 6579, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_methodattrs", NULL, &constant_bin[ 3043009 ], 1138, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ne", NULL, &constant_bin[ 3044147 ], 985, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_next", NULL, &constant_bin[ 3045132 ], 3531, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_nonzero", NULL, &constant_bin[ 3048663 ], 1086, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_numliterals", NULL, &constant_bin[ 3049749 ], 1249, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_operator", NULL, &constant_bin[ 3050998 ], 5112, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_paren", NULL, &constant_bin[ 3056110 ], 1543, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_print", NULL, &constant_bin[ 3057653 ], 2727, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raise", NULL, &constant_bin[ 3060380 ], 2498, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raw_input", NULL, &constant_bin[ 3062878 ], 936, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reduce", NULL, &constant_bin[ 3063814 ], 1262, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_renames", NULL, &constant_bin[ 3065076 ], 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_repr", NULL, &constant_bin[ 3067525 ], 1016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_set_literal", NULL, &constant_bin[ 3068541 ], 1988, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_standarderror", NULL, &constant_bin[ 3070529 ], 853, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_sys_exc", NULL, &constant_bin[ 3071382 ], 1705, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_throw", NULL, &constant_bin[ 3073087 ], 1996, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_tuple_params", NULL, &constant_bin[ 3075083 ], 5430, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_types", NULL, &constant_bin[ 3080513 ], 2196, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_unicode", NULL, &constant_bin[ 3082709 ], 1716, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_urllib", NULL, &constant_bin[ 3084425 ], 7134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ws_comma", NULL, &constant_bin[ 3091559 ], 1382, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xrange", NULL, &constant_bin[ 3092941 ], 3063, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xreadlines", NULL, &constant_bin[ 3096004 ], 1152, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_zip", NULL, &constant_bin[ 3097156 ], 1346, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.main", NULL, &constant_bin[ 3098502 ], 9803, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.patcomp", NULL, &constant_bin[ 3108305 ], 6634, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2", NULL, &constant_bin[ 3114939 ], 164, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.pgen2.conv", NULL, &constant_bin[ 3115103 ], 8169, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.driver", NULL, &constant_bin[ 3123272 ], 5363, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.grammar", NULL, &constant_bin[ 3128635 ], 6004, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.literals", NULL, &constant_bin[ 3134639 ], 2002, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.parse", NULL, &constant_bin[ 3136641 ], 7204, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.pgen", NULL, &constant_bin[ 3143845 ], 12101, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.token", NULL, &constant_bin[ 3155946 ], 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.tokenize", NULL, &constant_bin[ 3158233 ], 16923, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pygram", NULL, &constant_bin[ 3175156 ], 1400, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pytree", NULL, &constant_bin[ 3176556 ], 30143, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.refactor", NULL, &constant_bin[ 3206699 ], 23866, NUITKA_BYTECODE_FLAG },
    { (char *)"linecache", NULL, &constant_bin[ 3230565 ], 3252, NUITKA_BYTECODE_FLAG },
    { (char *)"logging", NULL, &constant_bin[ 3233817 ], 57300, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"logging.config", NULL, &constant_bin[ 3291117 ], 25834, NUITKA_BYTECODE_FLAG },
    { (char *)"logging.handlers", NULL, &constant_bin[ 3316951 ], 39156, NUITKA_BYTECODE_FLAG },
    { (char *)"macpath", NULL, &constant_bin[ 3356107 ], 7635, NUITKA_BYTECODE_FLAG },
    { (char *)"macurl2path", NULL, &constant_bin[ 3363742 ], 2228, NUITKA_BYTECODE_FLAG },
    { (char *)"mailbox", NULL, &constant_bin[ 3365970 ], 76279, NUITKA_BYTECODE_FLAG },
    { (char *)"mailcap", NULL, &constant_bin[ 3442249 ], 7045, NUITKA_BYTECODE_FLAG },
    { (char *)"markupbase", NULL, &constant_bin[ 3449294 ], 9281, NUITKA_BYTECODE_FLAG },
    { (char *)"md5", NULL, &constant_bin[ 3458575 ], 368, NUITKA_BYTECODE_FLAG },
    { (char *)"mhlib", NULL, &constant_bin[ 3458943 ], 33653, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetools", NULL, &constant_bin[ 3492596 ], 8176, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetypes", NULL, &constant_bin[ 3500772 ], 18385, NUITKA_BYTECODE_FLAG },
    { (char *)"mimify", NULL, &constant_bin[ 3519157 ], 11963, NUITKA_BYTECODE_FLAG },
    { (char *)"modulefinder", NULL, &constant_bin[ 3531120 ], 19074, NUITKA_BYTECODE_FLAG },
    { (char *)"multifile", NULL, &constant_bin[ 3550194 ], 5382, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing", NULL, &constant_bin[ 3555576 ], 8405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.connection", NULL, &constant_bin[ 3563981 ], 14354, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.dummy", NULL, &constant_bin[ 3578335 ], 5412, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.dummy.connection", NULL, &constant_bin[ 3583747 ], 2732, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.forking", NULL, &constant_bin[ 3586479 ], 14336, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.heap", NULL, &constant_bin[ 3600815 ], 6861, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.managers", NULL, &constant_bin[ 3607676 ], 38278, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.pool", NULL, &constant_bin[ 3645954 ], 22367, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.process", NULL, &constant_bin[ 3668321 ], 9435, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.queues", NULL, &constant_bin[ 3677756 ], 11466, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.reduction", NULL, &constant_bin[ 3689222 ], 5972, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.sharedctypes", NULL, &constant_bin[ 3695194 ], 8572, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.synchronize", NULL, &constant_bin[ 3703766 ], 10988, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.util", NULL, &constant_bin[ 3714754 ], 10066, NUITKA_BYTECODE_FLAG },
    { (char *)"mutex", NULL, &constant_bin[ 3724820 ], 2494, NUITKA_BYTECODE_FLAG },
    { (char *)"netrc", NULL, &constant_bin[ 3727314 ], 4646, NUITKA_BYTECODE_FLAG },
    { (char *)"new", NULL, &constant_bin[ 3731960 ], 852, NUITKA_BYTECODE_FLAG },
    { (char *)"nntplib", NULL, &constant_bin[ 3732812 ], 20948, NUITKA_BYTECODE_FLAG },
    { (char *)"ntpath", NULL, &constant_bin[ 3753760 ], 13077, NUITKA_BYTECODE_FLAG },
    { (char *)"nturl2path", NULL, &constant_bin[ 3766837 ], 1801, NUITKA_BYTECODE_FLAG },
    { (char *)"numbers", NULL, &constant_bin[ 3768638 ], 13880, NUITKA_BYTECODE_FLAG },
    { (char *)"opcode", NULL, &constant_bin[ 3782518 ], 6134, NUITKA_BYTECODE_FLAG },
    { (char *)"optparse", NULL, &constant_bin[ 3788652 ], 53841, NUITKA_BYTECODE_FLAG },
    { (char *)"os", NULL, &constant_bin[ 3842493 ], 25585, NUITKA_BYTECODE_FLAG },
    { (char *)"os2emxpath", NULL, &constant_bin[ 3868078 ], 4499, NUITKA_BYTECODE_FLAG },
    { (char *)"pdb", NULL, &constant_bin[ 3872577 ], 43433, NUITKA_BYTECODE_FLAG },
    { (char *)"pickle", NULL, &constant_bin[ 3916010 ], 38342, NUITKA_BYTECODE_FLAG },
    { (char *)"pickletools", NULL, &constant_bin[ 3954352 ], 57072, NUITKA_BYTECODE_FLAG },
    { (char *)"pipes", NULL, &constant_bin[ 4011424 ], 9268, NUITKA_BYTECODE_FLAG },
    { (char *)"pkgutil", NULL, &constant_bin[ 4020692 ], 18859, NUITKA_BYTECODE_FLAG },
    { (char *)"platform", NULL, &constant_bin[ 4039551 ], 37615, NUITKA_BYTECODE_FLAG },
    { (char *)"plistlib", NULL, &constant_bin[ 4077166 ], 19143, NUITKA_BYTECODE_FLAG },
    { (char *)"popen2", NULL, &constant_bin[ 4096309 ], 8985, NUITKA_BYTECODE_FLAG },
    { (char *)"poplib", NULL, &constant_bin[ 4105294 ], 13271, NUITKA_BYTECODE_FLAG },
    { (char *)"posixfile", NULL, &constant_bin[ 4118565 ], 7620, NUITKA_BYTECODE_FLAG },
    { (char *)"posixpath", NULL, &constant_bin[ 4126185 ], 11366, NUITKA_BYTECODE_FLAG },
    { (char *)"pprint", NULL, &constant_bin[ 4137551 ], 10148, NUITKA_BYTECODE_FLAG },
    { (char *)"profile", NULL, &constant_bin[ 4147699 ], 16372, NUITKA_BYTECODE_FLAG },
    { (char *)"pstats", NULL, &constant_bin[ 4164071 ], 24885, NUITKA_BYTECODE_FLAG },
    { (char *)"pty", NULL, &constant_bin[ 4188956 ], 4938, NUITKA_BYTECODE_FLAG },
    { (char *)"py_compile", NULL, &constant_bin[ 4193894 ], 6603, NUITKA_BYTECODE_FLAG },
    { (char *)"pyclbr", NULL, &constant_bin[ 4200497 ], 9617, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc", NULL, &constant_bin[ 4210114 ], 92288, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc_data", NULL, &constant_bin[ 4302402 ], 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pydoc_data.topics", NULL, &constant_bin[ 4302522 ], 409111, NUITKA_BYTECODE_FLAG },
    { (char *)"random", NULL, &constant_bin[ 4711633 ], 25476, NUITKA_BYTECODE_FLAG },
    { (char *)"repr", NULL, &constant_bin[ 4737109 ], 5343, NUITKA_BYTECODE_FLAG },
    { (char *)"rexec", NULL, &constant_bin[ 4742452 ], 24068, NUITKA_BYTECODE_FLAG },
    { (char *)"rfc822", NULL, &constant_bin[ 4766520 ], 31721, NUITKA_BYTECODE_FLAG },
    { (char *)"rlcompleter", NULL, &constant_bin[ 4798241 ], 6054, NUITKA_BYTECODE_FLAG },
    { (char *)"robotparser", NULL, &constant_bin[ 4804295 ], 7904, NUITKA_BYTECODE_FLAG },
    { (char *)"runpy", NULL, &constant_bin[ 4812199 ], 8780, NUITKA_BYTECODE_FLAG },
    { (char *)"sched", NULL, &constant_bin[ 4820979 ], 4968, NUITKA_BYTECODE_FLAG },
    { (char *)"sets", NULL, &constant_bin[ 4825947 ], 16775, NUITKA_BYTECODE_FLAG },
    { (char *)"sgmllib", NULL, &constant_bin[ 4842722 ], 15334, NUITKA_BYTECODE_FLAG },
    { (char *)"sha", NULL, &constant_bin[ 4858056 ], 411, NUITKA_BYTECODE_FLAG },
    { (char *)"shelve", NULL, &constant_bin[ 4858467 ], 10194, NUITKA_BYTECODE_FLAG },
    { (char *)"shlex", NULL, &constant_bin[ 4868661 ], 7509, NUITKA_BYTECODE_FLAG },
    { (char *)"shutil", NULL, &constant_bin[ 4876170 ], 18568, NUITKA_BYTECODE_FLAG },
    { (char *)"site", NULL, &constant_bin[ 4894738 ], 17118, NUITKA_BYTECODE_FLAG },
    { (char *)"sitecustomize", NULL, &constant_bin[ 4911856 ], 224, NUITKA_BYTECODE_FLAG },
    { (char *)"smtpd", NULL, &constant_bin[ 4912080 ], 15826, NUITKA_BYTECODE_FLAG },
    { (char *)"smtplib", NULL, &constant_bin[ 4927906 ], 30169, NUITKA_BYTECODE_FLAG },
    { (char *)"sndhdr", NULL, &constant_bin[ 4958075 ], 7315, NUITKA_BYTECODE_FLAG },
    { (char *)"socket", NULL, &constant_bin[ 4965390 ], 16084, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3", NULL, &constant_bin[ 4981474 ], 154, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"sqlite3.dbapi2", NULL, &constant_bin[ 4981628 ], 2682, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3.dump", NULL, &constant_bin[ 4984310 ], 2057, NUITKA_BYTECODE_FLAG },
    { (char *)"sre", NULL, &constant_bin[ 4986367 ], 509, NUITKA_BYTECODE_FLAG },
    { (char *)"ssl", NULL, &constant_bin[ 4986876 ], 32205, NUITKA_BYTECODE_FLAG },
    { (char *)"stat", NULL, &constant_bin[ 5019081 ], 2723, NUITKA_BYTECODE_FLAG },
    { (char *)"statvfs", NULL, &constant_bin[ 5021804 ], 610, NUITKA_BYTECODE_FLAG },
    { (char *)"stringold", NULL, &constant_bin[ 5022414 ], 12487, NUITKA_BYTECODE_FLAG },
    { (char *)"subprocess", NULL, &constant_bin[ 5034901 ], 42004, NUITKA_BYTECODE_FLAG },
    { (char *)"sunau", NULL, &constant_bin[ 5076905 ], 18301, NUITKA_BYTECODE_FLAG },
    { (char *)"sunaudio", NULL, &constant_bin[ 5095206 ], 1969, NUITKA_BYTECODE_FLAG },
    { (char *)"symbol", NULL, &constant_bin[ 5097175 ], 3014, NUITKA_BYTECODE_FLAG },
    { (char *)"symtable", NULL, &constant_bin[ 5100189 ], 11678, NUITKA_BYTECODE_FLAG },
    { (char *)"sysconfig", NULL, &constant_bin[ 5111867 ], 18670, NUITKA_BYTECODE_FLAG },
    { (char *)"tabnanny", NULL, &constant_bin[ 5130537 ], 8195, NUITKA_BYTECODE_FLAG },
    { (char *)"tarfile", NULL, &constant_bin[ 5138732 ], 75638, NUITKA_BYTECODE_FLAG },
    { (char *)"telnetlib", NULL, &constant_bin[ 5214370 ], 23073, NUITKA_BYTECODE_FLAG },
    { (char *)"tempfile", NULL, &constant_bin[ 5237443 ], 20241, NUITKA_BYTECODE_FLAG },
    { (char *)"test", NULL, &constant_bin[ 5257684 ], 114, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"test.pystone", NULL, &constant_bin[ 5257798 ], 7981, NUITKA_BYTECODE_FLAG },
    { (char *)"textwrap", NULL, &constant_bin[ 5265779 ], 11996, NUITKA_BYTECODE_FLAG },
    { (char *)"this", NULL, &constant_bin[ 5277775 ], 1210, NUITKA_BYTECODE_FLAG },
    { (char *)"threading", NULL, &constant_bin[ 5278985 ], 42422, NUITKA_BYTECODE_FLAG },
    { (char *)"timeit", NULL, &constant_bin[ 5321407 ], 12149, NUITKA_BYTECODE_FLAG },
    { (char *)"tkColorChooser", NULL, &constant_bin[ 5333556 ], 1402, NUITKA_BYTECODE_FLAG },
    { (char *)"tkCommonDialog", NULL, &constant_bin[ 5334958 ], 1496, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFileDialog", NULL, &constant_bin[ 5336454 ], 5121, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFont", NULL, &constant_bin[ 5341575 ], 7076, NUITKA_BYTECODE_FLAG },
    { (char *)"tkMessageBox", NULL, &constant_bin[ 5348651 ], 3864, NUITKA_BYTECODE_FLAG },
    { (char *)"tkSimpleDialog", NULL, &constant_bin[ 5352515 ], 9053, NUITKA_BYTECODE_FLAG },
    { (char *)"toaiff", NULL, &constant_bin[ 5361568 ], 3090, NUITKA_BYTECODE_FLAG },
    { (char *)"token", NULL, &constant_bin[ 5364658 ], 3798, NUITKA_BYTECODE_FLAG },
    { (char *)"tokenize", NULL, &constant_bin[ 5368456 ], 14465, NUITKA_BYTECODE_FLAG },
    { (char *)"trace", NULL, &constant_bin[ 5382921 ], 22716, NUITKA_BYTECODE_FLAG },
    { (char *)"traceback", NULL, &constant_bin[ 5405637 ], 11631, NUITKA_BYTECODE_FLAG },
    { (char *)"ttk", NULL, &constant_bin[ 5417268 ], 62193, NUITKA_BYTECODE_FLAG },
    { (char *)"tty", NULL, &constant_bin[ 5479461 ], 1303, NUITKA_BYTECODE_FLAG },
    { (char *)"turtle", NULL, &constant_bin[ 5480764 ], 139008, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest", NULL, &constant_bin[ 5619772 ], 2954, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"unittest.case", NULL, &constant_bin[ 5622726 ], 40172, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.loader", NULL, &constant_bin[ 5662898 ], 11299, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.main", NULL, &constant_bin[ 5674197 ], 7985, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.result", NULL, &constant_bin[ 5682182 ], 7869, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.runner", NULL, &constant_bin[ 5690051 ], 7599, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.signals", NULL, &constant_bin[ 5697650 ], 2738, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.suite", NULL, &constant_bin[ 5700388 ], 10460, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.util", NULL, &constant_bin[ 5710848 ], 4494, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib", NULL, &constant_bin[ 5715342 ], 50856, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib2", NULL, &constant_bin[ 5766198 ], 47064, NUITKA_BYTECODE_FLAG },
    { (char *)"urlparse", NULL, &constant_bin[ 5813262 ], 14457, NUITKA_BYTECODE_FLAG },
    { (char *)"user", NULL, &constant_bin[ 5827719 ], 1714, NUITKA_BYTECODE_FLAG },
    { (char *)"uu", NULL, &constant_bin[ 5829433 ], 4294, NUITKA_BYTECODE_FLAG },
    { (char *)"uuid", NULL, &constant_bin[ 5833727 ], 22469, NUITKA_BYTECODE_FLAG },
    { (char *)"warnings", NULL, &constant_bin[ 5856196 ], 13570, NUITKA_BYTECODE_FLAG },
    { (char *)"wave", NULL, &constant_bin[ 5869766 ], 19911, NUITKA_BYTECODE_FLAG },
    { (char *)"weakref", NULL, &constant_bin[ 5889677 ], 15733, NUITKA_BYTECODE_FLAG },
    { (char *)"webbrowser", NULL, &constant_bin[ 5905410 ], 19681, NUITKA_BYTECODE_FLAG },
    { (char *)"whichdb", NULL, &constant_bin[ 5925091 ], 2229, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref", NULL, &constant_bin[ 5927320 ], 719, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wsgiref.handlers", NULL, &constant_bin[ 5928039 ], 16172, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.headers", NULL, &constant_bin[ 5944211 ], 7429, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.simple_server", NULL, &constant_bin[ 5951640 ], 6199, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.util", NULL, &constant_bin[ 5957839 ], 5961, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.validate", NULL, &constant_bin[ 5963800 ], 16786, NUITKA_BYTECODE_FLAG },
    { (char *)"xdrlib", NULL, &constant_bin[ 5980586 ], 9810, NUITKA_BYTECODE_FLAG },
    { (char *)"xml", NULL, &constant_bin[ 5990396 ], 1068, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom", NULL, &constant_bin[ 5991464 ], 6427, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom.NodeFilter", NULL, &constant_bin[ 5997891 ], 1112, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.domreg", NULL, &constant_bin[ 5999003 ], 3293, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.expatbuilder", NULL, &constant_bin[ 6002296 ], 33003, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minicompat", NULL, &constant_bin[ 6035299 ], 3394, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minidom", NULL, &constant_bin[ 6038693 ], 65227, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.pulldom", NULL, &constant_bin[ 6103920 ], 12986, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.xmlbuilder", NULL, &constant_bin[ 6116906 ], 16396, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree", NULL, &constant_bin[ 6133302 ], 119, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.etree.ElementInclude", NULL, &constant_bin[ 6133421 ], 1951, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementPath", NULL, &constant_bin[ 6135372 ], 7560, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementTree", NULL, &constant_bin[ 6142932 ], 34794, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.cElementTree", NULL, &constant_bin[ 6177726 ], 166, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.parsers", NULL, &constant_bin[ 6177892 ], 304, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.parsers.expat", NULL, &constant_bin[ 6178196 ], 277, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax", NULL, &constant_bin[ 6178473 ], 3679, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.sax._exceptions", NULL, &constant_bin[ 6182152 ], 6127, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.expatreader", NULL, &constant_bin[ 6188279 ], 14701, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.handler", NULL, &constant_bin[ 6202980 ], 12970, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.saxutils", NULL, &constant_bin[ 6215950 ], 14685, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.xmlreader", NULL, &constant_bin[ 6230635 ], 19078, NUITKA_BYTECODE_FLAG },
    { (char *)"xmllib", NULL, &constant_bin[ 6249713 ], 26732, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpclib", NULL, &constant_bin[ 6276445 ], 43832, NUITKA_BYTECODE_FLAG },
    { (char *)"zipfile", NULL, &constant_bin[ 6320277 ], 41594, NUITKA_BYTECODE_FLAG },
    { NULL, NULL, 0 }
};

void setupMetaPathBasedLoader( void )
{
    static bool init_done = false;

    if ( init_done == false )
    {
        registerMetaPathBasedUnfreezer( meta_path_loader_entries );
        init_done = true;
    }
}
