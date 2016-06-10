#include "CDiseaseCube.hpp"
#include "CCity.hpp"
#include "CStringOption.h"
#include "CEnumOption.h"
#include "CBoard.hpp"
#include <QMessageBox>
#include <QBitmap>
#include "CBoardItemsFactory.h"

QMap<DiseaseType, QPixmap> CDiseaseCube::pixmaps;

CDiseaseCube* CDiseaseCube::createIn(CCity* destination, DiseaseType color)
{
    CDiseaseCube* newCube = dynamic_cast<CDiseaseCube*>(CBoardItemsFactory().create("CDiseaseCube", destination->getZoom(), destination->pos()));
    newCube->color = color;
    newCube->updateOptions();
    newCube->setDestination(destination);
    newCube->stackUnder(destination);
    newCube->setWindowOpacity(0.25);
    return newCube;
}

void CDiseaseCube::disappear()
{
    CPoint endPos = getStandardMiddle();
    endPos.setY(-standardSize.height());
    QPropertyAnimation *anim = createPropertyAnimation(this, "middlePos", getStandardMiddle(), endPos, 1500, QEasingCurve::OutBounce);
    connect(anim, &QPropertyAnimation::finished, this, &CDiseaseCube::deleteLater);
    container->addAnimation(anim);
}

CDiseaseCube::CDiseaseCube(DiseaseType _color, CCity *_city) : CBoardItem((QWidget*)defaultContainer), color(_color), location(_city), nth(-1)
{
    disableResize();
    if (pixmaps.size() == 0)
    {
        pixmaps[BLACK] = QPixmap("img\\cube_black.png");
        pixmaps[BLUE] = QPixmap("img\\cube_blue.png");
        pixmaps[RED] = QPixmap("img\\cube_red.png");
        pixmaps[YELLOW] = QPixmap("img\\cube_yellow.png");
    }
    options += new CEnumOption<DiseaseType>("Cube color", DiseaseType_SL, color);
}

CDiseaseCube::~CDiseaseCube() {
}

DiseaseType CDiseaseCube::getColor() const
{
    return color;
}

void CDiseaseCube::updateOptions()
{
    const QPixmap& image = pixmapOf(color);
    setImage(image);
    setMask(image.createMaskFromColor(Qt::transparent));
    setToolTip(createToolTip());
}

QString CDiseaseCube::createToolTip()
{
    QString tooltip = CBoardItem::createToolTip();
    tooltip += "\nColor: " + DiseaseType_SL[color];
    if (location != nullptr)
        tooltip += "\nLocation: " + location->getName();
    tooltip += "\nnth: " + QString::number(nth);
    return tooltip;
}

void CDiseaseCube::loadFrom(QTextStream &is)
{
    CBoardItem::loadFrom(is);
    QString colorStr;
    colorStr = is.readLine();
    color = (DiseaseType)DiseaseType_SL.indexOf(colorStr);
    int whereId;
    is >> whereId;
    references[(CBoardItem**)&location] = whereId;
    is.readLine();
    updateOptions();
}

void CDiseaseCube::saveTo(QTextStream &os) const
{
    CBoardItem::saveTo(os);
    os << DiseaseType_SL[(int)color] << endl;
    os << (location ? location->getId() : -1) << endl;
}

void CDiseaseCube::setDestination(CCity * newLocation)
{
    location = newLocation;
    if (newLocation == nullptr)
        return;
    CPoint cityPoint = newLocation->getStandardPos();
    nth = newLocation->cubesOf(color);
    CPoint verticalOffset = CPoint(0, -20)*nth;
    CPoint diseaseOffset = offsetOf(color);
    CPoint endPos = cityPoint + verticalOffset + diseaseOffset + standardSize/2;
    setStandardMiddle(CPoint(endPos.x(), -100));
    CPoint stdm1 = getStandardMiddle();
    QPropertyAnimation *anim = createPropertyAnimation(this, "middlePos", getStandardMiddle(), endPos, 1500, QEasingCurve::OutBounce);
    container->addAnimation(anim);
}

bool CDiseaseCube::isFirst() const
{
    return nth == 1;
}

void CDiseaseCube::setAlpha(double newAlpha)
{
    alpha = newAlpha;

}

CPoint CDiseaseCube::offsetOf(DiseaseType type)
{
    switch (type) {
    case BLUE:
        return CPoint(0, 0);
    case YELLOW:
        return CPoint(18, 10);
    case BLACK:
        return CPoint(36, 20);
    case RED:
        return CPoint(54, 30);
    default:
        return CPoint();
    }
}
const QPixmap& CDiseaseCube::pixmapOf(DiseaseType type)
{
    return pixmaps[type];
}