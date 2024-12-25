#pragma once
#include <iostream>
#include <tuple>

template <typename Tuple, unsigned cur, unsigned tuple_size>
class PrintTuple
{
public:
    static void print(std::ostream& out, const Tuple& tuple){
        out << std::get<cur> (tuple) << ", ";
        PrintTuple<Tuple, cur + 1, tuple_size>::print(out, tuple);
    }
};

template<typename Tuple, unsigned tuple_size>
class PrintTuple<Tuple, tuple_size, tuple_size>
{
public:
    static void print(std::ostream& out, const Tuple& tuple) {
        out << std::get<tuple_size>(tuple);
    }
};

template <typename ...Args>
std::ostream& operator<<(std::ostream &out, const std::tuple<Args...>& arg){
    out << "(";
    PrintTuple<std::tuple<Args...>, 0, sizeof...(Args) - 1>::print(out, arg);
    out << ")";
    return out;
}
