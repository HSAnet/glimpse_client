#include "snmp_definition.h"

SnmpDefinition::SnmpDefinition()
{

}

SnmpDefinition::~SnmpDefinition()
{

}

SnmpDefinitionPtr SnmpDefinition::fromVariant(const QVariant &variant)
{
    return SnmpDefinitionPtr(new SnmpDefinition());
}

QVariant SnmpDefinition::toVariant() const
{
    return QVariant();
}

