#ifndef NEWEVENTTOOLBUTTON_H
#define NEWEVENTTOOLBUTTON_H

#include "event.h"
#include <QToolButton>

class NewEventToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit NewEventToolButton(QWidget *parent = nullptr);
    QString getSelectedEventType();
public slots:
    void newObject();
    void newWarp();
    void newHealLocation();
    void newTrigger();
    void newWeatherTrigger();
    void newSign();
    void newHiddenItem();
    void newSecretBase();
signals:
    void newEventAdded(QString);
private:
    QString selectedEventType;
    QAction *newObjectAction;
    QAction *newWarpAction;
    QAction *newHealLocationAction;
    QAction *newTriggerAction;
    QAction *newWeatherTriggerAction;
    QAction *newSignAction;
    QAction *newHiddenItemAction;
    QAction *newSecretBaseAction;
    void init();
};

#endif // NEWEVENTTOOLBUTTON_H
