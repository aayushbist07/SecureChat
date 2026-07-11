#ifndef BUBBLEMESSAGE_H
#define BUBBLEMESSAGE_H

#include <QDialog>
#include <QString>
#include <QWidget>

namespace Ui {
class BubbleMessage;
}

class BubbleMessage : public QDialog
{
    Q_OBJECT

public:
    //so that it will run when the object is created
    explicit BubbleMessage(const QString &username , const QString &message , const QString &time , QWidget *parent = nullptr);

    ~BubbleMessage();

private:
    Ui::BubbleMessage *ui;
};

#endif // BUBBLEMESSAGE_H
