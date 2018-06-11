#pragma once
/*
* Covariant Script Mathematics Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/cni.hpp>
#include <mozart/random.hpp>
#include <algorithm>

static cs::extension math_ext;
static cs::extension math_const_ext;
static cs::extension_t math_const_ext_shared = cs::make_shared_extension(math_const_ext);
namespace math_cs_ext {
	using namespace cs;

	number abs(number n)
	{
		return std::abs(n);
	}

	number ln(number n)
	{
		return std::log(n);
	}

	number log10(number n)
	{
		return std::log10(n);
	}

	number log(number a, number b)
	{
		return std::log(b / a);
	}

	number sin(number n)
	{
		return std::sin(n);
	}

	number cos(number n)
	{
		return std::cos(n);
	}

	number tan(number n)
	{
		return std::tan(n);
	}

	number asin(number n)
	{
		return std::asin(n);
	}

	number acos(number n)
	{
		return std::acos(n);
	}

	number atan(number n)
	{
		return std::atan(n);
	}

	number sqrt(number n)
	{
		return std::sqrt(n);
	}

	number root(number a, number b)
	{
		return std::pow(a, number(1) / b);
	}

	number pow(number a, number b)
	{
		return std::pow(a, b);
	}

	number _min(number a, number b)
	{
		return (std::min)(a, b);
	}

	number _max(number a, number b)
	{
		return (std::max)(a, b);
	}

	number rand(number b, number e)
	{
		return cov::rand<number>(b, e);
	}

	number randint(number b, number e)
	{
		return cov::rand<long>(b, e);
	}

	void init()
	{
		math_const_ext.add_var("max", var::make_constant<number>((std::numeric_limits<number>::max)()));
		math_const_ext.add_var("min", var::make_constant<number>((std::numeric_limits<number>::min)()));
		math_const_ext.add_var("inf", var::make_constant<number>(std::numeric_limits<number>::infinity()));
		math_const_ext.add_var("nan", var::make_constant<number>(std::numeric_limits<number>::quiet_NaN()));
		math_const_ext.add_var("pi", var::make_constant<number>(std::asin(number(1)) * 2));
		math_const_ext.add_var("e", var::make_constant<number>(std::exp(number(1))));
		math_ext.add_var("constant", var::make_protect<extension_t>(math_const_ext_shared));
		math_ext.add_var("abs", var::make_protect<callable>(cni(abs), true));
		math_ext.add_var("ln", var::make_protect<callable>(cni(ln), true));
		math_ext.add_var("log10", var::make_protect<callable>(cni(log10), true));
		math_ext.add_var("log", var::make_protect<callable>(cni(log), true));
		math_ext.add_var("sin", var::make_protect<callable>(cni(sin), true));
		math_ext.add_var("cos", var::make_protect<callable>(cni(cos), true));
		math_ext.add_var("tan", var::make_protect<callable>(cni(tan), true));
		math_ext.add_var("asin", var::make_protect<callable>(cni(asin), true));
		math_ext.add_var("acos", var::make_protect<callable>(cni(acos), true));
		math_ext.add_var("atan", var::make_protect<callable>(cni(atan), true));
		math_ext.add_var("sqrt", var::make_protect<callable>(cni(sqrt), true));
		math_ext.add_var("root", var::make_protect<callable>(cni(root), true));
		math_ext.add_var("pow", var::make_protect<callable>(cni(pow), true));
		math_ext.add_var("min", var::make_protect<callable>(cni(_min), true));
		math_ext.add_var("max", var::make_protect<callable>(cni(_max), true));
		math_ext.add_var("rand", var::make_protect<callable>(cni(rand)));
		math_ext.add_var("randint", var::make_protect<callable>(cni(randint)));
	}
}
