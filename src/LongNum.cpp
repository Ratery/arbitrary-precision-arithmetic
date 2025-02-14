#include "LongNum.hpp"
#include <algorithm>
#include <utility>
#include <stdexcept>

LongNum::LongNum(const bool _is_negative, const unsigned _exp, std::vector<uint32_t>& _limbs)
    : is_negative(_is_negative),
      exp(_exp),
      limbs(std::move(_limbs)) {
}

LongNum::LongNum(unsigned long long x) {
    if (x == 0) {
        return;
    }
    while (x) {
        limbs.push_back(x & UINT32_MAX);
        x >>= base;
    }
}

LongNum::LongNum(long long x) {
    if (x == 0) {
        return;
    }
    is_negative = x < 0;
    x = std::abs(x);
    while (x) {
        limbs.push_back(x & UINT32_MAX);
        x >>= base;
    }
}

bool operator==(const LongNum& lhs, const LongNum& rhs) {
    if (lhs.limbs.empty() && rhs.limbs.empty()) {
        return true;
    }
    if (lhs.is_negative != rhs.is_negative) {
        return false;
    }
    if (lhs.exp < rhs.exp) {
        return lhs.with_precision(rhs.exp) == rhs;
    }
    if (lhs.exp > rhs.exp) {
        return lhs == rhs.with_precision(lhs.exp);
    }
    return lhs.limbs == rhs.limbs;
}

