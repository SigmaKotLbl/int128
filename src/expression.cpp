#include "../include/expression.hpp"
#include <iostream>

static void print_expr(std::ostream& os, const Expression& expr, int precedence = 0) {
    if (auto c = dynamic_cast<const ConstExpr*>(&expr)) {
        os << c->get_value();
    } else if (auto v = dynamic_cast<const VariableExpr*>(&expr)) {
        os << v->get_name();
    } else if (auto n = dynamic_cast<const NegateExpr*>(&expr)) {
        os << "-(";
        print_expr(os, n->get_operand(), 0);
        os << ")";
    } else if (auto a = dynamic_cast<const AddExpr*>(&expr)) {
        if (precedence > 1) os << "(";
        print_expr(os, a->get_left(), 1);
        os << " + ";
        print_expr(os, a->get_right(), 1);
        if (precedence > 1) os << ")";
    } else if (auto s = dynamic_cast<const SubtractExpr*>(&expr)) {
        if (precedence > 1) os << "(";
        print_expr(os, s->get_left(), 1);
        os << " - ";
        print_expr(os, s->get_right(), 2);
        if (precedence > 1) os << ")";
    } else if (auto m = dynamic_cast<const MultiplyExpr*>(&expr)) {
        if (precedence > 2) os << "(";
        print_expr(os, m->get_left(), 2);
        os << " * ";
        print_expr(os, m->get_right(), 2);
        if (precedence > 2) os << ")";
    } else if (auto d = dynamic_cast<const DivideExpr*>(&expr)) {
        if (precedence > 2) os << "(";
        print_expr(os, d->get_left(), 2);
        os << " / ";
        print_expr(os, d->get_right(), 3);
        if (precedence > 2) os << ")";
    } else {
        os << "?";
    }
}

std::ostream& operator<<(std::ostream& os, const Expression& expr) {
    print_expr(os, expr, 0);
    return os;
}


ConstExpr::ConstExpr(Int128 value) : value_(value) {}
Int128 ConstExpr::eval(const std::map<std::string, Int128>&) const { return value_; }
Expression* ConstExpr::clone() const { return new ConstExpr(value_); }
Int128 ConstExpr::get_value() const { return value_; }

VariableExpr::VariableExpr(std::string name) : name_(std::move(name)) {}
Int128 VariableExpr::eval(const std::map<std::string, Int128>& vars) const {
    auto it = vars.find(name_);
    if (it == vars.end()) return Int128(0);
    return it->second;
}
Expression* VariableExpr::clone() const { return new VariableExpr(name_); }
std::string VariableExpr::get_name() const { return name_; }

NegateExpr::NegateExpr(Expr expr) : expr_(std::move(expr)) {}
Int128 NegateExpr::eval(const std::map<std::string, Int128>& vars) const {
    return -(expr_->eval(vars));
}
Expression* NegateExpr::clone() const {
    return new NegateExpr(Expr(expr_->clone()));
}
const Expression& NegateExpr::get_operand() const { return *expr_; }

AddExpr::AddExpr(Expr left, Expr right) : left_(std::move(left)), right_(std::move(right)) {}
Int128 AddExpr::eval(const std::map<std::string, Int128>& vars) const {
    return left_->eval(vars) + right_->eval(vars);
}
Expression* AddExpr::clone() const {
    return new AddExpr(Expr(left_->clone()), Expr(right_->clone()));
}
const Expression& AddExpr::get_left() const { return *left_; }
const Expression& AddExpr::get_right() const { return *right_; }

SubtractExpr::SubtractExpr(Expr left, Expr right) : left_(std::move(left)), right_(std::move(right)) {}
Int128 SubtractExpr::eval(const std::map<std::string, Int128>& vars) const {
    return left_->eval(vars) - right_->eval(vars);
}
Expression* SubtractExpr::clone() const {
    return new SubtractExpr(Expr(left_->clone()), Expr(right_->clone()));
}
const Expression& SubtractExpr::get_left() const { return *left_; }
const Expression& SubtractExpr::get_right() const { return *right_; }

MultiplyExpr::MultiplyExpr(Expr left, Expr right) : left_(std::move(left)), right_(std::move(right)) {}
Int128 MultiplyExpr::eval(const std::map<std::string, Int128>& vars) const {
    return left_->eval(vars) * right_->eval(vars);
}
Expression* MultiplyExpr::clone() const {
    return new MultiplyExpr(Expr(left_->clone()), Expr(right_->clone()));
}
const Expression& MultiplyExpr::get_left() const { return *left_; }
const Expression& MultiplyExpr::get_right() const { return *right_; }

DivideExpr::DivideExpr(Expr left, Expr right) : left_(std::move(left)), right_(std::move(right)) {}
Int128 DivideExpr::eval(const std::map<std::string, Int128>& vars) const {
    return left_->eval(vars) / right_->eval(vars);
}
Expression* DivideExpr::clone() const {
    return new DivideExpr(Expr(left_->clone()), Expr(right_->clone()));
}
const Expression& DivideExpr::get_left() const { return *left_; }
const Expression& DivideExpr::get_right() const { return *right_; }