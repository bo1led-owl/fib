#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <utility>

#include "number.hh"

namespace {
template <class T>
concept Context = std::constructible_from<T> && requires(T& a, Number::Digit n) {
    a.prepare(n);
    { a.fib(n) } -> std::convertible_to<const Number&>;
};

class LinearContext final {
public:
    Number a;
    Number b;

    void prepare(size_t n) {
        a.reserveForNthFib(n);
        b.reserveForNthFib(n);
    }

    Number& fib(size_t n) {
        a.assign(0);
        b.assign(1);
        for (size_t i = 0; i < n; ++i) {
            a += b;
            std::swap(a, b);
        }

        return a;
    }
};

class MatExpContext final {
    struct Vector final {
        Number a;
        Number b;
        Number c;

        Vector() : a{}, b{}, c{} {}
        Vector(Number::Digit a, Number::Digit b, Number::Digit c) : a{a}, b{b}, c{c} {}

        void reserveForNthFib(Number::Digit n) {
            a.reserveForNthFib(n);
            b.reserveForNthFib(n);
            c.reserveForNthFib(n);
        }

        void assign(Number::Digit a_val, Number::Digit b_val, Number::Digit c_val) {
            a.assign(a_val);
            b.assign(b_val);
            c.assign(c_val);
        }

        Vector(const Vector&) = delete;
        Vector& operator=(const Vector&) = delete;

        Vector(Vector&& other) {
            std::swap(a, other.a);
            std::swap(b, other.b);
            std::swap(c, other.c);
        }

        Vector& operator=(Vector&& other) {
            std::swap(a, other.a);
            std::swap(b, other.b);
            std::swap(c, other.c);

            return *this;
        }

        static void mul(Vector& res, const Vector& a, const Vector& b) {
            Number::mul(res.a, a.a, b.a);
            Number::mul(res.b, a.a, b.b);
            Number::mul(res.c, a.c, b.c);

            Number::mulAdd(res.b, a.b, b.c);
            Number::mulAddToTwoNumbers(res.a, res.c, a.b, b.b);
        }
    };

    /*
    a b
    b c
    */
    Vector a, b, c;

public:
    MatExpContext() : a{}, b{}, c{} {}

    void prepare(Number::Digit n) {
        a.reserveForNthFib(n);
        b.reserveForNthFib(n);
        c.reserveForNthFib(n);
    }

    Number& fib(Number::Digit n) {
        a.assign(1, 0, 1);
        b.assign(1, 1, 0);

        while (n > 0) {
            if (n % 2 != 0) {
                Vector::mul(c, a, b);
                std::swap(a, c);
            }

            Vector::mul(c, b, b);
            std::swap(b, c);
            n /= 2;
        }

        return a.b;
    }
};

template <Context Ctx>
class Environment final {
    static constexpr size_t THREADS = 4;
    static constexpr size_t REPETITIONS = 15;
    static constexpr Number::Digit STEP = 1000;
    static constexpr Number::Digit START = 0;
    static constexpr double MAX_TIME = 1.0;
    static constexpr double EPS = 0.25;

    std::atomic<Number::Digit> N = START;
    std::atomic<bool> running = true;

    template <typename T>
    static inline void doNotOptimizeAway(const T& x) noexcept {
        __asm__ volatile("nop" ::"r"(&x));
    }

    using Clock = std::chrono::steady_clock;

    void measure() {
        Ctx ctx{};

        while (running) {
            Number::Digit n = N;
            N += STEP;

            ctx.prepare(n);

            double time = std::numeric_limits<double>::max();

            for (size_t i = 0; i < REPETITIONS; ++i) {
                auto start = Clock::now();

                Number& res = ctx.fib(n);
                doNotOptimizeAway(res);

                auto end = Clock::now();

                auto delta = end - start;
                size_t micros = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
                time = std::min(time, micros / 1'000'000.0);
            }

            if (time > MAX_TIME + EPS) {
                running = false;
            }

            std::cout << n << ": " << time << '\n';
        }
    }

public:
    void run() {
        std::vector<std::jthread> threads;
        for (size_t i = 0; i < THREADS; ++i) {
            threads.emplace_back(&Environment<Ctx>::measure, this);
        }
    }
};

template <Context Ctx>
void printRange(Number::Digit l, Number::Digit r) {
    Ctx ctx{};

    for (size_t i = l; i <= r; ++i) {
        Number& res = ctx.fib(i);

        std::cout << i << ":\t" << res.toString() << '\n';
    }
}

template <typename T>
std::optional<T> parse(const std::string_view s) {
    T value;
    if (std::from_chars(s.data(), s.data() + s.size(), value).ec == std::errc{}) {
        return value;
    } else {
        return std::nullopt;
    }
};
}  // namespace

int main(int argc, const char** argv) {
#ifdef LINEAR
    using Ctx = LinearContext;
#endif
#ifdef MATEXP
    using Ctx = MatExpContext;
#endif

    if (argc == 1) {
        Environment<Ctx>{}.run();
    } else if (argc == 3) {
        size_t l = parse<size_t>(argv[1]).value();
        size_t r = parse<size_t>(argv[2]).value();

        printRange<Ctx>(l, r);
    } else {
        std::cerr << "Incorrect number of arguments. Pass no arguments to run measurements, pass two arguments (`l` "
                     "and `r`) to all Fibonacci numbers in range [F_l, F_r]\n";
        return 2;
    }

    return 0;
}
