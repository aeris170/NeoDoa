#include <Utility/CheckSubstring.hpp>

#include <cassert>
#include <iostream>

#include <unicode/coll.h>
#include <unicode/unistr.h>
#include <unicode/stsearch.h>

bool CheckSubstring(const std::string_view text, const std::string_view query) {
    // Convert text and query to UnicodeString
    icu::UnicodeString uText(text.data());
    icu::UnicodeString uQuery(query.data());
    assert(!uText.isBogus());
    assert(!uQuery.isBogus());

    UErrorCode status = U_ZERO_ERROR;
    icu::RuleBasedCollator* collator = static_cast<icu::RuleBasedCollator*>(
        icu::Collator::createInstance(icu::Locale::getEnglish(), status)
        );
    //collator->setStrength(icu::Collator::ECollationStrength::PRIMARY);
    collator->setAttribute(UColAttribute::UCOL_NORMALIZATION_MODE, UColAttributeValue::UCOL_ON, status);

    icu::StringSearch search(uQuery, uText, collator, nullptr, status);

    int32_t matchStart = search.first(status);
    return matchStart != USEARCH_DONE;
}
bool CheckSubstringIgnoreCase(const std::string_view text, const std::string_view query) {
    // Convert text and query to UnicodeString
    icu::UnicodeString uText(text.data());
    icu::UnicodeString uQuery(query.data());
    assert(!uText.isBogus());
    assert(!uQuery.isBogus());

    UErrorCode status = U_ZERO_ERROR;
    icu::RuleBasedCollator* collator = static_cast<icu::RuleBasedCollator*>(
        icu::Collator::createInstance(icu::Locale::getEnglish(), status)
    );
    collator->setStrength(icu::Collator::ECollationStrength::PRIMARY);
    collator->setAttribute(UColAttribute::UCOL_NORMALIZATION_MODE, UColAttributeValue::UCOL_ON, status);

    icu::StringSearch search(uQuery, uText, collator, nullptr, status);

    int32_t matchStart = search.first(status);
    return matchStart != USEARCH_DONE;
}

int FindSubstringIndex(const std::string_view text, const std::string_view query) {
    // Convert text and query to UnicodeString
    icu::UnicodeString uText(text.data());
    icu::UnicodeString uQuery(query.data());
    assert(!uText.isBogus());
    assert(!uQuery.isBogus());

    UErrorCode status = U_ZERO_ERROR;
    icu::RuleBasedCollator* collator = static_cast<icu::RuleBasedCollator*>(
        icu::Collator::createInstance(icu::Locale::getEnglish(), status)
        );
    //collator->setStrength(icu::Collator::ECollationStrength::PRIMARY);
    collator->setAttribute(UColAttribute::UCOL_NORMALIZATION_MODE, UColAttributeValue::UCOL_ON, status);

    icu::StringSearch search(uQuery, uText, collator, nullptr, status);

    int matchStart = search.first(status);
    return matchStart;
}
int FindSubstringIndexIgnoreCase(const std::string_view text, const std::string_view query) {
    // Convert text and query to UnicodeString
    icu::UnicodeString uText(text.data());
    icu::UnicodeString uQuery(query.data());
    assert(!uText.isBogus());
    assert(!uQuery.isBogus());

    UErrorCode status = U_ZERO_ERROR;
    icu::RuleBasedCollator* collator = static_cast<icu::RuleBasedCollator*>(
        icu::Collator::createInstance(icu::Locale::getEnglish(), status)
        );
    collator->setStrength(icu::Collator::ECollationStrength::PRIMARY);
    collator->setAttribute(UColAttribute::UCOL_NORMALIZATION_MODE, UColAttributeValue::UCOL_ON, status);

    icu::StringSearch search(uQuery, uText, collator, nullptr, status);

    int matchStart = search.first(status);
    return matchStart;
}