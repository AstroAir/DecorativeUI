// tests/unit/test_validation_framework.cpp
#include <QtTest/QtTest>
#include <QString>
#include <memory>

#include "../../src/Core/Validation.hpp"

using namespace DeclarativeUI::Core::Validation;

class ValidationFrameworkTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // **Validation Result Tests**
    void testValidationResult();
    void testValidationMessage();
    void testValidationResultCombination();

    // **Basic Validators Tests**
    void testRequiredValidator();
    void testMinLengthValidator();
    void testMaxLengthValidator();
    void testRangeValidator();
    void testMinValidator();
    void testMaxValidator();
    void testPatternValidator();
    void testEmailValidator();
    void testUrlValidator();
    void testCustomValidator();

    // **Validation Chain Tests**
    void testValidationChainCreation();
    void testValidationChainMethods();
    void testValidationChainExecution();
    void testValidationChainStopOnError();

    // **Factory Function Tests**
    void testValidateFactory();

private:
};

void ValidationFrameworkTest::initTestCase() {
    // Setup test environment
}

void ValidationFrameworkTest::cleanupTestCase() {
    // Cleanup test environment
}

// **Validation Result Tests**
void ValidationFrameworkTest::testValidationResult() {
    // Test default constructor
    ValidationResult defaultResult;
    QVERIFY(defaultResult.isValid());
    QVERIFY(!defaultResult.hasErrors());
    QVERIFY(!defaultResult.hasWarnings());
    QCOMPARE(defaultResult.getMessages().size(), 0);

    // Test constructor with validity
    ValidationResult validResult(true);
    QVERIFY(validResult.isValid());

    ValidationResult invalidResult(false);
    QVERIFY(!invalidResult.isValid());
    QVERIFY(invalidResult.hasErrors());

    // Test constructor with messages
    std::vector<ValidationMessage> messages = {
        ValidationMessage{"Error message", ValidationSeverity::Error},
        ValidationMessage{"Warning message", ValidationSeverity::Warning}
    };
    ValidationResult resultWithMessages(false, std::move(messages));
    QVERIFY(!resultWithMessages.isValid());
    QVERIFY(resultWithMessages.hasErrors());
    QVERIFY(resultWithMessages.hasWarnings());
    QCOMPARE(resultWithMessages.getMessages().size(), 2);
}

void ValidationFrameworkTest::testValidationMessage() {
    // Test ValidationMessage construction
    ValidationMessage message("Test message", ValidationSeverity::Warning, "test_field");
    QCOMPARE(message.message, QString("Test message"));
    QCOMPARE(message.severity, ValidationSeverity::Warning);
    QCOMPARE(message.field_name, QString("test_field"));

    // Test default values
    ValidationMessage defaultMessage("Default test");
    QCOMPARE(defaultMessage.message, QString("Default test"));
    QCOMPARE(defaultMessage.severity, ValidationSeverity::Error);
    QVERIFY(defaultMessage.field_name.isEmpty());
}

void ValidationFrameworkTest::testValidationResultCombination() {
    ValidationResult result1(true);
    ValidationResult result2(false, {{ValidationMessage{"Error"}}});

    // Test += operator
    result1 += result2;
    QVERIFY(!result1.isValid());
    QCOMPARE(result1.getMessages().size(), 1);

    // Test + operator
    ValidationResult result3(true);
    ValidationResult result4(false, {{ValidationMessage{"Another error"}}});
    ValidationResult combined = result3 + result4;
    QVERIFY(!combined.isValid());
    QCOMPARE(combined.getMessages().size(), 1);
}

// **Basic Validators Tests**
void ValidationFrameworkTest::testRequiredValidator() {
    Validators::Required<QString> validator{"Field is required"};

    // Test valid string
    ValidationResult validResult = validator("Hello");
    QVERIFY(validResult.isValid());

    // Test empty string
    ValidationResult invalidResult = validator("");
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages().size(), 1);
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Field is required"));
}

