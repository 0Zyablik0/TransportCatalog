//
// Created by Artem Kryukov on 25.12.2021.
//

#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>


namespace Json {

    class Node : public std::variant<std::vector<Node>,
            std::map<std::string, Node>,
            int,
            std::string,
            bool,
            double> {
    public:
        friend std::ostream;
        using variant::variant;

        const auto &AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }

        const auto &AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }

        int AsInt() const {
            return std::get<int>(*this);
        }

        const auto &AsString() const {
            return std::get<std::string>(*this);
        }

        const auto &AsBool() const {
            return std::get<bool>(*this);
        }

        const auto AsDouble() const {
            return std::holds_alternative<double>(*this) ? std::get<double>(*this) : std::get<int>(*this);
        }
    };


    class Document {
    public:
        explicit Document(Node root);

        const Node &GetRoot() const;

    private:
        Node root;
    };


    Document Load(std::istream &input);

}

std::ostream& operator<<(std::ostream& stream, const Json::Node &node);

std::ostream& operator<<(std::ostream& stream, const Json::Document &document);
