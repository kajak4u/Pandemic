#pragma once
#include "COption.h"
#include <QInputDialog>

class CIntOption : public COption {
    int& val;
    int minVal;
    int maxVal;
    const QString caption;
public:
    CIntOption(const QString& _caption, int& _val, int minValue, int maxValue)
        : caption(_caption), val(_val), minVal(minValue), maxVal(maxValue) {}
    QString getCurrent() {
        return val;
    }
    void changeByUser() {
        bool ok;
        int typed = QInputDialog::getInt(nullptr, caption, "Choose " + caption + ":", val, minVal, maxVal, 1, &ok);
        if (ok)
            val = typed;
    }
    COption* clone() const {
        return new CIntOption(*this);
    }
};