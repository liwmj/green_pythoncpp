
#ifdef _WIN32
#include <cstdlib>
#endif

#include "python/green_pythoncpp.h"

#define  TestGuard(X, Y) printf("-------%s begin-----------\n", X);try {Y;}catch(exception& e_){printf("exception<%s>\n", e_.what());}\
        printf("-------%s end-----------\n", X);

void test_base(green_pythoncpp_t& green_pythoncpp)
{
    printf("sys.version=%s\n", green_pythoncpp.get_global_var<string>("sys", "version").c_str());
    green_pythoncpp.set_global_var("test_green_pythoncpp", "global_var", "OhNice");
    printf("test_green_pythoncpp.global_var=%s\n", green_pythoncpp.get_global_var<string>("test_green_pythoncpp", "global_var").c_str());
    printf("time.asctime=%s\n", green_pythoncpp.call<string>("time", "asctime").c_str());
    int a1 = 100; float a2 = 3.14f; string a3 = "OhWell";
    green_pythoncpp.call<void>("test_green_pythoncpp", "test_base", a1, a2, a3);
}

void test_stl(green_pythoncpp_t& green_pythoncpp)
{
    vector<int> a1;a1.push_back(100);a1.push_back(200);
    list<string> a2; a2.push_back("Oh");a2.push_back("Nice");
    vector<list<string> > a3;a3.push_back(a2);
    
    green_pythoncpp.call<bool>("test_green_pythoncpp", "test_stl", a1, a2, a3);

}
void test_return_stl(green_pythoncpp_t& green_pythoncpp)
{
    typedef map<string, list<vector<int> > > ret_t;
    ret_t val = green_pythoncpp.call<ret_t>("test_green_pythoncpp", "test_return_stl");
}

static int print_val(int a1, int a2, const string& a3, const vector<double>& a4)
{
    //printf("%s[%d,%f,%s,%d]\n", __FUNCTION__, a1, a2, a3.c_str(), a4.size());
    return 0;
}
struct ops_t
{
    static list<int> return_stl()
    {
        list<int> ret;ret.push_back(1024);
        printf("%s\n", __FUNCTION__);
        return ret;
    }
};

void test_reg_function()
{
    green_pythoncpp_t green_pythoncpp;//("ext1");
    green_pythoncpp.reg(&print_val, "print_val")
            .reg(&ops_t::return_stl, "return_stl");
    green_pythoncpp.init("ext1");
    green_pythoncpp.call<void>("test_green_pythoncpp", "test_reg_function");
}

class foo_t
{
public:
	foo_t(int v_):m_value(v_)
	{
		printf("%s\n", __FUNCTION__);
	}
    virtual ~foo_t()
    {
        printf("%s\n", __FUNCTION__);
    }
	int get_value() const { return m_value; }
	void set_value(int v_) { m_value = v_; }
	void test_stl(map<string, list<int> >& v_) 
	{
		printf("%s\n", __FUNCTION__);
	}
	int m_value;
};

class dumy_t: public foo_t
{
public:
    dumy_t(int v_):foo_t(v_)
    {
        printf("%s\n", __FUNCTION__);
    }
    ~dumy_t()
    {
        printf("%s\n", __FUNCTION__);
    }
    void dump() 
    {
        printf("%s\n", __FUNCTION__);
    }
};


static foo_t* obj_test(dumy_t* p)
{
    printf("%s\n", __FUNCTION__);
    return p;
}

void test_register_base_class(green_pythoncpp_t& green_pythoncpp)
{
	green_pythoncpp.reg_class<foo_t, PYCTOR(int)>("foo_t")
			.reg(&foo_t::get_value, "get_value")
			.reg(&foo_t::set_value, "set_value")
			.reg(&foo_t::test_stl, "test_stl")
            .reg_property(&foo_t::m_value, "m_value");

    green_pythoncpp.reg_class<dumy_t, PYCTOR(int)>("dumy_t", "dumy_t class inherit foo_t ctor <int>", "foo_t")
        .reg(&dumy_t::dump, "dump");

    green_pythoncpp.reg(obj_test, "obj_test");

    green_pythoncpp.init("ext2");
    green_pythoncpp.call<void>("test_green_pythoncpp", "test_register_base_class");
};

void test_register_inherit_class(green_pythoncpp_t& green_pythoncpp)
{
    green_pythoncpp.call<void>("test_green_pythoncpp", "test_register_inherit_class");
};

void test_cpp_obj_to_py(green_pythoncpp_t& green_pythoncpp)
{
    foo_t tmp_foo(2013);
	vector<foo_t*> vt;
	vt.push_back(&tmp_foo);
    green_pythoncpp.call<void>("test_green_pythoncpp", "test_cpp_obj_to_py", &tmp_foo);
	printf("test_cpp_obj_to_py changed m_value=%d\n", tmp_foo.m_value);
	green_pythoncpp.call<void>("test_green_pythoncpp", "test_cpp_obj_to_py_ext", vt);
}

void test_cpp_obj_py_obj(green_pythoncpp_t& green_pythoncpp)
{
    dumy_t tmp_foo(2013);
    
    //foo_t* p = green_pythoncpp.call<foo_t*>("test_green_pythoncpp", "test_cpp_obj_py_obj", &tmp_foo);
    //p = NULL;
}

void test_py_class_lambda(green_pythoncpp_t& green_pythoncpp)
{
    PyObject* pobj = green_pythoncpp.call<PyObject*>("test_green_pythoncpp", "test_cpp_obj_return_py_obj");
    green_pythoncpp.obj_call<void>(pobj, "sayHi", 1, string("soNice"));
    
    PyObject* pFunc= green_pythoncpp.call<PyObject*>("test_green_pythoncpp", "test_cpp_obj_return_py_lambda");
    green_pythoncpp.call_lambda<void>(pFunc, 112233);
    
    Py_XDECREF(pFunc);
    Py_XDECREF(pobj);
}

int main(int argc, char* argv[])
{
    Py_Initialize();

	green_pythoncpp_t::add_path("./");
    green_pythoncpp_t green_pythoncpp;//("ext2");

    TestGuard("test_base", test_base(green_pythoncpp));
    TestGuard("test_stl", test_stl(green_pythoncpp));
    TestGuard("test_reg_function", test_reg_function());
    TestGuard("test_register_base_class", test_register_base_class(green_pythoncpp));

    TestGuard("test_register_inherit_class", test_register_inherit_class(green_pythoncpp));
    TestGuard("test_cpp_obj_to_py", test_cpp_obj_to_py(green_pythoncpp));
    TestGuard("test_cpp_obj_py_obj", test_cpp_obj_py_obj(green_pythoncpp));
    
    TestGuard("test_py_class_lambda", test_py_class_lambda(green_pythoncpp));
    
#ifdef _WIN32
	system("pause");
#endif
	Py_Finalize();
    printf("main exit...\n");
    return 0;
}
