#include <covscript/dll.hpp>
#include <covscript/cni.hpp>

struct reflect_proxy {
    bool is_rvalue = false;
    short protect_level = 0;
    std::size_t refcount = 1;
    void *data = nullptr;
};

struct reflect_any {
    reflect_proxy *proxy = nullptr;
};

int remove_protect(cs::var &val) {
    reflect_proxy *proxy = reinterpret_cast<reflect_any *>(&val)->proxy;
    int level = proxy->protect_level;
    proxy->protect_level = 0;
    return level;
}

void set_protect(cs::var &val, int level) {
    reinterpret_cast<reflect_any *>(&val)->proxy->protect_level = level;
}

template<typename...ArgsT>
void modify_function(cs::var &func, ArgsT &&...args) {
    int level = remove_protect(func);
    func.swap(cs::var::make<cs::callable>(std::forward<ArgsT>(args)...), true);
    set_protect(func, level);
}

CNI_ROOT_NAMESPACE {
    CNI_V(privileged_do, [](cs::var &val, const cs::var &func) {
        int level = remove_protect(val);
        try {
            cs::invoke(func);
        } catch (...) {
            set_protect(val, level);
            throw;
        }
        set_protect(val, level);
    })

    CNI_V(modify_mem_fn, [](cs::object_method &om, const cs::var &target) {
        if (target.type() == typeid(cs::object_method))
            modify_function(om.callable,
                            target.val<cs::object_method>().callable.const_val<cs::callable>().get_raw_data(),
                            cs::callable::types::member_fn);
        else if (target.type() == typeid(cs::callable))
            modify_function(om.callable, target.const_val<cs::callable>().get_raw_data(),
                            cs::callable::types::member_fn);
        else
            throw cs::lang_error("Not a callable object!");
    })

    void proxy(cs::type_t &type, const cs::array &arr) {
        cs::map_t<cs::string, cs::var> map;
        for (auto &it: arr) {
            if (it.type() == typeid(cs::pair)) {
                const cs::pair &p = it.const_val<cs::pair>();
                if (p.first.type() == typeid(cs::string)) {
                    map.emplace(p.first.const_val<cs::string>(), p.second);
                    continue;
                }
            }
            throw cs::lang_error("Wrong format of proxy map!");
        }
        type.constructor = [type, map]() -> cs::var {
            cs::var cs_struct = type.constructor();
            for (auto &it: map) {
                cs::var &om = cs_struct.val<cs::structure>().get_var(it.first);
                if (it.second.type() == typeid(cs::object_method))
                    modify_function(om,
                                    it.second.val<cs::object_method>().callable.const_val<cs::callable>().get_raw_data(),
                                    cs::callable::types::member_fn);
                else if (it.second.type() == typeid(cs::callable))
                    modify_function(om, it.second.const_val<cs::callable>().get_raw_data(),
                                    cs::callable::types::member_fn);
                else
                    throw cs::lang_error("Not a callable object!");
            }
            return cs_struct;
        };
    }

    CNI(proxy)
}
