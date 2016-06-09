#pragma once
#include <QString>
class COption
{
public:
    COption() {}
    virtual ~COption() {}
    virtual QString getCurrent() = 0;
    virtual void changeByUser() = 0;
    virtual COption* clone() const = 0;
};

