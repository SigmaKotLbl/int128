#include "../include/int128.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>


static std::pair<uint64_t, uint64_t> mul64(uint64_t a, uint64_t b) {
    uint32_t a_lo = static_cast<uint32_t>(a);
    uint32_t a_hi = static_cast<uint32_t>(a >> 32);
    uint32_t b_lo = static_cast<uint32_t>(b);
    uint32_t b_hi = static_cast<uint32_t>(b >> 32);

    uint64_t p1 = static_cast<uint64_t>(a_lo) * b_lo;
    uint64_t p2 = static_cast<uint64_t>(a_lo) * b_hi;
    uint64_t p3 = static_cast<uint64_t>(a_hi) * b_lo;
    uint64_t p4 = static_cast<uint64_t>(a_hi) * b_hi;

    uint64_t lo = p1;
    uint64_t hi = (p2 >> 32) + (p3 >> 32) + (p4 << 32);

    lo += (p2 << 32);
    if (lo < (p2 << 32)) ++hi;
    lo += (p3 << 32);
    if (lo < (p3 << 32)) ++hi;

    return {hi, lo};
}

static uint64_t add_with_carry(uint64_t a, uint64_t b, uint64_t& carry) {
    uint64_t sum = a + b;
    uint64_t new_carry = (sum < a) ? 1 : 0;
    sum += carry;
    if (sum < carry) new_carry = 1;
    carry = new_carry;
    return sum;
}

static uint64_t sub_with_borrow(uint64_t a, uint64_t b, uint64_t& borrow) {
    uint64_t diff = a - b;
    uint64_t new_borrow = (a < b) ? 1 : 0;
    a = diff;
    diff = a - borrow;
    if (a < borrow) new_borrow = 1;
    borrow = new_borrow;
    return diff;
}


Int128::Int128() : lo(0), hi(0) {}

Int128::Int128(int64_t value) {
    lo = static_cast<uint64_t>(value);
    hi = (value < 0) ? UINT64_MAX : 0;
}

Int128::Int128(std::string_view str) {
    bool negative = false;
    if (!str.empty() && str[0] == '-') {
        negative = true;
        str.remove_prefix(1);
    }

    while (!str.empty() && str[0] == '0')
        str.remove_prefix(1);
    if (str.empty()) {
        lo = 0;
        hi = 0;
        return;
    }

    Int128 result;
    for (char ch : str) {
        if (!std::isdigit(static_cast<unsigned char>(ch)))
            break;
        int digit = ch - '0';
        Int128 ten(10);
        result = mul_unsigned(result, ten);
        result = add_unsigned(result, Int128(digit));
    }

    if (negative) {
        result.lo = ~result.lo;
        result.hi = ~result.hi;
        uint64_t carry = 1;
        result.lo = add_with_carry(result.lo, 0, carry);
        result.hi = add_with_carry(result.hi, 0, carry);
    }

    *this = result;
}

Int128::Int128(uint64_t low, uint64_t high) : lo(low), hi(high) {}


Int128::operator int64_t() const {
    return static_cast<int64_t>(lo);
}

Int128::operator double() const {
    double result = static_cast<double>(static_cast<int64_t>(hi)) * 18446744073709551616.0;
    result += static_cast<double>(lo);
    return result;
}


std::string Int128::str() const {
    if (hi == 0 && lo == 0) return "0";

    Int128 tmp = *this;
    bool negative = tmp.is_negative();
    if (negative) tmp = -tmp;

    std::string digits;
    Int128 ten(10);
    while (!(tmp.hi == 0 && tmp.lo == 0)) {
        auto [quot, rem] = divmod_unsigned(tmp, ten);
        digits.push_back(static_cast<char>('0' + rem.lo));
        tmp = quot;
    }
    if (negative) digits.push_back('-');
    std::reverse(digits.begin(), digits.end());
    return digits;
}


bool Int128::is_negative() const {
    return (static_cast<int64_t>(hi) < 0);
}

Int128 Int128::abs() const {
    return is_negative() ? -(*this) : *this;
}


Int128 Int128::add_unsigned(const Int128& a, const Int128& b) {
    uint64_t carry = 0;
    uint64_t lo = add_with_carry(a.lo, b.lo, carry);
    uint64_t hi = a.hi + b.hi + carry;
    return Int128(lo, hi);
}

