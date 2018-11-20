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
#include <covscript/mozart/random.hpp>
#include <covscript/cni.hpp>
#include <algorithm>

static cs::name_space math_ext;
static cs::name_space math_const_ext;
static cs::namespace_t math_const_ext_shared = cs::make_shared_namespace(math_const_ext);
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
		math_const_ext
		.add_var("max", var::make_constant<number>((std::numeric_limits<number>::max)()))
		.add_var("min", var::make_constant<number>((std::numeric_limits<number>::min)()))
		.add_var("inf", var::make_constant<number>(std::numeric_limits<number>::infinity()))
		.add_var("nan", var::make_constant<number>(std::numeric_limits<number>::quiet_NaN()))
		.add_var("pi", var::make_constant<number>(std::asin(number(1)) * 2))
		.add_var("e", var::make_constant<number>(std::exp(number(1))));
		math_ext
		.add_var("constants", make_namespace(math_const_ext_shared))
		.add_var("abs", make_cni(abs, true))
		.add_var("ln", make_cni(ln, true))
		.add_var("log10", make_cni(log10, true))
		.add_var("log", make_cni(log, true))
		.add_var("sin", make_cni(sin, true))
		.add_var("cos", make_cni(cos, true))
		.add_var("tan", make_cni(tan, true))
		.add_var("asin", make_cni(asin, true))
		.add_var("acos", make_cni(acos, true))
		.add_var("atan", make_cni(atan, true))
		.add_var("sqrt", make_cni(sqrt, true))
		.add_var("root", make_cni(root, true))
		.add_var("pow", make_cni(pow, true))
		.add_var("min", make_cni(_min, true))
		.add_var("max", make_cni(_max, true))
		.add_var("rand", make_cni(rand))
		.add_var("randint", make_cni(randint));
	}
}
