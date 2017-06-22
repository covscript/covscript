#pragma once
#include "../cni.hpp"
static cov_basic::extension thread_ext;
namespace thread_cbs_ext {
	using namespace cov_basic;
	cov::any join(array& args)
	{
		if(args.at(1).type()!=typeid(thread))
			throw syntax_error("Arguments error.");
		array _args{args.begin()+1,args.end()};
		args.at(1).val<thread>(true).join(_args);
		return number(0);
	}
	void idle(thread& th)
	{
		th.idle();
	}
	void kill(thread& th)
	{
		th.kill();
	}
	void init()
	{
		thread_ext.add_var("join",cov::any::make_protect<native_interface>(join));
		thread_ext.add_var("idle",cov::any::make_protect<native_interface>(cni(idle)));
		thread_ext.add_var("kill",cov::any::make_protect<native_interface>(cni(kill)));
	}
}
