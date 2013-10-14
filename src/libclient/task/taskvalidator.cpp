#include "taskvalidator.h"

TaskValidator::TaskValidator()
{
}

TaskValidator::~TaskValidator()
{
}

TaskValidator::ValidationResult TaskValidator::validate(const TestDefinitionPtr &testDefinition)
{
    return Valid;
}
