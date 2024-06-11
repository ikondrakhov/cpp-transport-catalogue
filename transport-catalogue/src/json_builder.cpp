#include "json_builder.h"

using namespace json;

Builder& Builder::Key(std::string key) {
    if (key_ || !(nodes_stack_.back()->IsDict())) {
        throw std::logic_error("Called Key not for dict: " + key);
    }
    key_ = key;
    return *this;
}

Builder& Builder::Value(Node v) {
    if (nodes_stack_.back()->IsNull()) {
        nodes_stack_.back()->GetValue() = v.GetValue();
    }
    else if (nodes_stack_.back()->IsDict()) {
        if (key_) {
            std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = v;
            key_.reset();
        }
        else {
            throw std::logic_error("No key for dict is found");
        }
    }
    else if (nodes_stack_.back()->IsArray()) {
        std::get<Array>(nodes_stack_.back()->GetValue()).push_back(v);
    }
    else {
        throw std::logic_error("Error occured");
    }
    return *this;
}

DictItemContext& Builder::StartDict() {
    if (nodes_stack_.back()->IsNull()) {
        nodes_stack_.back()->GetValue() = Dict{};
    }
    else if (nodes_stack_.back()->IsDict()) {
        if (key_) {
            std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = Dict{};
            nodes_stack_.push_back(&std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()]);
            key_.reset();
        }
        else {
            throw std::logic_error("No key for dict is found");
        }
    }
    else if (nodes_stack_.back()->IsArray()) {
        std::get<Array>(nodes_stack_.back()->GetValue()).push_back(Dict{});
        nodes_stack_.push_back(&std::get<Array>(nodes_stack_.back()->GetValue()).back());
    }
    else {
        throw std::logic_error("Logic error");
    }
    return *this;
}

ArrayItemContext& Builder::StartArray() {
    if (nodes_stack_.back()->IsNull()) {
        nodes_stack_.back()->GetValue() = Array{};
    }
    else if (nodes_stack_.back()->IsDict()) {
        if (key_) {
            std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = Array{};
            nodes_stack_.push_back(&std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()]);
            key_.reset();
        }
        else {
            throw std::logic_error("No key for dict found");
        }
    }
    else if (nodes_stack_.back()->IsArray()) {
        std::get<Array>(nodes_stack_.back()->GetValue()).push_back(Array{});
        nodes_stack_.push_back(&std::get<Array>(nodes_stack_.back()->GetValue()).back());
    }
    else {
        throw std::logic_error("Eror occured");
    }
    return *this;
}

Builder& Builder::EndDict() {
    if (nodes_stack_.back()->IsDict()) {
        nodes_stack_.pop_back();
    }
    else {
        throw std::logic_error("Current node is not dict");
    }
    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.back()->IsArray()) {
        nodes_stack_.pop_back();
    }
    else {
        throw std::logic_error("Current node is not array");
    }
    return *this;
}

Node& Builder::Build() {
    if (root_.IsNull()) {
        throw std::logic_error("Error building json");
    }
    return root_;
}