#include "taskvalidator.h"

TaskValidator::TaskValidator()
{
}

TaskValidator::~TaskValidator()
{
}

TaskValidator::ValidationResult TaskValidator::validate(const TestDefinitionPtr &testDefinition)
{
    // Simple checks
    if (testDefinition.isNull()
        || testDefinition->id().isNull()
        || testDefinition->timing().isNull())
    {
        return Invalid;
    }

    // TODO: Signature checks etc.

    return Valid;
}
