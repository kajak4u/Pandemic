#include "CCureMarker.hpp"
#include "CStringOption.h"
#include "CEnumOption.h"
#include "CBoard.hpp"
#include <QMouseEvent>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

CCureMarker::CCureMarker(QWidget * parent, DiseaseType _color)
    : CBoardItem(parent), status(NEW), color(_color), connectedIcon(nullptr)
{
    /*for (int i = 0; i < CureStatus_SL.size();++i)
        options += new CStringOption(CureStatus_SL[i]+" Disease's image src", images[(CureStatus)i]);*/
    options += new CEnumOption<DiseaseType>("Color", DiseaseType_SL, color);
    options += new CStringOption("Object name", name);
    connect(this, &CCureMarker::rightButtonUp, this, &CCureMarker::increaseStatus);
    animation = new QSequentialAnimationGroup(this);
}

CCureMarker::~CCureMarker() {
	
}

void CCureMarker::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    //for (int i = 0; i < CureStatus_SL.size(); ++i)
    //    images[(CureStatus)i] = ts.readLine();
    QString colorStr = ts.readLine();
    color = (DiseaseType)DiseaseType_SL.indexOf(colorStr);
    QString statusStr = ts.readLine();
    status = (CureStatus)CureStatus_SL.indexOf(statusStr);
    updateOptions();
}

void CCureMarker::increaseStatus(QMouseEvent*)
{
    setStatus((CureStatus)((status + 1) % CureStatus_SL.size()));
}

void CCureMarker::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << DiseaseType_SL[color] << endl;
    ts << CureStatus_SL[status] << endl;
}

CureStatus CCureMarker::getStatus() const
{
    return status;
}

void CCureMarker::setStatus(const CureStatus & s)
{
    if (s == status) {
        if(!images[s].isNull())
            setImage(images[s]);
    }
    else {
        if (animation->state() != QAbstractAnimation::Running)
            animation->clear();
        QPropertyAnimation *fadeOut = createPropertyAnimation(this, "angleY", 0, 90, images[status].isNull() ? 1 : 1000);
        connect(fadeOut, &QPropertyAnimation::finished, [this, s]() {
            setImage(images[s]);
        });
        animation->addAnimation(fadeOut);
        status = s;
        QPropertyAnimation *fadeIn = createPropertyAnimation(this, "angleY", 90, 0, images[s].isNull() ? 1 : 1000);
        animation->addAnimation(fadeIn);
        if(animation->state() != QAbstractAnimation::Running)
            animation->start();
    }
    if (connectedIcon != nullptr)
        connectedIcon->setPixmap(iconPath(status));
}

void CCureMarker::connectIcon(QLabel *label)
{
    connectedIcon = label;
}

QString CCureMarker::imagePath(CureStatus status) const
{
    switch (status) {
        case NEW:
            return "";
        case DISCOVERED:
            return "img/cure_" + DiseaseType_SL[color] + "_discovered.png";
        case TREATED:
            return "img/cure_treated.png";
        default:
            return "";
    }
}

QString CCureMarker::iconPath(CureStatus) const
{
    switch (status) {
    case NEW:
        return ":/icons/img/icons/cure_" + DiseaseType_SL[color].toLower() + "_new.png";
    case DISCOVERED:
        return ":/icons/img/icons/cure_" + DiseaseType_SL[color].toLower() + "_discovered.png";
    case TREATED:
        return ":/icons/img/icons/cure_treated.png";
    default:
        return "";
    }
}

//void CCureMarker::scale(double factor)
//{
//    zoomFactor *= factor;
//    move(standardPos*zoomFactor);
//    resize(standardSize*zoomFactor);
//    setPixmap(images[status].scaled(images[status].size()*zoomFactor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//    update();
//}

void CCureMarker::updateOptions()
{
    images[NEW] = imagePath(NEW);
    images[DISCOVERED] = imagePath(DISCOVERED);
    images[TREATED] = imagePath(TREATED);
    setStatus(status);
}
