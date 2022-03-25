#include "json.h"

#include <algorithm>
#include <cassert>
#include <ostream>
#include <sstream>
#include <variant>

using namespace std;

namespace json {

	namespace {

		Node LoadNode(istream& input);

		Node LoadNumber(std::istream& input) {
			using namespace std::literals;

			std::string parsed_num;

			// Считывает в parsed_num очередной символ из input
			auto read_char = [&parsed_num, &input] {
				parsed_num += static_cast<char>(input.get());
				if (!input) {
					throw ParsingError("Failed to read number from stream"s);
				}
			};

			// Считывает одну или более цифр в parsed_num из input
			auto read_digits = [&input, read_char] {
				if (!std::isdigit(input.peek())) {
					throw ParsingError("A digit is expected"s);
				}
				while (std::isdigit(input.peek())) {
					read_char();
				}
			};

			if (input.peek() == '-') {
				read_char();
			}
			// Парсим целую часть числа
			if (input.peek() == '0') {
				read_char();
				// После 0 в JSON не могут идти другие цифры
			}
			else {
				read_digits();
			}

			bool is_int = true;
			// Парсим дробную часть числа
			if (input.peek() == '.') {
				read_char();
				read_digits();
				is_int = false;
			}

			// Парсим экспоненциальную часть числа
			if (int ch = input.peek(); ch == 'e' || ch == 'E') {
				read_char();
				if (ch = input.peek(); ch == '+' || ch == '-') {
					read_char();
				}
				read_digits();
				is_int = false;
			}

			try {
				if (is_int) {
					// Сначала пробуем преобразовать строку в int
					try {
						//return std::stoi(parsed_num);
						return Node((int)std::stoi(parsed_num));
					}
					catch (...) {
						// В случае неудачи, например, при переполнении,
						// код ниже попробует преобразовать строку в double
					}
				}
				//return std::stod(parsed_num);
				return Node((double)std::stod(parsed_num));
			}
			catch (...) {
				throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
			}
		}

		Node LoadString(istream& input) {
			string line;
			char c;

			for (; input.get(c) && c != '\"';) {
				if (c == '\\') {
					input.get(c);
					switch (c) {
					case 'n':line.push_back('\n'); break;
					case 't':line.push_back('\t'); break;
					case 'r':line.push_back('\r'); break;
					case '\\':line.push_back('\\'); break;
					case '\"':line.push_back('\"'); break;
					default:
						line.push_back(c);
						break;
					}
				}
				else {
					line.push_back(c);
				}
			}

			if (c != '\"') {
				throw ParsingError("No closing quote");
			}

			return Node(move(line));
		}

		Node LoadArray(istream& input) {
			if (input.eof()) {
				throw ParsingError("");
			}
			Array result;

			for (char c; input >> c && c != ']';) {
				if (c != ',') {
					input.putback(c);
				}
				result.push_back(LoadNode(input));
			}

			return Node(move(result));
		}

		Node LoadDict(istream& input) {
			if (input.eof()) {
				throw ParsingError("");
			}
			Dict result;

			for (char c; input >> c && c != '}';) {
				if (c == ',') {
					input >> c;
				}

				string key = LoadString(input).AsString();
				input >> c;
				result.insert({ move(key), LoadNode(input) });
			}

			return Node(move(result));
		}

		Node LoadNull() {
			return Node(nullptr);
		}

		Node LoadBool(bool value) {
			return Node(value);
		}

		Node LoadNode(istream& input) {

			char c;
			input >> c;

			// массив Array
			if (c == '[') {
				if (input >> c) {
					// if(input.eof()) - не работает
					input.putback(c);
					return LoadArray(input);
				}
				else {
					throw ParsingError("Empty array");
				}
			}
			// словарь Dict
			else if (c == '{') {
				if (input >> c) {
					input.putback(c);
					return LoadDict(input);
				}
				else {
					throw ParsingError("Empty dictonary");
				}

			}
			// строка String
			else if (c == '\"') {
				return LoadString(input);
			}
			// true/false/null
			else if (isalpha(c)) {
				string temp;
				while (c != '}' && c != ']' && c != ',' && c != '\n' && !input.eof()) {

					temp.push_back(c);
					input.get(c);
				}
				input.putback(c);

				if (temp == "null") {
					return LoadNull();
				}
				else if (temp == "true") {
					return	LoadBool(true);
				}
				else if (temp == "false") {
					return	LoadBool(false);
				}
				// все остальное - неправильный ввод
				else {
					throw ParsingError("Failed to convert "s + temp + " to true, false or null"s);
				}
			}
			// число
			else if (isdigit(c) || c == '-') {
				input.putback(c);
				return  LoadNumber(input);
			}
			// не число, не буква, не массив, ...
			else {
				throw ParsingError("");
			}
		}

	}  // namespace

