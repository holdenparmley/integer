#ifndef INTEGER_H_
#define INTEGER_H_

#include <string>
#include <cmath>
#include <iostream>
#include <vector>

class Integer {
    std::string digits;
    char sign;

 public:
    Integer() {  // Default constructor; integer is 0, with positive sign (is 0 positive?)
        digits = "0";
        sign = '+';
    }

    Integer(const std::string& digs) {
        if (digs[0] == '-') {
            sign = '-';
            digits = digs.substr(1);
        } else {
            sign = '+';
            digits = digs;
        }
        for (char digit : digits) {
            if (digit < '0' || digit > '9') throw std::invalid_argument("Invalid digit detected!");
        }
    }

    Integer(const cs19::Integer& oldInteger) {
        digits = oldInteger.digits;
        sign = oldInteger.sign;
    }

    Integer operator=(const cs19::Integer &that) {
        if (*this != that) {
            digits = that.digits;
            sign = that.sign;
        }
        return *this;
    }

    operator double() const {
        // Hopefully, this section is pretty self explanatory
        double result = 0;
        double tens = 1;
        for (int i = digits.size() - 1; i >= 0; i--) {
            result += (digits[i] - '0') * tens;
            tens *= 10;
            }
        if (sign == '-') result *= -1;
        return result;
    }

    operator std::string() const {
        std::string result;
        if (sign == '-') result += sign;
        result += digits;
        return result;
    }

    bool operator==(const Integer &that) const {
        return (digits == that.digits && sign == that.sign);
    }

    bool operator!=(const Integer &that) const {
        return !(*this == that);
    }

    bool operator<(const Integer &that) const {
        if (sign == '-' && that.sign == '+') {
            return true;
        } else if (sign == '+' && that.sign == '-') {
            return false;
        } else if (sign == '+' && that.sign == '+') {
            return lowerAbsVal(that);
        } else {
            return higherAbsVal(that);
        }
    }

    bool operator<=(const Integer &that) const {
        return (*this < that || *this == that);
    }

    bool operator>(const Integer &that) const {
        return !(*this <= that);
    }

    bool operator>=(const Integer &that) const {
        return !(*this < that);
    }

    Integer operator+(const Integer &that) const {
        return add(that);
    }

    Integer operator-(const Integer &that) const {
        return subtract(that);
    }

    // Approached multiplication like it was taught in elementary school
    // Multiply in pieces based on tens place
    // For example, 2 * 345 = (2 * 3 * 100) * (2 * 4 * 10) + (2 * 5 * 1)
    Integer operator*(const Integer &that) const {
        int zerosLengths = that.digits.size();
        Integer result = Integer{"0"};
        for (int i = 0; i < zerosLengths; i++) {
            int thatDigit = that.digits[zerosLengths - 1 - i] - '0';
            Integer temp = Integer{"0"};
            for (int i = 0; i < thatDigit; i++) {
                temp = temp + *this;
            }
            std::string extraZeros(i, '0');
            temp.digits = temp.digits + extraZeros;
            result = result + temp;
        }
        if (that.sign == '-') result = -result;
        return result;
    }

    Integer operator-() const {
        if (sign == '+') {
            return {'-' + digits};
        } else {
            return {digits};
        }
    }

    friend std::ostream &operator<<(std::ostream &out, const cs19::Integer &integer) {
        out << static_cast<std::string>(integer);
        return out;
    }

 private:
    // A bunch of helper functions, that make the public part of the code more streamlined
    std::string zeroFilled(size_t maxSize, std::string digs) const {
        return std::string(maxSize - digs.size(), '0') + digs;
    }

    bool lowerAbsVal(const Integer &that) const {
    // Returns true if the absolute value of this is lower than the absolute value of that
        if (digits.size() != that.digits.size()) return digits.size() < that.digits.size();
        for (int i = 0; i < static_cast<int>(digits.size()); i++) {
            if (digits[i] < that.digits[i]) return true;
            if (digits[i] > that.digits[i]) return false;
        }
        return false;
    }

    bool higherAbsVal(const Integer &that) const {
        // Returns true if the absolute value of this is higher than the absolute value of that
        if (digits.size() != that.digits.size()) return digits.size() > that.digits.size();
        for (int i = 0; i < static_cast<int>(digits.size()); i++) {
            if (digits[i] > that.digits[i]) return true;
            if (digits[i] < that.digits[i]) return false;
        }
        return false;
    }

    bool equalAbsVal(const Integer &that) const {
        // Returns true if the absolute value of this is equal to the absolute value of that
        if (digits.size() != that.digits.size()) return false;
        for (int i = 0; i < static_cast<int>(digits.size()); i++) {
            if (digits[i] != that.digits[i]) return false;
        }
        return true;
    }

    std::string absAdd(const Integer &that) const {
        std::size_t maxSize = std::max(digits.size(), that.digits.size());
        std::string leftSide = zeroFilled(maxSize, digits);
        std::string rightSide = zeroFilled(maxSize, that.digits);
        std::string result(maxSize + 1, '0');
        int carry = 0;
        for (int i = maxSize - 1; i >= 0; i--) {
            int sum = (leftSide[i] - '0') + (rightSide[i] - '0') + carry;
            result[i + 1] = (sum % 10) + '0';
            carry = sum / 10;
        }
        if (carry != 0) {
            result[0] = '1';
        } else {
            result = result.substr(1);
        }
        return result;
    }

    std::string absSubtract(const Integer &that) const {
        std::size_t maxSize = std::max(digits.size(), that.digits.size());
        std::string leftSide = zeroFilled(maxSize, digits);
        std::string rightSide = zeroFilled(maxSize, that.digits);
        std::string result(maxSize, '0');
        int carry = 0;
        for (int i = maxSize - 1; i >= 0; i--) {
            int diff = (leftSide[i] - '0') - (rightSide[i] - '0') + carry + 10;
            result[i] = (diff % 10) + '0';
            carry = -1 + diff / 10;
        }
        while (result[0] == '0'  && result.size() > 1) {
            result = result.substr(1);
        }
        return result;
    }

    // Addition and subtraction are easier if we take into account the sign, do simple
    // addition or subtraction (simple subtraction being subtraction where the number to
    // be subtracted is less than the other number), and append a sign back if needed

    Integer add(const Integer &that) const {
        if (sign == '+' && that.sign == '+') {
            return {absAdd(that)};
        } else if (sign == '-' && that.sign == '-') {
            return {"-" + absAdd(that)};
        } else if (sign == '+' && that.sign == '-') {
            if (higherAbsVal(that) || equalAbsVal(that)) {
                return {absSubtract(that)};
            } else {
                std::string result = "-";
                result += that.absSubtract(*this);
                return {result};
            }
        } else {
            return {that.subtract(Integer{digits})};
        }
    }

    Integer subtract(const Integer &that) const {
        if (sign == '+' && that.sign == '-') {
            return {absAdd(that)};
        } else if (sign == '-' && that.sign == '+') {
            return {"-" + absAdd(that)};
        } else if (sign == '+' && that.sign == '+') {
            if (higherAbsVal(that) || equalAbsVal(that)) {
                return {absSubtract(that)};
            } else {
                return {"-" + that.absSubtract(*this)};
            }
        } else {
            if (higherAbsVal(that)) {
                std::string result = "-";
                result += absSubtract(that);
                return {result};
            } else {
                return {that.absSubtract(*this)};
            }
        }
    }
};

#endif
