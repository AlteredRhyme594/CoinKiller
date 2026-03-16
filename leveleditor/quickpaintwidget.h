#ifndef QUICKPAINTWIDGET_H
#define QUICKPAINTWIDGET_H

#include "level.h"
#include "leveleditor/quickpaintview.h"

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>


class QuickPaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuickPaintWidget(QWidget *parent, Level* level);

    void loadPreset(QString name);
    void savePreset(QString name);
    void deletePreset(QString name);

    QuickPaintView* quickPaintView;

public slots:
    void scrollTo(int x, int y);

private slots:
    void handlePresetSelectionChange(int index);
    void handleSavePresetButtonClick();
    void handleAddPresetButtonClick();
    void handleDeletePresetButtonClick();

    void handlePaintButtonClick();
    void handleEraseButtonClick();

private:
    QComboBox* presets;
    QPushButton* saveButton;
    QPushButton* addButton;
    QPushButton* removeButton;
    QPushButton* resetViewButton;

    QScrollArea* quickPaintViewArea;

    QPushButton* paintButton;
    QPushButton* eraseButton;

    float zoom;

signals:
    void presetChanged(TilesetObject*);
    void paintingModeSet();
    void erasingModeSet();
};

#endif // QUICKPAINTWIDGET_H
