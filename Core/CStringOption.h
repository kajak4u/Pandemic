#pragma once
#include "COption.h"
#include <QInputDialog>

class CStringOption : public COption {
    QString& val;
    const QString caption;
public:
    CStringOption(const QString& _caption, QString& _val)
        : caption(_caption), val(_val) {}
    QString getCurrent() {
        return val;
    }
    void changeByUser() {
        bool ok;
        QString typed = QInputDialog::getText(nullptr, caption, "Choose "+ caption+":", QLineEdit::Normal, val, &ok);
        if (ok)
            val = typed;
    }
    COption* clone() const {
        return new CStringOption(*this);
    }
};