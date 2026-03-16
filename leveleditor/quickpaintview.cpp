#include "quickpaintview.h"

#include <QPaintEvent>

QuickPaintView::QuickPaintView(QWidget *parent, Level *level)
    : QWidget(parent), level(level)
{
    zoom = 1.0f;
    clearAll();
    updateHints();

#ifdef USE_KDE_BLUR
    setBackgroundColor(QColor(0,0,0,0));
#endif
}


void QuickPaintView::paintEvent(QPaintEvent *evt)
{
    QPainter painter(this);
    paint(painter, evt->rect().adjusted(0, 0, 20, 20), zoom);
}


void QuickPaintView::paint(QPainter &painter, QRect rect, float zoomLvl)
{
    painter.scale(zoomLvl,zoomLvl);

    QRect drawrect = QRect(rect.x()/zoomLvl, rect.y()/zoomLvl, rect.width()/zoomLvl, rect.height()/zoomLvl);

    painter.fillRect(drawrect, backgroundColor);

    TileGrid tileGrid;
    tileGrid[0xFFFFFFFF] = 1;

    // Render Tiles
    TilesetObject middleObj = objects[ObjectPosition::MIDDLE];

    for (int i = 0; i < ObjectPosition::COUNT; i++)
    {
        TilesetObject obj = objects[i];
        QRect bounds = hints[i];
        QRect tileRect = getDrawTileRect(ObjectPosition(i));

        if (obj != UNDEF_OBJECT && !bounds.isNull())
        {
            if (!drawrect.intersects(tileRect))
                continue;

            quint16 tsid = obj.second & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->Render2DTiles(true);
                level->tilesets[tsid]->Render3DOverlay(true);
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj.first & 0xFFF, tileRect.x()/20, tileRect.y()/20, tileRect.width()/20, tileRect.height()/20, 1);
            }
            else
            {
                qDebug("attempt to draw obj (%04X, %04X) with non-existing tileset", obj.first, obj.second);
            }

            // draw middle tile to cover missing spots
            if (i != ObjectPosition::MIDDLE && bounds != tileRect) {
                quint16 tsid = middleObj.second & 0x3;
                if (level->tilesets[tsid])
                {
                    level->tilesets[tsid]->Render2DTiles(true);
                    level->tilesets[tsid]->Render3DOverlay(true);
                    level->tilesets[tsid]->drawObject(painter, tileGrid, middleObj.first & 0xFFF, bounds.x()/20, bounds.y()/20, bounds.width()/20, bounds.height()/20, 1);
                }
                else
                {
                    qDebug("attempt to draw middle tile obj (%04X, %04X) with non-existing tileset", middleObj.first, middleObj.second);
                }
            }
        }
    }

    {
        // Render the middle tile on the full area to cover missing spots
        TilesetObject obj = objects[ObjectPosition::MIDDLE];
        QRect bounds = QRect(hints[ObjectPosition::TOPLEFT].topLeft(), hints[ObjectPosition::BOTTOMRIGHT].bottomRight());

        if (obj != UNDEF_OBJECT && !bounds.isNull() && drawrect.intersects(bounds))
        {
            quint16 tsid = obj.second & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->Render2DTiles(true);
                level->tilesets[tsid]->Render3DOverlay(true);
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj.first & 0xFFF, bounds.x()/20, bounds.y()/20, bounds.width()/20, bounds.height()/20, 1);
            }
            else
            {
                qDebug("attempt to draw obj (%04X, %04X) with non-existing tileset", obj.first, obj.second);
            }
        }
    }

    // Render Hints
    painter.drawRects(hints, ObjectPosition::COUNT);

}


void QuickPaintView::updateHints()
{
    // int countDefined = 0;

    // get the size for middle object first to set the other hint positions based on it
    QSize middleSize = getMaxSizeForPosition(ObjectPosition::MIDDLE);
    QPoint center = rect().center();
    center.setX(center.x() - center.x() % 20);
    center.setY(center.y() - center.y() % 20);
    hints[ObjectPosition::MIDDLE] = QRect(center, middleSize);

    // if (objects[ObjectPosition::MIDDLE] != UNDEF_OBJECT && objectsDefs[ObjectPosition::MIDDLE] != nullptr)
    //     countDefined += 1;

    for (int i = 0; i < ObjectPosition::COUNT; i++)
    {
        if (i == ObjectPosition::MIDDLE)
            continue;

        if (i < ObjectPosition::TOPLEFTCORNER) {

            int locX = i % 3;
            int locY = i / 3;

            QSize size = getMaxSizeForPosition(ObjectPosition::MIDDLE);

            QPoint topLeft = QPoint(
                hints[ObjectPosition::MIDDLE].x() - size.width() * (locX == 0) + hints[ObjectPosition::MIDDLE].width() * (locX == 2),
                hints[ObjectPosition::MIDDLE].y() - size.height() * (locY == 0) + hints[ObjectPosition::MIDDLE].height() * (locY == 2)
            );

            hints[i] = QRect(topLeft, size);

        } else {

            // TODO corners

        }
    }

    update();
}


