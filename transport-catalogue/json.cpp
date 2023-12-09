#include "json.h"

using namespace std;

namespace json {

	namespace {
		Node LoadNode(istream& input);

		Node LoadArray(istream& input) {
			Array result;

			char c;
			for (; input >> c && c != ']';) {
				if (c != ',') {
					input.putback(c);
				}
				result.push_back(LoadNode(input));
			}

			if (c != ']') {
				throw ParsingError("Error parsing Array");
			}

			return Node(move(result));
		}

		Number LoadNumber(std::istream& input) {
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
						return std::stoi(parsed_num);
					}
					catch (...) {
						// В случае неудачи, например, при переполнении,
						// код ниже попробует преобразовать строку в double
					}
				}
				return std::stod(parsed_num);
			}
			catch (...) {
				throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
			}
		}

		// Считывает содержимое строкового литерала JSON-документа
		// Функцию следует использовать после считывания открывающего символа ":
		std::string LoadString(std::istream& input) {
			using namespace std::literals;

			auto it = std::istreambuf_iterator<char>(input);
			auto end = std::istreambuf_iterator<char>();
			std::string s;
			while (true) {
				if (it == end) {
					// Поток закончился до того, как встретили закрывающую кавычку?
					throw ParsingError("String parsing error");
				}
				const char ch = *it;
				if (ch == '"') {
					// Встретили закрывающую кавычку
					++it;
					break;
				}
				else if (ch == '\\') {
					// Встретили начало escape-последовательности
					++it;
					if (it == end) {
						// Поток завершился сразу после символа обратной косой черты
						throw ParsingError("String parsing error");
					}
					const char escaped_char = *(it);
					// Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
					switch (escaped_char) {
					case 'n':
						s.push_back('\n');
						break;
					case 't':
						s.push_back('\t');
						break;
					case 'r':
						s.push_back('\r');
						break;
					case '"':
						s.push_back('"');
						break;
					case '\\':
						s.push_back('\\');
						break;
					default:
						// Встретили неизвестную escape-последовательность
						throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
					}
				}
				else if (ch == '\n' || ch == '\r') {
					// Строковый литерал внутри- JSON не может прерываться символами \r или \n
					throw ParsingError("Unexpected end of line"s);
				}
				else {
					// Просто считываем очередной символ и помещаем его в результирующую строку
					s.push_back(ch);
				}
				++it;
			}

			return s;
		}

		Node LoadDict(istream& input) {
			Dict result;

			char c;
			for (; input >> c && c != '}';) {
				if (c == ',') {
					input >> c;
				}

				string key = LoadString(input);
				input >> c;
				result.insert({ move(key), LoadNode(input) });
			}

			if (c != '}') {
				throw ParsingError("Error parsing map");
			}

			return Node(move(result));
		}

		Node LoadNull(istream& input) {
			std::vector<char> null(3);
			input.read(&null[0], 3);
			if (null != std::vector<char>{'u', 'l', 'l'}) {
				throw ParsingError("Error reading null value");
			}
			return Node{};
		}

		Node LoadFalse(istream& input) {
			std::vector<char> f(4);
			input.read(&f[0], 4);
			if (f != std::vector<char>{'a', 'l', 's', 'e'}) {
				throw ParsingError("Error reading bool value");
			}
			return Node{ false };
		}

		Node LoadTrue(istream& input) {
			std::vector<char> t(3);
			input.read(&t[0], 3);
			if (t != std::vector<char>{'r', 'u', 'e'}) {
				throw ParsingError("Error reading bool value");
			}
			return Node{ true };
		}

		Node LoadNode(istream& input) {
			char c;
			input >> c;

			if (c == '[') {
				return LoadArray(input);
			}
			else if (c == '{') {
				return LoadDict(input);
			}
			else if (c == '"') {
				return LoadString(input);
			}
			else if (c == 'n') {
				return LoadNull(input);
			}
			else if (c == 'f') {
				return LoadFalse(input);
			}
			else if (c == 't') {
				return LoadTrue(input);
			}
			else {
				input.putback(c);
				return LoadNumber(input);
			}
		}

	}  // namespace

	Node::Node() {

	}

	Node::Node(bool b)
		: value_(b) {
	}

	Node::Node(Array array)
		: value_(move(array)) {
	}

	Node::Node(Dict map)
		: value_(move(map)) {
	}

	Node::Node(int value)
		: value_(value) {
	}

	Node::Node(double value)
		: value_(value) {

	}

	Node::Node(string value)
		: value_(move(value)) {
	}

	Node::Node(Number n) {
		if (std::holds_alternative<int>(n)) {
			value_ = std::get<int>(n);
		}
		else {
			value_ = std::get<double>(n);
		}
	}

	bool Node::IsInt() const {
		return std::holds_alternative<int>(value_);
	}
	bool Node::IsDouble() const {
		return std::holds_alternative<double>(value_) ||
			std::holds_alternative<int>(value_);
	}
	bool Node::IsPureDouble() const {
		return std::holds_alternative<double>(value_);
	}
	bool Node::IsBool() const {
		return std::holds_alternative<bool>(value_);
	}
	bool Node::IsString() const {
		return std::holds_alternative<std::string>(value_);
	}
	bool Node::IsNull() const {
		return std::holds_alternative<std::nullptr_t>(value_);
	}
	bool Node::IsArray() const {
		return std::holds_alternative<Array>(value_);
	}
	bool Node::IsMap() const {
		return std::holds_alternative<Dict>(value_);
	}

	int Node::AsInt() const {
		if (IsInt()) {
			return std::get<int>(value_);
		}
		throw std::logic_error("Type is not int");
	}

	bool Node::AsBool() const {
		if (IsBool()) {
			return std::get<bool>(value_);
		}
		throw std::logic_error("Type is not bool");
	}

	double Node::AsDouble() const {
		if (IsPureDouble()) {
			return std::get<double>(value_);
		}
		else if (IsInt()) {
			return (double)std::get<int>(value_);
		}
		throw std::logic_error("Type is not double");
	}

	const std::string& Node::AsString() const {
		if (IsString()) {
			return std::get<std::string>(value_);
		}
		throw std::logic_error("Type is not string");
	}

	const Array& Node::AsArray() const {
		if (IsArray()) {
			return std::get<Array>(value_);
		}
		throw std::logic_error("Type is not array");
	}

	const Dict& Node::AsMap() const {
		if (IsMap()) {
			return  std::get<Dict>(value_);
		}
		throw std::logic_error("Type is not map");
	}

	bool operator!=(const Node& nl, const Node& nr) {
		return !(nl == nr);
	}

	bool operator==(const Node& nl, const Node& nr) {
		return (nl.IsInt() && nr.IsInt() && nl.AsInt() == nr.AsInt())
			|| (nl.IsPureDouble() && nr.IsPureDouble() && nl.AsDouble() == nr.AsDouble())
			|| (nl.IsBool() && nr.IsBool() && nl.AsBool() == nr.AsBool())
			|| (nl.IsString() && nr.IsString() && nl.AsString() == nr.AsString())
			|| (nl.IsNull() && nr.IsNull())
			|| (nl.IsArray() && nr.IsArray() && nl.AsArray() == nr.AsArray())
			|| (nl.IsMap() && nr.IsMap() && nl.AsMap() == nr.AsMap());
	}

	Document::Document(Node root)
		: root_(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root_;
	}

	Document Load(istream& input) {
		Document d = Document{ LoadNode(input) };
		return d;
	}

	bool operator==(const Document& dl, const Document& dr) {
		return dl.GetRoot() == dr.GetRoot();
	}

	bool operator!=(const Document& dl, const Document& dr) {
		return !(dl.GetRoot() == dr.GetRoot());
	}

	// Перегрузка функции PrintValue для вывода значений null
	void PrintValue(std::nullptr_t, const PrintContext& ctx) {
		ctx.out << "null"sv;
	}

	void PrintValue(const std::string& value, const PrintContext& ctx) {
		using namespace std::literals;

		std::string result = "\"";
		// Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
		for (auto it = value.begin(); it < value.end(); it++) {
			char c = (*it);
			switch (c) {
			case('\n'):
				result += "\\n";
				break;
			case('\t'):
				result += "\\t";
				break;
			case('\r'):
				result += "\\r";
				break;
			case('\"'):
				result += "\\\"";
				break;
			case('\\'):
				result += "\\\\";
				break;
			default:
				result += c;
				break;
			}
		}

		ctx.out << result << "\"";
	}

	void PrintValue(bool value, const PrintContext& ctx) {
		if (value) {
			ctx.out << "true";
		}
		else {
			ctx.out << "false";
		}
	}

	void PrintValue(Array values, const PrintContext& ctx) {
		ctx.out << "[";
		bool isFirst = true;
		for (const Node& n : values) {
			if (!isFirst) {
				ctx.out << ",";
			}
			PrintNode(n, ctx);
			isFirst = false;
		}
		ctx.out << "]";
	}

	void PrintNode(const Node& node, const PrintContext& ctx) {
		std::visit(
			[&ctx](const auto& value) {
				PrintValue(value, ctx.Indented());
			},
			node.GetValue());
	}

	void PrintValue(Dict dict, const PrintContext& ctx) {
		ctx.out << "{";
		bool isFirst = true;
		for (const auto& [key, value] : dict) {
			if (!isFirst) {
				ctx.out << ",";
			}
			ctx.out << "\"" << key << "\": ";
			PrintNode(value, ctx);
			isFirst = false;
		}
		ctx.out << "}";
	}
	// Другие перегрузки функции PrintValue пишутся аналогично

	void Print(const Document& doc, std::ostream& output) {
		PrintNode(doc.GetRoot(), { output });
	}

}  // namespace json