std::strong_ordering operator<=>(const LongNum& lhs, const LongNum& rhs) {
    if (lhs.limbs.empty() && rhs.limbs.empty()) {
        return std::strong_ordering::equal;
    }
    if (lhs.is_negative && !rhs.is_negative) {
        return std::strong_ordering::less;
    }
    if (!lhs.is_negative && rhs.is_negative) {
        return std::strong_ordering::greater;
    }
    if (lhs.exp < rhs.exp) {
        return lhs.with_precision(rhs.exp) <=> rhs;
    }
    if (lhs.exp > rhs.exp) {
        return lhs <=> rhs.with_precision(lhs.exp);
    }
    if (lhs.limbs.size() < rhs.limbs.size()) {
        return lhs.is_negative ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    if (lhs.limbs.size() > rhs.limbs.size()) {
        return lhs.is_negative ? std::strong_ordering::less : std::strong_ordering::greater;
    }
    for (size_t i = 0; i < lhs.limbs.size(); i++) {
        if (lhs.limbs[i] < rhs.limbs[i]) {
            return lhs.is_negative ? std::strong_ordering::greater : std::strong_ordering::less;
        }
        if (lhs.limbs[i] > rhs.limbs[i]) {
            return lhs.is_negative ? std::strong_ordering::less : std::strong_ordering::greater;
        }
    }
    return std::strong_ordering::equal;
}

LongNum LongNum::operator+() const {
    return *this;
}

LongNum LongNum::operator-() const {
    LongNum res(*this);
    if (*this != 0) {  // -0 must be +0
        res.is_negative ^= 1;
    }
    return res;
}

LongNum& LongNum::operator<<=(const unsigned shift) {
    *this = *this << shift;
    return *this;
}

LongNum operator<<(const LongNum& number, const unsigned shift) {
    if (!shift || number == 0) {
        return number;
    }
    const unsigned zeros_cnt = (shift + LongNum::base - 1) / LongNum::base;
    const unsigned r = shift % LongNum::base;
    std::vector<uint32_t> limbs(number.limbs.size() + zeros_cnt, 0);
    for (size_t i = zeros_cnt; i < limbs.size(); i++) {
        limbs[i] = number.limbs[i - zeros_cnt];
    }
    if (r) {
        for (size_t i = zeros_cnt - 1; i < limbs.size() - 1; i++) {
            limbs[i] = (limbs[i] >> (LongNum::base - r)) | (limbs[i + 1] << r);
        }
        limbs.back() >>= LongNum::base - r;
    }
    LongNum res(number.is_negative, number.exp, limbs);
    res.remove_leading_zeros();
    return res;
}

LongNum& LongNum::operator>>=(const unsigned shift) {
    *this = *this >> shift;
    return *this;
}

LongNum operator>>(const LongNum& number, const unsigned shift) {
    if (!shift || number == 0) {
        return number;
    }
    const unsigned to_erase = shift / LongNum::base;
    if (to_erase >= number.limbs.size()) {
        return 0;
    }
    std::vector<uint32_t> limbs(number.limbs.size() - to_erase);
    const unsigned r = shift % LongNum::base;
    for (size_t i = 0; i < limbs.size(); i++) {
        limbs[i] = number.limbs[i + to_erase];
    }
    if (r) {
        for (size_t i = 0; i < limbs.size() - 1; i++) {
            limbs[i] = (limbs[i] >> r) | (limbs[i + 1] << (LongNum::base - r));
        }
        limbs.back() >>= r;
    }
    LongNum res(number.is_negative, number.exp, limbs);
    res.remove_leading_zeros();
    return res;
}

LongNum& LongNum::operator+=(const LongNum& rhs) {
    if (*this == 0) {
        *this = rhs;
        return *this;
    }
    if (rhs == 0) {
        return *this;
    }
    if (is_negative != rhs.is_negative) {
        *this -= -rhs;
        return *this;
    }
    if (exp < rhs.exp) {
        set_precision(rhs.exp);
    } else if (exp > rhs.exp) {
        *this += rhs.with_precision(exp);  // recursive call, this can only happen once
        return *this;
    }
    unsigned carry = 0;
    for (size_t i = 0; i < std::max(limbs.size(), rhs.limbs.size()) || carry; i++) {
        if (i == limbs.size()) {
            limbs.push_back(0);
        }
        limbs[i] += carry;  // TODO: optimize carry calculation here
        carry = limbs[i] < carry;
        if (i < rhs.limbs.size()) {
            limbs[i] += rhs.limbs[i];
            carry += limbs[i] < rhs.limbs[i];
        }
    }
    return *this;
}

LongNum operator+(LongNum lhs, const LongNum& rhs) {
    lhs += rhs;
    return lhs;
}

LongNum& LongNum::operator-=(const LongNum& rhs) {
    if (rhs == 0) {
        return *this;
    }
    if (*this == rhs) {
        *this = 0;
        return *this;
    }
    if (is_negative != rhs.is_negative) {
        *this += -rhs;
        return *this;
    }
    if ((*this < rhs) ^ is_negative) {  // handle cases like -1 - (-2) and 1 - 2
        *this = -(rhs - *this);
        return *this;
    }
    if (exp < rhs.exp) {
        set_precision(rhs.exp);
    } else if (exp > rhs.exp) {
        *this -= rhs.with_precision(exp);  // recursive call, this can only happen once
        return *this;
    }
    unsigned carry = 0;
    for (size_t i = 0; i < rhs.limbs.size() || carry; i++) {
        if (i < rhs.limbs.size()) {
            const unsigned new_carry = limbs[i] < rhs.limbs[i] || (limbs[i] == rhs.limbs[i] && carry);  // TODO: optimize carry calculation here
            limbs[i] -= rhs.limbs[i] + carry;
            carry = new_carry;
        } else {
            limbs[i] -= carry;
            carry = (limbs[i] == UINT32_MAX);
        }
    }
    remove_leading_zeros();
    return *this;
}

LongNum operator-(LongNum lhs, const LongNum& rhs) {
    lhs -= rhs;
    return lhs;
}

LongNum& LongNum::operator*=(const LongNum& rhs) {
    *this = *this * rhs;
    return *this;
}

LongNum operator*(const LongNum& lhs, const LongNum& rhs) {
    if (lhs == 0 || rhs == 0) {
        return 0;
    }
    LongNum res;
    res.exp = lhs.exp + rhs.exp;
    res.limbs.resize(lhs.limbs.size() + rhs.limbs.size());
    for (size_t i = 0; i < lhs.limbs.size(); i++) {
        uint32_t carry = 0;
        for (size_t j = 0; j < rhs.limbs.size() || carry; j++) {
            if (j < rhs.limbs.size()) {
                const uint64_t prod = (uint64_t) lhs.limbs[i] * rhs.limbs[j];
                res.limbs[i + j] += carry;
                carry = res.limbs[i + j] < carry;
                res.limbs[i + j] += prod;
                carry += res.limbs[i + j] < (uint32_t) prod;
                carry += prod >> LongNum::base;
            } else {
                res.limbs[i + j] += carry;
                carry = 0;
            }
        }
    }
    res.remove_leading_zeros();
    res.set_precision(std::max(lhs.exp, rhs.exp));
    res.is_negative = lhs.is_negative ^ rhs.is_negative;
    return res;
}

LongNum& LongNum::operator/=(const LongNum& rhs) {
    *this = *this / rhs;
    return *this;
}

LongNum operator/(LongNum lhs, const LongNum& rhs) {
    auto div_one_digit = [](const LongNum& a, const LongNum& b, LongNum& res) {
        // divide a / b as integers (ignoring exp and sign), len(b) = 1

        res = a;
        unsigned carry = 0;
        for (size_t i = res.limbs.size() - 1; i != (size_t)-1; i--) {
            const uint64_t cur = res.limbs[i] + ((uint64_t)carry << LongNum::base);
            res.limbs[i] = cur / b.limbs.front();
            carry = cur - res.limbs[i] * b.limbs.front();
        }
    };

    auto long_div = [](LongNum u_num, LongNum v_num, LongNum& res) {
        // source: https://skanthak.hier-im-netz.de/division.html
        // divide a / b as integers (ignoring exp and sign), a >= b, len(b) >= 2

        const size_t m = u_num.limbs.size(), n = v_num.limbs.size();  // initial sizes
        const unsigned s = std::countl_zero(v_num.limbs.back());  // normalization (make first bit of divisor set to 1)
        u_num <<= s;
        v_num <<= s;
        std::vector<uint32_t>& u = u_num.limbs;
        if (u.size() == m) {
            u.push_back(0);
        }
        const std::vector<uint32_t>& v = v_num.limbs;

        res.limbs.resize(m - n + 1);
        std::vector<uint32_t>& q = res.limbs;
        for (size_t j = m - n; j != (size_t)-1; j--) {
            uint64_t qhat = (((uint64_t)u[j + n] << LongNum::base) | u[j + n - 1]) / v[n - 1];
            uint64_t rhat = (((uint64_t)u[j + n] << LongNum::base) | u[j + n - 1]) - qhat * v[n - 1];
            for (size_t i = 0; i < 2; i++) {
                if (qhat >= (1ull << LongNum::base) || qhat * v[n - 2] > ((rhat << LongNum::base) | u[j + n - 2])) {
                    qhat--;
                    rhat += v[n - 1];
                    if (rhat >= (1ull << LongNum::base)) {
                        break;
                    }
                } else {
                    break;
                }
            }
            int64_t t;
            uint64_t carry = 0;
            for (size_t i = 0; i < n; i++) {
                const uint64_t prod = qhat * v[i];
                t = (int64_t)(u[i + j] - carry - (prod & UINT32_MAX));
                u[i + j] = t;
                carry = (prod >> LongNum::base) - (t >> LongNum::base);
            }
            t = (int64_t)(u[j + n] - carry);
            u[j + n] = t;

            q[j] = qhat;
            if (t < 0) {  // if we subtracted too much, add back
                q[j]--;
                carry = 0;
                for (size_t i = 0; i < n; i++) {
                    t = (int64_t)(u[i + j] + v[i] + carry);
                    u[i + j] = t;
                    carry = t >> LongNum::base;
                }
                u[j + n] += carry;
            }
        }
    };

    if (rhs == 0) {
        throw std::invalid_argument("Division by zero");
    }
    if (lhs == 0) {
        return 0;
    }
    if (lhs.exp < rhs.exp) {
        lhs <<= 2 * rhs.exp - lhs.exp;
    } else {
        lhs <<= rhs.exp;
    }
    if (lhs.limbs.size() < rhs.limbs.size() ||
        (lhs.limbs.size() == rhs.limbs.size() && lhs.limbs.back() < rhs.limbs.back())) {
        return 0;
    }
    LongNum res;
    if (rhs.limbs.size() == 1) {
        div_one_digit(lhs, rhs, res);
    } else {
        long_div(lhs, rhs, res);
    }
    res.exp = std::max(lhs.exp, rhs.exp);
    res.is_negative = lhs.is_negative ^ rhs.is_negative;
    res.remove_leading_zeros();
    return res;
}

void LongNum::set_precision(const unsigned precision) {
    if (exp < precision) {
        *this <<= precision - exp;
    } else if (exp > precision) {
        *this >>= exp - precision;
    }
    exp = precision;
}

LongNum LongNum::with_precision(const unsigned precision) const {
    if (exp < precision) {
        LongNum res = *this << (precision - exp);
        res.exp = precision;
        return res;
    }
    if (exp > precision) {
        LongNum res = *this >> (exp - precision);
        res.exp = precision;
        return res;
    }
    return *this;
}

void LongNum::remove_leading_zeros() {
    while (!limbs.empty() && limbs.back() == 0) {
        limbs.pop_back();
    }
    if (limbs.empty()) {
        is_negative = false;
    }
}

LongNum LongNum::truncate() const {
    return this->with_precision(0);
}

LongNum LongNum::frac() const {
    return *this - this->truncate();
}

void LongNum::shrink_to_fit() {
    limbs.shrink_to_fit();
}

LongNum LongNum::pow(unsigned power) const {
    LongNum res = 1, a = *this;
    while (power) {
        if (power & 1)
            res *= a;
        a *= a;
        power >>= 1;
    }
    return res;
}

LongNum LongNum::sqrt() const {
    if (*this < 0) {
        throw std::invalid_argument("Square root of negative number is undefined");
    }
    LongNum x0 = 0, x1 = 1;
    while (x0 != x1) {
        x0 = x1;
        x1 += *this / x1;
        x1 >>= 1;
    }
    return x1;
}

std::string LongNum::to_binary_string() const {
	if (*this == 0) {
        return "0";
    }
    std::string res;
    if (is_negative) {
        res = "-";
    }
    for (uint32_t limb : limbs) {
        for (unsigned j = 0; j < base; j++) {
            res += std::to_string((limb >> j) & 1);
        }
    }
    while (res.back() == '0' && res.length() > exp + 1) {
        res.pop_back();
    }
    if (exp > 0) {
        res.insert(res.begin() + exp, '.');
    }
    std::ranges::reverse(res);
    return res;
}

LongNum LongNum::from_binary_string(std::string str) {
    if (str.empty()) {
        throw std::invalid_argument("Invalid string");
    }
    LongNum res;
    const size_t point_pos = str.find_first_of(".,");
    if (point_pos == std::string::npos) {
        res.exp = 0;
    } else {
        res.exp = str.size() - point_pos - 1;
        str.erase(str.begin() + point_pos);
    }
    if (str.find_first_of(".,") != std::string::npos) {
        throw std::invalid_argument("Invalid string");
    }
    for (const auto c: str) {
        if (c != '0' && c != '1') {
            throw std::invalid_argument("Invalid string");
        }
    }
    for (int i = (int)str.size() - 1; i >= 0; i -= base) {
        res.limbs.push_back(0);
        for (unsigned j = 0; j < std::min(base, (unsigned)i + 1); j++) {
            if (str[i - j] == '1') {
                res.limbs.back() |= (1 << j);
            }
        }
    }
    return res;
}

std::string LongNum::to_string(unsigned decimal_precision) const {
    const LongNum base = 10;  // TODO: make it work with any base
    std::string res;
    LongNum whole = this->truncate();
    while (whole != 0) {
        LongNum q = whole / base;
        LongNum r = whole - q * base;
        if (r == 0) {
            res += '0';
        } else {
            res += std::to_string(r.limbs.front());
        }
        whole = q;
    }
    if (res.empty()) {
        res += '0';
    }
    if (is_negative) {
        res += '-';
    }
    std::ranges::reverse(res);
    if (decimal_precision == 0) {
        return res;
    }
    LongNum frac = this->frac();
    if (frac == 0) {
        return res;
    }
    res += '.';
    unsigned cnt = 0;
    while (frac != 0 && cnt++ < decimal_precision) {
        frac *= base;
        LongNum r = frac.truncate();
        frac -= r;
        if (r == 0) {
            res += '0';
        } else {
            res += std::to_string(r.limbs.front());
        }
    }
    return res;
}

LongNum LongNum::from_string(std::string str, const std::optional<unsigned>& precision) {
    const LongNum base = 10;  // TODO: make it work with any base
    if (str.empty()) {
        throw std::invalid_argument("Invalid string");
    }
    LongNum res = 0;
    if (str.front() == '-') {
        str.erase(str.begin());
        res.is_negative = true;
    }
    if (str.front() == '+') {
        str.erase(str.begin());
    }
    const size_t point_pos = str.find_first_of(".,");
    unsigned decimal_exp;
    if (point_pos == std::string::npos) {
        decimal_exp = 0;
    } else {
        decimal_exp = str.size() - point_pos - 1;
        str.erase(str.begin() + point_pos);
    }
    if (str.find_first_of(".,") != std::string::npos) {
        throw std::invalid_argument("Invalid string");
    }
    for (const auto c: str) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("Invalid string");
        }
    }
    for (const auto digit: str) {
        res *= base;
        res += digit - '0';
    }
    res.set_precision(precision.value_or(decimal_exp * 4));
    res /= base.pow(decimal_exp);
    return res;
}

LongNum operator""_longnum(const long double number) {
    return LongNum::from_string(std::to_string(number));
}

LongNum operator""_longnum(const unsigned long long number) {
    return LongNum(number);
}

LongNum operator""_longnum(const char* number, std::size_t len) {
    return LongNum::from_string(std::string(number, len));
};

std::istream& operator>>(std::istream& stream, LongNum& number) {
    std::string str;
    stream >> str;
    number = LongNum::from_string(str);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const LongNum& number) {
    return stream << number.to_string();
}
