//
// Created by Artem Kryukov on 25.12.2021.
//

#include "json.h"
#include "test_runner.h"

using namespace std;

namespace Json {

    Document::Document(Node root) : root(move(root)) {
    }

    const Node &Document::GetRoot() const {
        return root;
    }

    Node LoadNode(istream &input);

    Node LoadArray(istream &input) {
        vector<Node> result;

        for (char c; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNum(istream &input) {
        bool is_negative = false;
        if (input.peek() == '-') {
            is_negative = true;
            input.get();
        }
        int int_part = 0;
        while (isdigit(input.peek())) {
            int_part *= 10;
            int_part += input.get() - '0';
        }
        if (input.peek() != '.') {
            return Node(int_part * (is_negative ? -1 : 1));
        }
        input.get();  // '.'
        double result = int_part;
        double frac_mult = 0.1;
        while (isdigit(input.peek())) {
            result += frac_mult * (input.get() - '0');
            frac_mult /= 10;
        }
        return Node(result * (is_negative ? -1 : 1));
    }


    Node LoadString(istream &input) {
        string line;
        getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadBool(istream &input) {
        string s;
        while (isalpha(input.peek())) {
            s.push_back(input.get());
        }
        return Node(s == "true");
    }


    Node LoadDict(istream &input) {
        map<string, Node> result;

        for (char c; input >> c && c != '}';) {
            if (c == ',') {
                input >> c;
            }

            string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(istream &input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (c == 't' or c == 'f') {
            input.putback(c);
            return LoadBool(input);
        } else {
            input.putback(c);
            return LoadNum(input);
        }
    }

    Document Load(istream &input) {
        return Document{LoadNode(input)};
    }

}

std::ostream &operator<<(std::ostream &stream, const Json::Node &node) {
    if (std::holds_alternative<double>(node)) {
        stream << node.AsDouble();
    } else if (std::holds_alternative<std::map<std::string, Json::Node>>(node)) {
        stream << node.AsMap();
    } else if (std::holds_alternative<std::vector<Json::Node>>(node)) {
        stream << node.AsArray();
    } else if (std::holds_alternative<std::string>(node)) {
        stream << "\"" << node.AsString() << "\"";
    } else if (std::holds_alternative<int>(node)) {
        stream << node.AsInt();
    } else if (std::holds_alternative<bool>(node)) {
        if (node.AsBool())
            stream << "true";
        else
            stream << "false";
    }


    return stream;
}

std::ostream &operator<<(std::ostream &stream, const Json::Document &document){
    stream << document.GetRoot();
    return stream;
}