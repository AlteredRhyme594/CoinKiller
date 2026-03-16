#include "quickpaintwidget.h"
#include "settingsmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

QuickPaintWidget::QuickPaintWidget(QWidget *parent, Level* level)
    : QWidget(parent)
{
    zoom = 1;

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    QHBoxLayout *presetLayout = new QHBoxLayout();
    layout->addLayout(presetLayout);
    presetLayout->setAlignment(Qt::AlignVCenter);

    presets = new QComboBox(this);
    presetLayout->addWidget(new QLabel(tr("Preset:")));
    presetLayout->addWidget(presets, 1);
    connect(presets, &QComboBox::currentIndexChanged, this, &QuickPaintWidget::handlePresetSelectionChange);

    saveButton = new QPushButton(tr("Save"), this);
    presetLayout->addWidget(saveButton);
    addButton = new QPushButton(tr("Add"), this);
    presetLayout->addWidget(addButton);
    removeButton = new QPushButton(tr("Remove"), this);
    presetLayout->addWidget(removeButton);

    QString basePath(SettingsManager::getInstance()->dataPath("icons/"));
    resetViewButton = new QPushButton(QIcon(basePath + "zoom100.png"), "", this);
    presetLayout->addWidget(resetViewButton);

    quickPaintView = new QuickPaintView(this, level);
    quickPaintViewArea = new QScrollArea(this);
    quickPaintViewArea->setWidget(quickPaintView);
    layout->addWidget(quickPaintViewArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    paintButton = new QPushButton(tr("Paint"), this);
    buttonLayout->addWidget(paintButton);
    eraseButton = new QPushButton(tr("Erase"), this);
    buttonLayout->addWidget(eraseButton);
}

void QuickPaintWidget::scrollTo(int x, int y)
{
    qMax(0.0f, x*zoom);
    qMax(0.0f, y*zoom);
    quickPaintViewArea->horizontalScrollBar()->setValue(x*zoom);
    quickPaintViewArea->verticalScrollBar()->setValue(y*zoom);
}


void QuickPaintWidget::loadPreset(QString name)
{

}


void QuickPaintWidget::savePreset(QString name)
{

}


void QuickPaintWidget::deletePreset(QString name)
{

}

void QuickPaintWidget::handlePresetSelectionChange(int index)
{
    QString preset = presets->itemData(index, Qt::DisplayRole).toString();
    if (preset.length() > 0)
        loadPreset(preset);
}


void QuickPaintWidget::handleSavePresetButtonClick()
{

}


void QuickPaintWidget::handleAddPresetButtonClick()
{

}


void QuickPaintWidget::handleDeletePresetButtonClick()
{

}


void QuickPaintWidget::handlePaintButtonClick()
{

}


void QuickPaintWidget::handleEraseButtonClick()
{

}
