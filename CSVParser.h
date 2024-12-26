#pragma once

#include <iostream>
#include <sstream>
#include <limits>
#include <tuple>
#include <string>
#include <vector>

template<typename... Args>
class CSVIterator;

template<typename... Args>
class CSVParser {
    std::istream &stream;
    size_t skipLines;
    char rowDelimiter;
    char colDelimiter;
    char escapeChar;

public:
    explicit CSVParser(std::istream &in, size_t skip = 0, char rowDelim = '\n', char colDelim = ',', char escChar = '"')
            : stream(in), skipLines(skip), rowDelimiter(rowDelim), colDelimiter(colDelim), escapeChar(escChar) {
        for (size_t i = 0; i < skipLines && stream.eof(); ++i) {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), rowDelimiter);
        }
    }

    CSVIterator<Args...> begin() { return CSVIterator<Args...>(stream, rowDelimiter, colDelimiter, escapeChar); }

    CSVIterator<Args...> end() { return CSVIterator<Args...>(); }
};


template<typename... Args>
class CSVIterator {
    std::istream *stream;
    std::string currentLine;
    std::tuple<Args...> currentTuple;
    bool end;
    char rowDelimiter;
    char colDelimiter;
    char escapeChar;
    int lineNumber;

    class Parsing {
    public:
        template<typename Tuple, size_t Index>
        static void parseElement(std::istringstream &in, Tuple& result, char colDelimiter, char escapeChar) {}

        template<typename Tuple, size_t Index, typename T>
        static void parseCurrentElement(std::istringstream &stream, Tuple &result, T &value, char colDelimiter, char escapeChar) {
            if constexpr (std::is_same<T, std::string>::value) {
                parseStringElement(stream, value, colDelimiter, escapeChar);
            } else {
                std::string token;
                if (!std::getline(stream, token, colDelimiter)) {
                    throw std::runtime_error("failed to read token");
                }
                std::istringstream tokenStream(token);
                tokenStream >> value;
                if (tokenStream.fail()) {
                    throw std::runtime_error("conversion failed");

                }
            }
        }

        template<typename Tuple, size_t Index, typename T, typename... Rest>
        static void parseElement(std::istringstream &stream, Tuple &result, char colDelimiter, char escapeChar) {
            parseCurrentElement<Tuple, Index, T>(stream, result, std::get<Index>(result), colDelimiter, escapeChar);
            parseElement<Tuple, Index + 1, Rest...>(stream, result, colDelimiter, escapeChar);
        }

        template<typename... Arguments>
        static std::tuple<Arguments...> parseLine(const std::string &line, char colDelimiter, char escapeChar) {
            std::istringstream stream(line);
            std::tuple<Arguments...> result;
            parseElement<std::tuple<Arguments...>, 0, Arguments...>(stream, result, colDelimiter, escapeChar);
            return result;
        }

    private:
        static void parseStringElement(std::istringstream &stream, std::string &value, char colDelimiter, char escapeChar) {
            stream >> std::ws;
            if (stream.peek() == escapeChar) {
                stream.get();
                std::string temp;
                bool isEscaped = true;

                while (isEscaped) {
                    char c = stream.get();
                    if (stream.eof()) {
                        throw std::runtime_error("unclosed quoted field");
                    }

                    if (c == escapeChar) {
                        if (stream.peek() == escapeChar) {
                            temp += escapeChar;
                            stream.get();
                        } else {
                            isEscaped = false;
                        }
                    } else {
                        temp += c;
                    }
                }

                value = temp;

                if (stream.peek() == colDelimiter) {
                    stream.get();
                } else if (!stream.eof() && stream.peek() != '\n') {
                    throw std::runtime_error("unexpected character after quoted field");
                }
            } else {
                std::getline(stream, value, colDelimiter);
            }
        }

    };

public:
    CSVIterator() : stream(nullptr), end(true), rowDelimiter('\n'), colDelimiter(','), escapeChar('"'), lineNumber(0){}

    explicit CSVIterator(std::istream &in, char rowDelim, char colDelim, char escChar)
            : stream(&in), end(false), rowDelimiter(rowDelim), colDelimiter(colDelim), escapeChar(escChar), lineNumber(0){
        ++(*this);
    }

    CSVIterator &operator++() {
        if (std::getline(*stream, currentLine, rowDelimiter)) {
            ++lineNumber;
            if (currentLine.empty())
                return *this;
            try {
                currentTuple = Parsing::template parseLine<Args...>(currentLine, colDelimiter, escapeChar);
            } catch (const std::exception &err) {
                end = true;
                throw std::runtime_error(std::string(err.what()) + " in line: " + std::to_string(lineNumber));
            }
        } else {
            end = true;
        }
        return *this;
    }

    std::tuple<Args...> operator*() const { return currentTuple; }

    bool operator!=(const CSVIterator &other) const { return end != other.end; }
};

