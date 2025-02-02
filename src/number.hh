#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

class Number final {
public:
    using Digit = uint64_t;
    using DoubleDigit = __uint128_t;

private:
    static constexpr size_t DIGIT_BITS = 8 * sizeof(Digit);
    static constexpr DoubleDigit BASE =
        static_cast<DoubleDigit>(std::numeric_limits<Digit>::max()) + 1;
    static constexpr Digit MASK = std::numeric_limits<Digit>::max();

    std::vector<Digit> m_digits;

    Digit& operator[](this Number& self, size_t i) noexcept;
    const Digit& operator[](this Number const& self, size_t i) noexcept;
    Digit getOr(this Number const& self, size_t i, Digit default_val) noexcept;

    Digit* data(this Number& self) noexcept;
    const Digit* data(this Number const& self) noexcept;

    Digit div10(this Number& self);

public:
    Number();
    explicit Number(Digit value);

    Number(const Number&);
    Number& operator=(this Number& self, const Number& other);
    Number(Number&& other) noexcept;
    Number& operator=(this Number& self, Number&& other) noexcept;

    size_t size(this Number const& self) noexcept;

    void assign(this Number& self, Digit value);

    void reserveForNthFib(this Number& self, Digit n);
    void reserve(this Number& self, size_t cap);

    std::string toString(this Number const& self);
    std::string toString(this Number&& self);

    Number& operator+=(this Number& self, const Number& other);
    static void addToTwoNumbers(Number& a1, Number& a2, const Number& b);

    static void mul(Number& res, const Number& lhs, const Number& rhs);
    static void mulAdd(Number& res, const Number& lhs, const Number& rhs);
    static void mulAddToTwoNumbers(Number& res1, Number& res2,
                                   const Number& lhs, const Number& rhs);
};
