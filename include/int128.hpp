#ifndef INT128_H
#define INT128_H

#include <cstdint>
#include <string>
#include <string_view>
#include <ostream>

class Int128 {
public:
    Int128();
    Int128(int64_t value);
    explicit Int128(std::string_view str);

    explicit operator int64_t() const;
    explicit operator double() const;
    
    std::string str() const;

    Int128 operator+() const;
    Int128 operator-() const;

    Int128& operator+=(const Int128& other);
    Int128& operator-=(const Int128& other);
    Int128& operator*=(const Int128& other);
    Int128& operator/=(const Int128& other);

    Int128 operator+(const Int128& other) const;
    Int128 operator-(const Int128& other) const;
    Int128 operator*(const Int128& other) const;
    Int128 operator/(const Int128& other) const;

    bool operator==(const Int128& other) const;
    bool operator!=(const Int128& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Int128& value);
    
    friend bool operator<(const Int128& a, const Int128& b);
    friend bool operator>(const Int128& a, const Int128& b);
    friend bool operator<=(const Int128& a, const Int128& b);
    friend bool operator>=(const Int128& a, const Int128& b);

private:
    uint64_t lo;
    uint64_t hi;

    Int128(uint64_t low, uint64_t high);
    bool is_negative() const;
    Int128 abs() const;
    static Int128 add_unsigned(const Int128& a, const Int128& b);
    static Int128 sub_unsigned(const Int128& a, const Int128& b);
    static Int128 mul_unsigned(const Int128& a, const Int128& b);
    static Int128 div_unsigned(const Int128& a, const Int128& b);
    static std::pair<Int128, Int128> divmod_unsigned(const Int128& a, const Int128& b);
};

#endif