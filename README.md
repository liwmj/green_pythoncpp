# PythonCpp

pythoncpp是C\++库，是嵌入python并注册C\++相关的对象到python的封装库。

## 特性

* 只有头文件
* 轻量级
* 高效清洁
* 容易调用python函数
* 容易设置或获取var在python脚本
* 容易使用C\++静态函数扩展python
* 容易使用C\++类扩展python，python C\++类一旦注册,可以使用它像装入的类型。
* 当python引发异常，pythoncpp也将返回给C\++。

## 安装

只需要将头文件放置python的include目录下即可使用，当前你也可以选择自己的目录。
PS：最好系统安装好Python，直接包含头文件即可。


## 支持
> 支持python2.5 python2.6 python2.7


## 使用

##### 获取/设置可变python脚本/模块
``` c++
printf("sys.version=%s\n", pythoncpp.get_global_var<string>("sys", "version").c_str());
pythoncpp.set_global_var("fftest", "global_var", "OhNice");
printf("fftest.global_var=%s\n", pythoncpp.get_global_var<string>("fftest", "global_var").c_str());
```

##### 调用的python函数，支持所有基本类型作为参数或返回值，支持最多九个参数。
``` c++
int a1 = 100; float a2 = 3.14f; string a3 = "OhWell";
pythoncpp.call<void>("fftest", "test_base", a1, a2, a3);
```

##### 调用python函数，支持所有STL类型作为参数或返回值，最多支持九个参数。
``` c++
vector<int> a1;a1.push_back(100);a1.push_back(200);
list<string> a2; a2.push_back("Oh");a2.push_back("Nice");
vector<list<string> > a3;a3.push_back(a2);

pythoncpp.call<bool>("fftest", "test_stl", a1, a2, a3);
typedef map<string, list<vector<int> > > ret_t;
ret_t val = pythoncpp.call<ret_t>("fftest", "test_return_stl");
```

##### 注册c++静态函数，所有基本类型的支持，最多支持九个参数。
``` c++
static int print_val(int a1, float a2, const string& a3, const vector<double>& a4)
{
    printf("%s[%d,%f,%s,%d]\n", __FUNCTION__, a1, a2, a3.c_str(), a4.size());
    return 0;
}

struct ops_t {
    static list<int> return_stl() {
        list<int> ret;ret.push_back(1024);
        printf("%s\n", __FUNCTION__);
        return ret;
    }
};

void test_reg_function()
{
    pythoncpp_t pythoncpp;//("ext1");
    pythoncpp.reg(&print_val, "print_val")
            .reg(&ops_t::return_stl, "return_stl");
    pythoncpp.init("ext1");
    pythoncpp.call<void>("fftest", "test_reg_function");
}
```

##### 注册C\++类，python可以使用轻松的使用它，就像在本地一样。
``` c++
class foo_t
{
public:
	foo_t(int v_):m_value(v_) {
		printf("%s\n", __FUNCTION__);
	}
    virtual ~foo_t() {
        printf("%s\n", __FUNCTION__);
    }
	int get_value() const { return m_value; }

	void set_value(int v_) { m_value = v_; }

	void test_stl(map<string, list<int> >& v_) {
		printf("%s\n", __FUNCTION__);
	}
	int m_value;
};

class dumy_t: public foo_t
{
public:
    dumy_t(int v_):foo_t(v_) {
        printf("%s\n", __FUNCTION__);
    }
    ~dumy_t() {
        printf("%s\n", __FUNCTION__);
    }
    void dump() {
        printf("%s\n", __FUNCTION__);
    }
};


static foo_t* obj_test(dumy_t* p)
{
    printf("%s\n", __FUNCTION__);
    return p;
}

void test_register_base_class(pythoncpp_t& pythoncpp)
{
	pythoncpp.reg_class<foo_t, PYCTOR(int)>("foo_t")
			.reg(&foo_t::get_value, "get_value")
			.reg(&foo_t::set_value, "set_value")
			.reg(&foo_t::test_stl, "test_stl")
            .reg_property(&foo_t::m_value, "m_value");

    pythoncpp.reg_class<dumy_t, PYCTOR(int)>("dumy_t", "dumy_t class inherit foo_t ctor <int>", "foo_t")
        .reg(&dumy_t::dump, "dump");

    pythoncpp.reg(obj_test, "obj_test");

    pythoncpp.init("ext2");
    pythoncpp.call<void>("fftest", "test_register_base_class");
};
```

##### 注册C\++类继承。
``` c++
pythoncpp.call<void>("fftest", "test_register_inherit_class");
```

##### C\++可以作为python对象指针，对象实例可以在载入python后进行访问。
``` c++
void test_cpp_obj_to_py(pythoncpp_t& pythoncpp)
{
    foo_t tmp_foo(2013);
    pythoncpp.call<void>("fftest", "test_cpp_obj_to_py", &tmp_foo);
}

void test_cpp_obj_py_obj(pythoncpp_t& pythoncpp)
{
    dumy_t tmp_foo(2013);

    foo_t* p = pythoncpp.call<foo_t*>("fftest", "test_cpp_obj_py_obj", &tmp_foo);
}
```

##### python测试脚本
``` python
def test_base(a1, a2, a3):
	print('test_base', a1, a2, a3)
	return 0

def test_stl(a1, a2, a3):
	print('test_stl', a1, a2, a3)
	return True

def test_return_stl():
	print('test_return_stl')
	#map<string, list<vector<int> > >
	ret = {'Oh':[[111,222], [333, 444] ] }
	return ret

def test_reg_function():
	import ext1
	ext1.print_val(123, 45.6 , "----789---", [3.14])
	ret = ext1.return_stl()
	print('test_reg_function', ret)

def test_register_base_class():
	import ext2
	foo = ext2.foo_t(20130426)
	print("test_register_base_class get_val:", foo.get_value())
	foo.set_value(778899)
	print("test_register_base_class get_val:", foo.get_value(), foo.m_value)
	foo.test_stl({"key": [11,22,33] })
	print('test_register_base_class test_register_base_class', foo)

def test_register_inherit_class():
	import ext2
	dumy = ext2.dumy_t(20130426)
	print("test_register_inherit_class get_val:", dumy.get_value())
	dumy.set_value(778899)
	print("test_register_inherit_class get_val:", dumy.get_value(), dumy.m_value)
	dumy.test_stl({"key": [11,22,33] })
	dumy.dump()
	print('test_register_inherit_class', dumy)

def test_cpp_obj_to_py(foo):
	import ext2
	print("test_cpp_obj_to_py get_val:", foo.get_value())
	foo.set_value(778899)
	print("test_cpp_obj_to_py get_val:", foo.get_value(), foo.m_value)
	foo.test_stl({"key": [11,22,33] })
	print('test_cpp_obj_to_py test_register_base_class', foo)

def test_cpp_obj_py_obj(dumy):
	import ext2
	print("test_cpp_obj_py_obj get_val:", dumy.get_value())
	dumy.set_value(778899)
	print("test_cpp_obj_py_obj get_val:", dumy.get_value(), dumy.m_value)
	dumy.test_stl({"key": [11,22,33] })
	dumy.dump()
	ext2.obj_test(dumy)
	print('test_cpp_obj_py_obj', dumy)

	return dumy

```


## 谢谢

欢迎使用，祝使用愉快

* [http://liwangmj.com](http://liwangmj.com)
* [liwangmj@gmail.com](mailto:liwangmj@gmail.com)

