#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <tuple>
#include <string>
#include <functional>
#include <cstring>
#include <cassert>

using IntVector = std::vector<int>;
using BoolVector = std::vector<uint8_t>;
using BoolMatrix = std::vector<BoolVector>;

const int Undefined = std::numeric_limits<int>::max();
const int GuessStep = 5;
const int GapTolerance = 4;

template <typename T>
int getSize(const T& container) {
    return static_cast<int>(container.size());
}

template <typename T>
std::string containerToString(const T& container) {
    std::string result = "[";
    bool first = true;
    for (const auto& element : container) {
        if (!first) {
            result += ", ";
        }
        first = false;
        result += std::to_string(element);
    }
    return result + "]";
}

void minAssign(int& source, int target) {
    source = std::min(source, target);
}

std::string reversed(std::string text) {
    std::reverse(text.begin(), text.end());
    return text;
}

BoolMatrix getIsSubstrPalindrome(const std::string& text) {
    BoolMatrix isSubstrPalindrome(text.size(), BoolVector(text.size() + 1));

    for (int index = 0; index < getSize(text); ++index) {
        isSubstrPalindrome[index][1] = true;
    }
    for (int index = 0; index + 2 <= getSize(text); ++index) {
        isSubstrPalindrome[index][2] = text[index] == text[index + 1];
    }

    for (int length = 3; length <= getSize(text); ++length) {
        for (int index = 0; index + length <= getSize(text); ++index) {
            isSubstrPalindrome[index][length] = isSubstrPalindrome[index + 1][length - 2] &&
                                                text[index] == text[index + length - 1];
        }
    }

    return isSubstrPalindrome;
}

IntVector getSuffixPalindromeLengths(int prefixLength, const BoolMatrix& isSubstrPalindrome) {
    IntVector suffixPalindromeLengths;
    for (int length = prefixLength; length > 0; --length) {
        if (isSubstrPalindrome[prefixLength - length][length]) {
            suffixPalindromeLengths.push_back(length);
        }
    }
    return suffixPalindromeLengths;
}

void assertSingleTripleInvariant(const IntVector& palindromicLengths, int rightIndex) {
    if (rightIndex - 2 < 0) {
        return;
    }
    int first = palindromicLengths[rightIndex - 2];
    int second = palindromicLengths[rightIndex - 1];
    int third = palindromicLengths[rightIndex];
    if (first == Undefined || second == Undefined || third == Undefined) {
        return;
    }
    if (std::abs(first - third) > GapTolerance && std::abs(second - third) > GapTolerance) {
        throw std::logic_error("Invariant failed on " + containerToString(palindromicLengths) +
                               " at position " + std::to_string(rightIndex));
    }
}

void updateSinglePalindromicLength(
        int value, int index, IntVector& targetPalindromicLengths, bool withAssertions = true) {
    if (value != Undefined) {
        minAssign(targetPalindromicLengths[index], value + 1);
    }
    if (withAssertions) {
        for (int shift = 0; shift < 3 && index + shift < getSize(targetPalindromicLengths); ++shift) {
            assertSingleTripleInvariant(targetPalindromicLengths, index + shift);
        }
    }
}

void updatePalindromicLengthsWithSinglePalindrome(
        const std::string& extendedText, int leftBorder, int rightBorder,
        const IntVector& sourcePalindromicLengths, IntVector& targetPalindromicLengths) {
    while (leftBorder >= 0 && rightBorder < getSize(extendedText) &&
           extendedText[leftBorder] == extendedText[rightBorder]) {
        updateSinglePalindromicLength(sourcePalindromicLengths[leftBorder], rightBorder + 1,
                                      targetPalindromicLengths);
        --leftBorder;
        ++rightBorder;
    }
}

void updatePalindromicLengthsAfterExtension(
        const std::string& extendedText, int processedLength, const IntVector& suffixPalindromeLengths,
        const IntVector& sourcePalindromicLengths, IntVector& targetPalindromicLengths) {
    for (int suffixLength : suffixPalindromeLengths) {
        updatePalindromicLengthsWithSinglePalindrome(extendedText, processedLength - suffixLength, processedLength - 1,
                                                     sourcePalindromicLengths, targetPalindromicLengths);
    }
}

void updatePalindromicLengthsAfterAppend(
        const std::string& extendedText, int appendIndex,
        const IntVector& sourcePalindromicLengths, IntVector& targetPalindromicLengths) {
    updatePalindromicLengthsWithSinglePalindrome(extendedText, appendIndex, appendIndex,
                                                 sourcePalindromicLengths, targetPalindromicLengths);
    updatePalindromicLengthsWithSinglePalindrome(extendedText, appendIndex - 1, appendIndex,
                                                 sourcePalindromicLengths, targetPalindromicLengths);
}

void cutIncorrectPalindromicLengths(IntVector& palindromicLengths, int startIndex) {
    std::fill(palindromicLengths.begin() + startIndex, palindromicLengths.end(), Undefined);
}

