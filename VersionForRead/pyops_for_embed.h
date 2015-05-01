/* Copyright (C) 2015 Oneiric Tiger.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License Version 2
 * as published by the Free Software Foundation; either current version
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License Version 2 for more details.
 * 
 * You should have received a copy of the GNU General Public License Version 2
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/**
 * @file pyops_for_embed.h
 * @author Wim Li <liwangmj@gmail.com> (http://liwangmj.com)
 * @version 1.1.0
 * @date 2015-05-01
 */

#ifndef  _PYTHON_PYOPS_FOR_EMBED_H
#define  _PYTHON_PYOPS_FOR_EMBED_H

#include "pyops_base.h"

template<typename T>
const T& pycall_t::call(const string& mod_name_, const string& func_name_, pycall_arg_t& pyarg_, pytype_tool_impl_t<T>& pyret)
{
    PyObject *pName = NULL, *pModule = NULL;
    string err_msg;

    pName   = PyString_FromString(mod_name_.c_str());
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (NULL == pModule)
    {
        pyops_t::traceback(err_msg);
        throw runtime_error(err_msg.c_str());
        return pyret.get_value();
    }

    call_func(pModule, mod_name_, func_name_, pyarg_, pyret, err_msg);
    Py_DECREF(pModule);

    if (!err_msg.empty())
    {
        throw runtime_error(err_msg.c_str());
    }
    return pyret.get_value();
}
template<typename T>
const T& pycall_t::call_obj_method(PyObject *pObj, const string& func_name_, pycall_arg_t& pyarg_, pytype_tool_impl_t<T>& pyret)
{
    string err_msg;
    if (NULL == pObj)
    {
        pyops_t::traceback(err_msg);
        throw runtime_error(err_msg.c_str());
        return pyret.get_value();
    }

    static string mod_name_ = "NaN";
    call_func(pObj, mod_name_, func_name_, pyarg_, pyret, err_msg);

    if (!err_msg.empty())
    {
        throw runtime_error(err_msg.c_str());
    }
    return pyret.get_value();
}

template<typename T>
const T& pycall_t::call_lambda(PyObject *pFunc, pycall_arg_t& pyarg_, pytype_tool_impl_t<T>& pyret)
{
    string err_msg;
    if (NULL == pFunc)
    {
        err_msg = "can not call null PyObject";
        throw runtime_error(err_msg.c_str());
        return pyret.get_value();
    }

    call_func_obj(pFunc, pyarg_, pyret, err_msg);

    if (!err_msg.empty())
    {
        throw runtime_error(err_msg.c_str());
    }
    return pyret.get_value();
}


#endif
