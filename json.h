#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace json {

	class Node;
	using Dict = std::map<std::string, Node>;
	using Array = std::vector<Node>;
	using JSON_document = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
	using Number = std::variant<int, double>;

	// Эта ошибка должна выбрасываться при ошибках парсинга JSON
	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	class Node {
	public:
		Node() = default;
		Node(std::nullptr_t) { document_ = {}; }
		Node(Array array) :document_(array) {}
		Node(Dict dictonary) :document_(dictonary) {}
		Node(bool value) :document_(value) {}
		Node(int value) :document_(value) {}
		Node(double value) :document_(value) {}
		Node(std::string str) :document_(str) {}

		bool IsNull() const;
		bool IsInt() const;
		bool IsDouble() const;
		bool IsPureDouble() const;
		bool IsString() const;
		bool IsBool() const;
		bool IsArray() const;
		bool IsMap() const;

		const Array& AsArray() const;
		bool AsBool() const;
		double AsDouble() const;
		int	AsInt() const;
		const Dict& AsMap() const;
		const std::string& AsString() const;

		const JSON_document& GetJsonDocument() const { return document_; }

		bool operator==(const Node& node) {
			return (*this).GetJsonDocument().index() == node.GetJsonDocument().index();
		}
		bool operator!=(const Node& node) {
			return !((*this) == node);
		}


	private:
		JSON_document document_;
	};

	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

		bool operator==(const Document& doc) {
			return (*this).GetRoot().GetJsonDocument() == doc.GetRoot().GetJsonDocument();
		}
		bool operator!=(const Document& doc) {
			return !((*this) == doc);
		}

	private:
		Node root_;
	};

	Document Load(std::istream& input);

	void Print(const Document& doc, std::ostream& output);

	bool operator== (const Node& lhs, const Node& rhs);
	bool operator!= (const Node& lhs, const Node& rhs);
	bool operator==(const Document& lhs, const Document& rhs);
	bool operator!=(const Document& lhs, const Document& rhs);

}  // namespace json

