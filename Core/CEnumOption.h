#pragma once
#include "COption.h"
#include <QInputDialog>

template<typename _T> class CEnumOption : public COption {
    _T& val;
    const QStringList strList;
    const QString caption;
public:
    CEnumOption(const QString& _caption, const QStringList& _strList, _T& _val)
        : strList(_strList), caption(_caption), val(_val) {}
    QString getCurrent() {
        return strList[(int)val];
    }
    void changeByUser() {
        bool ok;
        QString chosen_str = QInputDialog::getItem(nullptr, caption, "Choose "+caption+":", strList, (int)val, false, &ok);
        if (ok && !chosen_str.isEmpty()) {
            val = (_T)strList.indexOf(chosen_str);
        }
    }
    COption* clone() const {
        return new CEnumOption<_T>(*this);
    }
};