void ValidationFrameworkTest::testMinLengthValidator() {
    Validators::MinLength<QString> validator(5, "Minimum 5 characters");

    // Test valid string
    ValidationResult validResult = validator("Hello World");
    QVERIFY(validResult.isValid());

    // Test short string
    ValidationResult invalidResult = validator("Hi");
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Minimum 5 characters"));

    // Test default message
    Validators::MinLength<QString> defaultValidator(3);
    ValidationResult defaultResult = defaultValidator("Hi");
    QVERIFY(!defaultResult.isValid());
    QVERIFY(defaultResult.getMessages()[0].message.contains("Minimum length is 3"));
}

void ValidationFrameworkTest::testMaxLengthValidator() {
    Validators::MaxLength<QString> validator(10, "Maximum 10 characters");

    // Test valid string
    ValidationResult validResult = validator("Hello");
    QVERIFY(validResult.isValid());

    // Test long string
    ValidationResult invalidResult = validator("This is a very long string");
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Maximum 10 characters"));
}

void ValidationFrameworkTest::testRangeValidator() {
    Validators::Range<int> validator(1, 10, "Value must be between 1 and 10");

    // Test valid value
    ValidationResult validResult = validator(5);
    QVERIFY(validResult.isValid());

    // Test value below range
    ValidationResult belowResult = validator(0);
    QVERIFY(!belowResult.isValid());

    // Test value above range
    ValidationResult aboveResult = validator(15);
    QVERIFY(!aboveResult.isValid());

    // Test default message
    Validators::Range<double> defaultValidator(0.0, 1.0);
    ValidationResult defaultResult = defaultValidator(2.0);
    QVERIFY(!defaultResult.isValid());
    QVERIFY(defaultResult.getMessages()[0].message.contains("between 0 and 1"));
}

void ValidationFrameworkTest::testMinValidator() {
    Validators::Min<int> validator(5, "Minimum value is 5");

    // Test valid value
    ValidationResult validResult = validator(10);
    QVERIFY(validResult.isValid());

    // Test equal value
    ValidationResult equalResult = validator(5);
    QVERIFY(equalResult.isValid());

    // Test invalid value
    ValidationResult invalidResult = validator(3);
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Minimum value is 5"));
}

void ValidationFrameworkTest::testMaxValidator() {
    Validators::Max<int> validator(100, "Maximum value is 100");

    // Test valid value
    ValidationResult validResult = validator(50);
    QVERIFY(validResult.isValid());

    // Test equal value
    ValidationResult equalResult = validator(100);
    QVERIFY(equalResult.isValid());

    // Test invalid value
    ValidationResult invalidResult = validator(150);
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Maximum value is 100"));
}

void ValidationFrameworkTest::testPatternValidator() {
    Validators::Pattern validator("^[A-Z][a-z]+$", "Must start with uppercase letter");

    // Test valid string
    ValidationResult validResult = validator("Hello");
    QVERIFY(validResult.isValid());

    // Test invalid string
    ValidationResult invalidResult = validator("hello");
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Must start with uppercase letter"));
}

void ValidationFrameworkTest::testEmailValidator() {
    Validators::Email validator{"Invalid email"};

    // Test valid emails
    ValidationResult validResult1 = validator("test@example.com");
    QVERIFY(validResult1.isValid());

    ValidationResult validResult2 = validator("user.name+tag@domain.co.uk");
    QVERIFY(validResult2.isValid());

    // Test invalid emails
    ValidationResult invalidResult1 = validator("invalid-email");
    QVERIFY(!invalidResult1.isValid());

    ValidationResult invalidResult2 = validator("@domain.com");
    QVERIFY(!invalidResult2.isValid());

    ValidationResult invalidResult3 = validator("user@");
    QVERIFY(!invalidResult3.isValid());
}

void ValidationFrameworkTest::testUrlValidator() {
    Validators::Url validator{"Invalid URL"};

    // Test valid URLs
    ValidationResult validResult1 = validator("https://www.example.com");
    QVERIFY(validResult1.isValid());

    ValidationResult validResult2 = validator("http://subdomain.domain.org/path?query=value");
    QVERIFY(validResult2.isValid());

    // Test invalid URLs
    ValidationResult invalidResult1 = validator("not-a-url");
    QVERIFY(!invalidResult1.isValid());

    ValidationResult invalidResult2 = validator("ftp://example.com");
    QVERIFY(!invalidResult2.isValid());
}

