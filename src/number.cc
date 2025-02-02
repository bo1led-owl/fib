#include "number.hh"

#include <algorithm>
#include <cstring>

Number::Number() : m_digits{} {}
Number::Number(Digit value) : Number{} {
    assign(value);
}

void Number::assign(this Number& self, Digit value) {
    self.m_digits.clear();
    if (value > 0) {
        self.m_digits.push_back(value);
    }
}

Number::Digit* Number::data(this Number& self) noexcept {
    return self.m_digits.data();
}
const Number::Digit* Number::data(this Number const& self) noexcept {
    return self.m_digits.data();
}

Number::Digit& Number::operator[](this Number& self, size_t i) noexcept {
    return self.m_digits[i];
}
const Number::Digit& Number::operator[](this Number const& self, size_t i) noexcept {
    return self.m_digits[i];
}

Number::Number(const Number& other) : m_digits{other.m_digits} {}
Number& Number::operator=(this Number& self, const Number& other) {
    self.m_digits = other.m_digits;
    return self;
}

Number::Number(Number&& other) noexcept : m_digits{std::move(other.m_digits)} {}
Number& Number::operator=(this Number& self, Number&& other) noexcept {
    std::swap(self.m_digits, other.m_digits);
    return self;
}

size_t Number::size(this Number const& self) noexcept {
    return self.m_digits.size();
}

Number::Digit Number::getOr(this Number const& self, size_t i, Digit default_val) noexcept {
    if (i >= self.m_digits.size()) {
        return default_val;
    }
    return self.m_digits[i];
}

void Number::reserveForNthFib(this Number& self, Digit n) {
    self.reserve(n / DIGIT_BITS + 12);
}

void Number::reserve(this Number& self, size_t cap) {
    size_t cur_cap = self.m_digits.capacity();

    if (cur_cap >= cap) {
        return;
    }

    if (cur_cap * 2 < cap) {
        self.m_digits.reserve(cap);
    } else {
        self.m_digits.reserve(cur_cap * 2);
    }
}

std::string Number::toString(this Number const& self) {
    Number num{self};
    return std::move(num).toString();
}

std::string Number::toString(this Number&& self) {
    if (self.size() == 0) {
        return "0";
    }

    std::string s;
    while (self.size() > 0) {
        s.push_back((char)self.div10() + '0');
    }
    std::reverse(s.begin(), s.end());
    return s;
}

Number& Number::operator+=(this Number& a, const Number& b) {
    Digit carry = 0;
    size_t blen = b.size();
    size_t maxlen = std::max(a.size(), b.size());
    a.m_digits.resize(maxlen, 0);

    for (size_t i = 0; i < blen; ++i) {
        Digit res;
        Digit lhs = a[i];
        Digit rhs = b[i];
        carry = __builtin_add_overflow(lhs, carry, &res);
        carry += __builtin_add_overflow(res, rhs, a.data() + i);
    }

    for (size_t i = blen; i < maxlen; ++i) {
        Digit lhs = a[i];
        carry = __builtin_add_overflow(lhs, carry, a.data() + i);
    }

    if (carry > 0) {
        a.m_digits.push_back(carry);
    }

    return a;
}

void Number::addToTwoNumbers(Number& a1, Number& a2, const Number& b) {
    Digit carry1 = 0;
    Digit carry2 = 0;

    size_t maxlen = std::max(std::max(a1.size(), a2.size()), b.size());

    a1.m_digits.resize(maxlen, 0);
    a2.m_digits.resize(maxlen, 0);

    for (size_t i = 0; i < maxlen; ++i) {
        Digit res;
        carry1 = __builtin_add_overflow(a1.getOr(i, 0), carry1, &res);
        carry1 += __builtin_add_overflow(res, b.getOr(i, 0), a1.data() + i);

        carry2 = __builtin_add_overflow(a2.getOr(i, 0), carry2, &res);
        carry2 += __builtin_add_overflow(res, b.getOr(i, 0), a2.data() + i);
    }

    if (carry1 > 0) {
        a1.m_digits.push_back(carry1);
    }
    if (carry2 > 0) {
        a2.m_digits.push_back(carry2);
    }
}

void Number::mul(Number& res, const Number& lhs, const Number& rhs) {
    res.m_digits.clear();

    for (size_t i = 0; i < lhs.size(); ++i) {
        Digit carry = 0;

        for (size_t j = 0; j < rhs.size() || carry != 0; ++j) {
            if (i + j >= res.size()) {
                assert(res.m_digits.capacity() > res.m_digits.size() + 1);
                res.m_digits.push_back(0);
            }

            DoubleDigit cur = static_cast<DoubleDigit>(lhs[i]) * static_cast<DoubleDigit>(rhs.getOr(j, 0));
            cur += res[i + j];
            cur += carry;

            res[i + j] = cur & MASK;
            carry = (cur >> DIGIT_BITS) & MASK;
        }
    }
}

void Number::mulAdd(Number& res, const Number& lhs, const Number& rhs) {
    for (size_t i = 0; i < lhs.size(); ++i) {
        Digit carry = 0;

        for (size_t j = 0; j < rhs.size() || carry != 0; ++j) {
            if (i + j >= res.size()) {
                assert(res.m_digits.capacity() > res.m_digits.size() + 1);
                res.m_digits.push_back(0);
            }

            DoubleDigit cur = static_cast<DoubleDigit>(lhs[i]) * static_cast<DoubleDigit>(rhs.getOr(j, 0));
            cur += res[i + j];
            cur += carry;

            res[i + j] = cur & MASK;
            carry = (cur >> DIGIT_BITS) & MASK;
        }
    }
}

void Number::mulAddToTwoNumbers(Number& res1, Number& res2, const Number& lhs, const Number& rhs) {
    for (size_t i = 0; i < lhs.size(); ++i) {
        Digit carry1 = 0;
        Digit carry2 = 0;

        for (size_t j = 0; j < rhs.size() || carry1 != 0 || carry2 != 0; ++j) {
            if (i + j >= res1.size()) {
                assert(res1.m_digits.capacity() > res1.m_digits.size() + 1);
                res1.m_digits.push_back(0);
            }
            DoubleDigit cur = static_cast<DoubleDigit>(lhs[i]) * static_cast<DoubleDigit>(rhs.getOr(j, 0));

            cur += res1[i + j];
            cur += carry1;

            res1[i + j] = cur & MASK;
            carry1 = (cur >> DIGIT_BITS) & MASK;

            if (i + j >= res2.size()) {
                assert(res2.m_digits.capacity() > res2.m_digits.size() + 1);
                res2.m_digits.push_back(0);
            }

            cur = static_cast<DoubleDigit>(lhs[i]) * static_cast<DoubleDigit>(rhs.getOr(j, 0));

            cur += res2[i + j];
            cur += carry2;

            res2[i + j] = cur & MASK;
            carry2 = (cur >> DIGIT_BITS) & MASK;
        }
    }
}

Number::Digit Number::div10(this Number& self) {
    Digit carry = 0;
    for (size_t i = 0; i < self.m_digits.size(); ++i) {
        size_t index = (self.m_digits.size() - i - 1);

        DoubleDigit cur = static_cast<DoubleDigit>(self.m_digits[index]) + static_cast<DoubleDigit>(carry) * BASE;
        self.m_digits[index] = (cur / 10) & MASK;
        carry = (cur % 10) & MASK;
    }

    while (self.m_digits.size() > 0 && self.m_digits.back() == 0) {
        self.m_digits.pop_back();
    }

    assert(carry < 10);
    return carry;
}