	bool Node::IsNull()const {
		return std::holds_alternative<std::nullptr_t>(document_);
	}

	bool Node::IsInt()const {
		return std::holds_alternative<int>(document_);
	}

	bool Node::IsDouble()const {
		return std::holds_alternative<double>(document_) || std::holds_alternative<int>(document_);
	}

	bool Node::IsPureDouble()const {
		return std::holds_alternative<double>(document_) && !std::holds_alternative<int>(document_);
	}

	bool Node::IsString()const {
		return std::holds_alternative<std::string>(document_);
	}

	bool Node::IsBool()const {
		return std::holds_alternative<bool>(document_);
	}

	bool Node::IsArray()const {
		return std::holds_alternative<Array>(document_);
	}

	bool Node::IsMap()const {
		return std::holds_alternative<Dict>(document_);
	}

	const Array& Node::AsArray() const {
		if (!holds_alternative<Array>(document_)) {
			throw logic_error("value is not Array-type");
		}
		return std::get<Array>(document_);
	}

	const Dict& Node::AsMap() const {
		if (!holds_alternative<Dict>(document_)) {
			throw logic_error("value is not Dictonary-type");
		}
		return std::get<Dict>(document_);
	}

	int Node::AsInt() const {
		if (!holds_alternative<int>(document_)) {
			throw logic_error("value is not int-type");
		}
		return std::get<int>(document_);
	}

	double Node::AsDouble() const {
		if (std::holds_alternative<double>(document_)) {
			return std::get<double>(document_);
		}
		else if (std::holds_alternative<int>(document_))
			return std::get<int>(document_);
		else {
			throw logic_error("value is not int or double-type");
		}
	}

	const string& Node::AsString() const {
		if (!holds_alternative<string>(document_)) {
			throw logic_error("value is not String-type");
		}
		return std::get<std::string>(document_);
	}

	bool Node::AsBool() const {
		if (!holds_alternative<bool>(document_)) {
			throw logic_error("value is not bool-type");
		}
		return std::get<bool>(document_);
	}

	Document::Document(Node root)
		: root_(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root_;
	}

	Document Load(istream& input) {
		return Document{ LoadNode(input) };
	}

	void PrintStrng(ostream& out, const string& str) {
		out << '\"';
		for (char c : str) {
			switch (c)
			{
			case '\\':out << "\\\\"; break;
			case '\r':out << "\\\r"; break;
			case '\"':out << "\\\""; break;
			case '\t':out << "\\\t"; break;
			case '\n':out << "\\\n"; break;
			default:out << c;
			}
		}
		out << "\"";
	}

	struct VisitTypeDocument {
		ostream& out;
		void operator()(nullptr_t) const { out << "null"; }

		void operator()(double value) const { out << value; }

		void operator()(bool value) const {
			out << boolalpha;
			out << value;
		}

		void operator()(int value) const { out << value; }

		void operator()(Dict value) const {

			out << "{";
			size_t size = 0;

			for (auto doc : value) {
				std::ostringstream strm;
				out << "\"" << doc.first << "\": ";
				if (doc.second.IsString()) {
					PrintStrng(out, doc.second.AsString());
				}
				else {
					//strm.clear();
					std::visit(VisitTypeDocument{ strm }, doc.second.GetJsonDocument());
					out << strm.str();
				}
				size++;
				if (size < value.size()) {
					out << ",";
				}
			}
			out << "}";
		}

		void operator()(Array arr) const {
			size_t i = 0;
			out << "[";
			for (i = 0; i < arr.size(); ++i) {
				std::ostringstream strm;
				if (arr[i].IsString()) {
					PrintStrng(out, arr[i].AsString());
				}
				else {
					std::visit(VisitTypeDocument{ strm }, arr[i].GetJsonDocument());
					out << strm.str();
					//strm.clear();
				}
				if (i != arr.size() - 1) {
					out << ",";
				}
			}
			out << "]";

		}

		void operator()(std::string str) const {
			PrintStrng(out, str);
		}
	};

	void Print(const Document& doc, std::ostream& output) {
		std::ostringstream strm;
		std::visit(VisitTypeDocument{ strm }, doc.GetRoot().GetJsonDocument());
		output << strm.str();
	}

	bool operator== (const Node& lhs, const Node& rhs) {
		return	lhs.GetJsonDocument() == rhs.GetJsonDocument();
	}

	bool operator!= (const Node& lhs, const Node& rhs) {
		return !(lhs == rhs);
	}

	bool operator==(const Document& lhs, const Document& rhs) {
		return lhs.GetRoot().GetJsonDocument() == rhs.GetRoot().GetJsonDocument();
	}

	bool operator!=(const Document& lhs, const Document& rhs) {
		return !(lhs == rhs);
	}

}  // namespace json