void ValidationFrameworkTest::testCustomValidator() {
    // Test custom validator with boolean return
    Validators::Custom validator([](const int& value) { return value % 2 == 0; }, "Must be even");

    ValidationResult validResult = validator(4);
    QVERIFY(validResult.isValid());

    ValidationResult invalidResult = validator(3);
    QVERIFY(!invalidResult.isValid());
    QCOMPARE(invalidResult.getMessages()[0].message, QString("Must be even"));

    // Test custom validator with ValidationResult return
    Validators::Custom validationResultValidator(
        [](const QString& value) -> ValidationResult {
            if (value.contains("test")) {
                return ValidationResult{true};
            }
            return ValidationResult{false, {{ValidationMessage{"Must contain 'test'"}}}};
        }
    );

    ValidationResult customValidResult = validationResultValidator("testing");
    QVERIFY(customValidResult.isValid());

    ValidationResult customInvalidResult = validationResultValidator("hello");
    QVERIFY(!customInvalidResult.isValid());
}

// **Validation Chain Tests**
void ValidationFrameworkTest::testValidationChainCreation() {
    ValidationChain<QString> chain;
    
    // Test that chain can be created
    QVERIFY(true); // Chain creation is tested by not crashing
}

void ValidationFrameworkTest::testValidationChainMethods() {
    auto chain = validate<QString>()
        .required("String is required")
        .minLength(3, "Minimum 3 characters")
        .maxLength(20, "Maximum 20 characters")
        .pattern("^[A-Za-z]+$", "Only letters allowed")
        .custom([](const QString& value) { return !value.contains("bad"); }, "Cannot contain 'bad'");

    // Test valid string
    ValidationResult validResult = chain.validate("Hello");
    QVERIFY(validResult.isValid());

    // Test invalid string (too short)
    ValidationResult shortResult = chain.validate("Hi");
    QVERIFY(!shortResult.isValid());

    // Test invalid string (contains numbers)
    ValidationResult numberResult = chain.validate("Hello123");
    QVERIFY(!numberResult.isValid());

    // Test invalid string (contains "bad")
    ValidationResult badResult = chain.validate("badword");
    QVERIFY(!badResult.isValid());
}

void ValidationFrameworkTest::testValidationChainExecution() {
    auto chain = validate<int>()
        .range(1, 100, "Must be between 1 and 100")
        .custom([](const int& value) { return value % 5 == 0; }, "Must be divisible by 5");

    // Test valid value
    ValidationResult validResult = chain.validate(25);
    QVERIFY(validResult.isValid());

    // Test invalid value (out of range)
    ValidationResult rangeResult = chain.validate(150);
    QVERIFY(!rangeResult.isValid());

    // Test invalid value (not divisible by 5)
    ValidationResult divisibleResult = chain.validate(23);
    QVERIFY(!divisibleResult.isValid());
}

void ValidationFrameworkTest::testValidationChainStopOnError() {
    auto chain = validate<QString>()
        .required("Required")
        .minLength(5, "Min 5 chars")
        .maxLength(10, "Max 10 chars")
        .stopOnFirstError(true);

    // Test that validation stops on first error
    ValidationResult result = chain.validate("");
    QVERIFY(!result.isValid());
    QCOMPARE(result.getMessages().size(), 1); // Should only have the "Required" error
    QCOMPARE(result.getMessages()[0].message, QString("Required"));

    // Test without stop on first error
    auto chainContinue = validate<QString>()
        .required("Required")
        .minLength(5, "Min 5 chars")
        .stopOnFirstError(false);

    ValidationResult continueResult = chainContinue.validate("");
    QVERIFY(!continueResult.isValid());
    // Should have both errors since string is empty (fails required and minLength)
    QCOMPARE(continueResult.getMessages().size(), 2);
}

// **Factory Function Tests**
void ValidationFrameworkTest::testValidateFactory() {
    // Test that factory function creates a validation chain
    auto stringChain = validate<QString>();
    auto intChain = validate<int>();
    auto doubleChain = validate<double>();

    // Test that chains can be used
    ValidationResult stringResult = stringChain.required().validate("test");
    QVERIFY(stringResult.isValid());

    ValidationResult intResult = intChain.range(1, 10).validate(5);
    QVERIFY(intResult.isValid());
}

QTEST_MAIN(ValidationFrameworkTest)
#include "test_validation_framework.moc"