std::pair<IntVector, IntVector> getOddAndEvenPalindromicLengthsInternal(const std::string& text) {
    auto isSubstrPalindrome = getIsSubstrPalindrome(text);

    IntVector oddPalindromicLengths(text.size() + 1, Undefined);
    IntVector evenPalindromicLengths(text.size() + 1, Undefined);
    evenPalindromicLengths[0] = 0;
    bool guessIsAllowed = false;
    int processedLength = 0;

    while (processedLength < getSize(text)) {
        auto suffixPalindromeLengths = getSuffixPalindromeLengths(processedLength, isSubstrPalindrome);

        std::string extension(1, text[processedLength]);
        if (guessIsAllowed) {
            int longestPalindromeLength = suffixPalindromeLengths[0];
            int step = std::min({GuessStep, getSize(text) - processedLength,
                                 processedLength - longestPalindromeLength});
            if (step > 0) {
                extension = reversed(text.substr(processedLength - longestPalindromeLength - step, step));
            }
        }
        auto extendedText = text.substr(0, processedLength) + extension;

        updatePalindromicLengthsAfterExtension(extendedText, processedLength, suffixPalindromeLengths,
                                               oddPalindromicLengths, evenPalindromicLengths);
        updatePalindromicLengthsAfterExtension(extendedText, processedLength, suffixPalindromeLengths,
                                               evenPalindromicLengths, oddPalindromicLengths);

        guessIsAllowed = true;
        for (char extensionChar : extension) {
            if (text[processedLength] != extensionChar) {
                guessIsAllowed = false;
                cutIncorrectPalindromicLengths(oddPalindromicLengths, processedLength);
                cutIncorrectPalindromicLengths(evenPalindromicLengths, processedLength);
                break;
            }
            updatePalindromicLengthsAfterAppend(extendedText, processedLength,
                                                oddPalindromicLengths, evenPalindromicLengths);
            updatePalindromicLengthsAfterAppend(extendedText, processedLength,
                                                evenPalindromicLengths, oddPalindromicLengths);
            ++processedLength;
        }
    }

    return {oddPalindromicLengths, evenPalindromicLengths};
}

std::pair<IntVector, IntVector> getOddAndEvenPalindromicLengths(const std::string& text) {
    try {
        return getOddAndEvenPalindromicLengthsInternal(text);
    } catch (...) {
        std::cerr << "An exception occured during processing of " << text << std::endl;
        throw;
    }
}

IntVector getPalindromicLengths(const std::string& text) {
    IntVector oddPalindromicLengths, evenPalindromicLengths;
    std::tie(oddPalindromicLengths, evenPalindromicLengths) = getOddAndEvenPalindromicLengths(text);
    IntVector palindromicLengths(text.size() + 1);
    for (int index = 0; index < getSize(palindromicLengths); ++index) {
        palindromicLengths[index] = std::min(oddPalindromicLengths[index],
                                             evenPalindromicLengths[index]);
    }
    return palindromicLengths;
}

std::pair<IntVector, IntVector> getOddAndEvenPalindromicLengthsNaively(const std::string& text) {
    auto isSubstrPalindrome = getIsSubstrPalindrome(text);

    IntVector oddPalindromicLengths(text.size() + 1, Undefined);
    IntVector evenPalindromicLengths(text.size() + 1, Undefined);
    evenPalindromicLengths[0] = 0;

    for (int length = 0; length < getSize(text); ++length) {
        for (int palindromeLength = 1; length + palindromeLength <= getSize(text); ++palindromeLength) {
            if (!isSubstrPalindrome[length][palindromeLength]) {
                continue;
            }
            updateSinglePalindromicLength(oddPalindromicLengths[length], length + palindromeLength,
                                          evenPalindromicLengths, false);
            updateSinglePalindromicLength(evenPalindromicLengths[length], length + palindromeLength,
                                          oddPalindromicLengths, false);
        }
    }

    return {oddPalindromicLengths, evenPalindromicLengths};
}

void selfTestCallback(const std::string& text) {
    IntVector fastOddPalindromicLengths, fastEvenPalindromicLengths;
    std::tie(fastOddPalindromicLengths, fastEvenPalindromicLengths) = getOddAndEvenPalindromicLengths(text);
    IntVector slowOddPalindromicLengths, slowEvenPalindromicLengths;
    std::tie(slowOddPalindromicLengths, slowEvenPalindromicLengths) = getOddAndEvenPalindromicLengthsNaively(text);
    for (int index = 0; index < getSize(text) + 1; ++index) {
        if (fastOddPalindromicLengths[index] != slowOddPalindromicLengths[index] ||
            fastEvenPalindromicLengths[index] != slowEvenPalindromicLengths[index]) {
            throw std::logic_error("Failed on " + text + " at position " + std::to_string(index));
        }
    }
}

void generateTexts(
        const std::string& text, int length, int alphabetSize,
        const std::function<void(const std::string&)>& callback) {
    if (getSize(text) == length) {
        callback(text);
        return;
    }
    for (char newChar = 'a'; newChar < 'a' + alphabetSize; ++newChar) {
        generateTexts(text + newChar, length, alphabetSize, callback);
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && std::strcmp(argv[1], "--self-test") == 0) {
        assert(argc == 4);
        int maximalLength = std::atoi(argv[2]);
        int alphabetSize = std::atoi(argv[3]);
        for (int length = 1; length <= maximalLength; ++length) {
            std::cout << "Performing a self-test with length = " << length << std::endl;
            generateTexts(std::string(), length, alphabetSize, &selfTestCallback);
        }
        std::cout << "All tests are done!" << std::endl;
    } else {
        std::string text;
        std::cin >> text;
        auto palindromicLengths = getPalindromicLengths(text);
        for (int index = 1; index < getSize(palindromicLengths); ++index) {
            std::cout << palindromicLengths[index] << std::endl;
        }
    }
}
