#ifndef QUICKPAINTVIEW_H
#define QUICKPAINTVIEW_H

#include "level.h"

#include <QWidget>


enum ObjectPosition
{
    TOPLEFT,
    TOP,
    TOPRIGHT,

    LEFT,
    MIDDLE,
    RIGHT,

    BOTTOMLEFT,
    BOTTOM,
    BOTTOMRIGHT,

    TOPLEFTCORNER,
    TOPRIGHTCORNER,
    BOTTOMLEFTCORNER,
    BOTTOMRIGHTCORNER,

    COUNT
};


typedef QPair<int, int> TilesetObject;  // selObject, selTileset
static constexpr TilesetObject UNDEF_OBJECT = TilesetObject {-1, -1};


class QuickPaintView : public QWidget
{
    Q_OBJECT
public:
    explicit QuickPaintView(QWidget *parent, Level *level);

    void setTilesetObject(ObjectPosition pos, TilesetObject object);
    void setTilesetObjects(TilesetObject *objects);
    void clearTilesetObject(ObjectPosition pos);
    void clearAll();

    void setZoom(float zoom) { this->zoom = zoom; setMinimumSize(96*20*zoom, 96*20*zoom); setMaximumSize(96*20*zoom, 96*20*zoom); updateHints(); }
    void setBackgroundColor(QColor bgcolor) {backgroundColor = bgcolor; update(); }

public slots:
    void onDrawTypeChanged(ObjectType drawType) { this->drawType = drawType; }
    void onSelectedBgdatObjectChanged(int selObject, int selTileset) { selectedObject = TilesetObject { selObject, selTileset }; }

signals:
    void scrollTo(int x, int y);
    void presetChanged(TilesetObject *objects);

protected:
    void paintEvent(QPaintEvent *evt) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *evt) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *evt) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *evt) Q_DECL_OVERRIDE;

private:
    void paint(QPainter& painter, QRect rect, float zoomLvl);

    ObjectDef *getObjectDefs(TilesetObject object);

    void updateHints();
    QSize getMaxSizeForPosition(ObjectPosition pos);
    QRect getDrawTileRect(ObjectPosition pos);

    Level* level;
    float zoom;

    int dragX;
    int dragY;

    QColor backgroundColor;

    TilesetObject objects[ObjectPosition::COUNT];
    ObjectDef *objectsDefs[ObjectPosition::COUNT];
    QRect hints[ObjectPosition::COUNT];

    ObjectType drawType;
    TilesetObject selectedObject = UNDEF_OBJECT;
};

#endif // QUICKPAINTVIEW_H