QSize QuickPaintView::getMaxSizeForPosition(ObjectPosition pos)
{
    quint8 width = 1;
    quint8 height = 1;

    if (pos <= ObjectPosition::BOTTOMRIGHT) {

        int x = pos % 3;
        int y = pos / 3;

        for (int i = 0; i < 3; i++) {
            auto defs = objectsDefs[i+(y*3)];
            if (defs != nullptr)
                height = qMax(height, defs->height);
        }

        for (int i = 0; i < 3; i++) {
            auto defs = objectsDefs[x+(i*3)];
            if (defs != nullptr)
                width = qMax(width, defs->width);
        }


    } else if (pos < ObjectPosition::COUNT) {
        auto defs = objectsDefs[pos];
        if (defs != nullptr) {
            width = defs->width;
            height = defs->height;
        }
    }

    return QSize(width*20, height*20);
}


QRect QuickPaintView::getDrawTileRect(ObjectPosition pos)
{
    QRect bounds = QRect(hints[pos]);
    auto defs = objectsDefs[pos];

    if (pos <= ObjectPosition::BOTTOMRIGHT && defs != nullptr) {
        int x = pos % 3;
        int y = pos / 3;

        if (x == 0) // left col
        {
            bounds.setWidth(defs->width * 20);
        }
        else if (x == 2) // right col
        {
            int diff = bounds.width() - (defs->width * 20);
            bounds.adjust(diff, 0, 0, 0);
        }

        if (y == 0) // top row
        {
            bounds.setHeight(defs->height * 20);
        }
        else if (y == 2) // bottom row
        {
            int diff = bounds.height() - (defs->height * 20);
            bounds.adjust(0, diff, 0, 0);
        }
    }

    return bounds;
}


void QuickPaintView::mousePressEvent(QMouseEvent *evt)
{
    QPoint pos = evt->pos();

    if (evt->buttons() & Qt::MiddleButton)
    {
        dragX = pos.x();
        dragY = pos.y();
    }

    if (drawType == ObjectType::BGDATOBJECT) {
        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton) {
            for (int i = 0; i < ObjectPosition::COUNT; i++)
            {
                QRect rect = hints[i];
                if (pos.x() > rect.left() && pos.x() < rect.right()
                    && pos.y() > rect.top() && pos.y() < rect.bottom()) {

                    if (evt->buttons() == Qt::LeftButton) {
                        if (selectedObject != UNDEF_OBJECT) {
                            setTilesetObject(ObjectPosition(i), selectedObject);
                            update();
                        }
                    } else {
                        clearTilesetObject(ObjectPosition(i));
                        update();
                    }

                    break;
                }
            }
        }
    }
}


void QuickPaintView::mouseMoveEvent(QMouseEvent *evt)
{
    if (evt->buttons() & Qt::MiddleButton)
    {
        int x = evt->position().x();
        int y = evt->position().y();

        emit scrollTo((visibleRegion().boundingRect().x() - x + dragX)/zoom, (visibleRegion().boundingRect().y() - y + dragY)/zoom);
        update();
    }
}

void QuickPaintView::resizeEvent(QResizeEvent *evt)
{
    QWidget::resizeEvent(evt);
    if (evt->oldSize() != evt->size())
        updateHints();
}


void QuickPaintView::setTilesetObject(ObjectPosition pos, TilesetObject object)
{
    objects[pos] = object;
    objectsDefs[pos] = getObjectDefs(object);
    updateHints();
}


void QuickPaintView::setTilesetObjects(TilesetObject *objects)
{
    for (int i = 0; i < ObjectPosition::COUNT; i++) {
        TilesetObject obj = objects[i];
        this->objects[i] = obj;
        this->objectsDefs[i] = getObjectDefs(obj);
    }
    updateHints();
}


void QuickPaintView::clearTilesetObject(ObjectPosition pos)
{
    objects[pos] = UNDEF_OBJECT;
    objectsDefs[pos] = nullptr;
    updateHints();
}


void QuickPaintView::clearAll()
{
    for (int i = 0; i < ObjectPosition::COUNT; i++) {
        objects[i] = UNDEF_OBJECT;
        objectsDefs[i] = nullptr;
    }
    updateHints();
}


ObjectDef *QuickPaintView::getObjectDefs(TilesetObject object)
{
    if (object != UNDEF_OBJECT) {
        return level->tilesets[object.second & 0x3]->getObjectDef(object.first & 0xFFF);
    }
    return nullptr;
}

