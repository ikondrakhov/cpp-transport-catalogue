#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Number = std::variant<int, double>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        /* Реализуйте Node, используя std::variant */
        using NodeValue = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node();
        Node(bool b);
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(double value);
        Node(std::string value);
        Node(std::nullptr_t n) : value_(n) {}
        Node(Number n);

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const NodeValue& GetValue() const { return value_; }

    private:
        NodeValue value_;
    };

    bool operator==(const Node& nl, const Node& nr);

    bool operator!=(const Node& nl, const Node& nr);

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    bool operator==(const Document& dl, const Document& dr);

    bool operator!=(const Document& dl, const Document& dr);

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    void PrintNode(const Node& node, const PrintContext& ctx);

    void PrintValue(Dict dict, const PrintContext& out);
    void PrintValue(std::nullptr_t, const PrintContext& out);
    void PrintValue(const std::string& value, const PrintContext& out);
    void PrintValue(bool value, const PrintContext& out);
    void PrintValue(Array values, const PrintContext& out);

    // Шаблон, подходящий для вывода double и int
    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& ctx) {
        ctx.out << value;
    }

}  // namespace json