#pragma once
/*
* Covariant Script Programming Language
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
namespace cs {
	std::shared_ptr<runtime_type> covscript(const std::string &path, bool compile_only)
	{
		// Read from file
		std::deque<char> buff;
		std::ifstream in(path);
		if (!in.is_open())
			throw fatal_error(path + ": No such file or directory");
		while (!in.eof())
			buff.push_back(in.get());
		// Init resources
		runtime.new_instance();
		token_value::clean();
		init();
		// Lexer
		std::deque<token_base *> tokens;
		translate_into_tokens(buff, tokens, path);
		// Parser
		std::deque<statement_base *> statements;
		translate_into_statements(tokens, statements);
		// Process constant values
		token_value::mark();
		// Exit if compile_only is true
		if (compile_only)
			return runtime.pop_instance();
		// Run the instruction
		for (auto &ptr:statements) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw fatal_error("Uncaught exception.");
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
		}
		return runtime.pop_instance();
	}
}
