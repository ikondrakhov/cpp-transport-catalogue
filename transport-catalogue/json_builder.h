#pragma once

#include <map>
#include <memory>
#include <optional>
#include <stdexcept>

#include "json.h"

namespace json {
    class ValueItemContext;
    class DictItemContext;
    class ArrayItemContext;
    class Builder;
    
    class KeyItemContext {
    public:
        virtual ValueItemContext& Value(Node v) = 0;
        virtual DictItemContext& StartDict() = 0;
        virtual ArrayItemContext& StartArray() = 0;
    };
    
    class ValueItemContext {
    public:
        virtual KeyItemContext& Key(std::string key) = 0;
        virtual Builder& EndDict() = 0;
    };
    
    class DictItemContext {
    public:
        virtual KeyItemContext& Key(std::string key) = 0;
        virtual Builder& EndDict() = 0;
    };
    
    class ArrayItemContext {
    public:
        virtual ArrayItemContext& Value(Node v) = 0;
        virtual DictItemContext& StartDict() = 0;
        virtual ArrayItemContext& StartArray() = 0;
        virtual Builder& EndArray() = 0;
    };

    class Builder: KeyItemContext, ValueItemContext, DictItemContext, ArrayItemContext {
    public:
        Builder() : nodes_stack_{ &root_ } {
        }

        Builder& Key(std::string key);
        Builder& Value(Node v);
        DictItemContext& StartDict();
        ArrayItemContext& StartArray();
        Builder& EndDict();
        Builder& EndArray();
        Node& Build();

    private:
        std::optional<std::string> key_;
        Node root_;
        std::vector<Node*> nodes_stack_;
    };

}