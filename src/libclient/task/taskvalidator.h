#ifndef TASKVALIDATOR_H
#define TASKVALIDATOR_H

#include "task.h"

class CLIENT_API TaskValidator
{
    Q_DISABLE_COPY(TaskValidator)

public:
    TaskValidator();
    ~TaskValidator();

    enum ValidationResult
    {
        Invalid,
        Valid // TODO: Find more texts
    };

    ValidationResult validate(const TestDefinition &testDefinition);

protected:
    class Private;
    Private *d;
};

#endif // TASKVALIDATOR_H
