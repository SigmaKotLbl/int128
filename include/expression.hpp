#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "int128.hpp"
#include <map>
#include <memory>
#include <string>
#include <ostream>

class Expression {
public:
    virtual ~Expression() = default;

    virtual Int128 eval(const std::map<std::string, Int128>& vars) const = 0;

    virtual Expression* clone() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Expression& expr);
};

using Expr = std::unique_ptr<Expression>;

class ConstExpr : public Expression {
public:
    explicit ConstExpr(Int128 value);
    Int128 eval(const std::map<std::string, Int128>&) const override;
    Expression* clone() const override;
    Int128 get_value() const;
private:
    Int128 value_;
};

class VariableExpr : public Expression {
public:
    explicit VariableExpr(std::string name);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    std::string get_name() const;
private:
    std::string name_;
};

class NegateExpr : public Expression {
public:
    explicit NegateExpr(Expr expr);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    const Expression& get_operand() const;
private:
    Expr expr_;
};

class AddExpr : public Expression {
public:
    AddExpr(Expr left, Expr right);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    const Expression& get_left() const;
    const Expression& get_right() const;
private:
    Expr left_, right_;
};

class SubtractExpr : public Expression {
public:
    SubtractExpr(Expr left, Expr right);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    const Expression& get_left() const;
    const Expression& get_right() const;
private:
    Expr left_, right_;
};

class MultiplyExpr : public Expression {
public:
    MultiplyExpr(Expr left, Expr right);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    const Expression& get_left() const;
    const Expression& get_right() const;
private:
    Expr left_, right_;
};

class DivideExpr : public Expression {
public:
    DivideExpr(Expr left, Expr right);
    Int128 eval(const std::map<std::string, Int128>& vars) const override;
    Expression* clone() const override;
    const Expression& get_left() const;
    const Expression& get_right() const;
private:
    Expr left_, right_;
};


inline Expr Const(int64_t value) {
    return std::make_unique<ConstExpr>(Int128(value));
}

inline Expr Const(const Int128& value) {
    return std::make_unique<ConstExpr>(value);
}

inline Expr Variable(const std::string& name) {
    return std::make_unique<VariableExpr>(name);
}

inline Expr Negate(Expr expr) {
    return std::make_unique<NegateExpr>(std::move(expr));
}

inline Expr Add(Expr left, Expr right) {
    return std::make_unique<AddExpr>(std::move(left), std::move(right));
}

inline Expr Subtract(Expr left, Expr right) {
    return std::make_unique<SubtractExpr>(std::move(left), std::move(right));
}

inline Expr Multiply(Expr left, Expr right) {
    return std::make_unique<MultiplyExpr>(std::move(left), std::move(right));
}

inline Expr Divide(Expr left, Expr right) {
    return std::make_unique<DivideExpr>(std::move(left), std::move(right));
}


inline Expr operator+(Expr lhs, Expr rhs) {
    return Add(std::move(lhs), std::move(rhs));
}

inline Expr operator-(Expr lhs, Expr rhs) {
    return Subtract(std::move(lhs), std::move(rhs));
}

inline Expr operator*(Expr lhs, Expr rhs) {
    return Multiply(std::move(lhs), std::move(rhs));
}

inline Expr operator/(Expr lhs, Expr rhs) {
    return Divide(std::move(lhs), std::move(rhs));
}

inline Expr operator-(Expr expr) {
    return Negate(std::move(expr));
}

#endif