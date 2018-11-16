#pragma once
/*
* Covariant Script Programming Language
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
* Website: http://covscript.org
*/
#include <covscript/instance.hpp>

namespace cs {
	std::string get_sdk_path();

	std::string process_path(const std::string &raw)
	{
		auto pos0 = raw.find('\"');
		auto pos1 = raw.rfind('\"');
		if (pos0 != std::string::npos) {
			if (pos0 == pos1)
				throw cs::fatal_error("argument syntax error.");
			else
				return raw.substr(pos0 + 1, pos1 - pos0 - 1);
		}
		else
			return raw;
	}

	std::string get_import_path()
	{
		const char *import_path = std::getenv("CS_IMPORT_PATH");
		if (import_path != nullptr)
			return process_path(import_path);
		else
			return process_path(get_sdk_path() + cs::path_separator + "imports");
	}

	class process_type final {
		// Initializations
		void init_grammar();
		void init_runtime();
	public:
		translator_type translator;
		compiler_type compiler;
		instance_type instance;
		runtime_type runtime;
		process_type()=delete;
		process_type(const process&)=delete;
		explicit process_type(const array &);
		process_type(int, const char *[]);
		~process_type();
	};
}