Int128 Int128::sub_unsigned(const Int128& a, const Int128& b) {
    uint64_t borrow = 0;
    uint64_t lo = sub_with_borrow(a.lo, b.lo, borrow);
    uint64_t hi = a.hi - b.hi - borrow;
    return Int128(lo, hi);
}

Int128 Int128::mul_unsigned(const Int128& a, const Int128& b) {
    auto [hi1, lo1] = mul64(a.lo, b.lo);
    auto [hi2, lo2] = mul64(a.hi, b.lo);
    auto [hi3, lo3] = mul64(a.lo, b.hi);

    uint64_t lo = lo1;
    uint64_t hi = hi1;

    uint64_t carry = 0;
    hi = add_with_carry(hi, lo2, carry);

    carry = 0;
    hi = add_with_carry(hi, lo3, carry);

    return Int128(lo, hi);
}

Int128 Int128::div_unsigned(const Int128& a, const Int128& b) {
    return divmod_unsigned(a, b).first;
}

std::pair<Int128, Int128> Int128::divmod_unsigned(const Int128& a, const Int128& b) {
    if (b.hi == 0 && b.lo == 0) {
        return {Int128(0), Int128(0)};
    }

    Int128 q(0), r(0);
    for (int i = 127; i >= 0; --i) {
        r = add_unsigned(r, r);
        uint64_t bit_mask = (i >= 64) ? (1ULL << (i - 64)) : (1ULL << i);
        bool bit = (i >= 64) ? ((a.hi & bit_mask) != 0) : ((a.lo & bit_mask) != 0);
        if (bit) {
            r.lo |= 1;
        }
        if (r >= b) {
            r = sub_unsigned(r, b);
            if (i >= 64) q.hi |= (1ULL << (i - 64));
            else         q.lo |= (1ULL << i);
        }
    }
    return {q, r};
}


Int128 Int128::operator+() const { return *this; }

Int128 Int128::operator-() const {
    Int128 result(~lo, ~hi);
    uint64_t carry = 1;
    result.lo = add_with_carry(result.lo, 0, carry);
    result.hi = add_with_carry(result.hi, 0, carry);
    return result;
}

Int128& Int128::operator+=(const Int128& other) {
    *this = *this + other;
    return *this;
}

Int128& Int128::operator-=(const Int128& other) {
    *this = *this - other;
    return *this;
}

Int128& Int128::operator*=(const Int128& other) {
    *this = *this * other;
    return *this;
}

Int128& Int128::operator/=(const Int128& other) {
    *this = *this / other;
    return *this;
}

Int128 Int128::operator+(const Int128& other) const {
    return add_unsigned(*this, other);
}

Int128 Int128::operator-(const Int128& other) const {
    return *this + (-other);
}

Int128 Int128::operator*(const Int128& other) const {
    bool neg = is_negative() ^ other.is_negative();
    Int128 a = abs();
    Int128 b = other.abs();
    Int128 res = mul_unsigned(a, b);
    return neg ? -res : res;
}

Int128 Int128::operator/(const Int128& other) const {
    bool neg = is_negative() ^ other.is_negative();
    Int128 a = abs();
    Int128 b = other.abs();
    Int128 res = div_unsigned(a, b);
    return neg ? -res : res;
}

bool Int128::operator==(const Int128& other) const {
    return lo == other.lo && hi == other.hi;
}

bool Int128::operator!=(const Int128& other) const {
    return !(*this == other);
}


bool operator<(const Int128& a, const Int128& b) {
    int64_t a_hi = static_cast<int64_t>(a.hi);
    int64_t b_hi = static_cast<int64_t>(b.hi);
    if (a_hi != b_hi) return a_hi < b_hi;
    return a.lo < b.lo;
}

bool operator>(const Int128& a, const Int128& b) { return b < a; }
bool operator<=(const Int128& a, const Int128& b) { return !(b < a); }
bool operator>=(const Int128& a, const Int128& b) { return !(a < b); }


std::ostream& operator<<(std::ostream& os, const Int128& value) {
    os << value.str();
    return os;
}