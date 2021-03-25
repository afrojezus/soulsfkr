#ifndef BINDING_DIALOG_H
#define BINDING_DIALOG_H
#include "key_listener.h"
#include <QDialog>

namespace Ui {
class Binding_dialog;
}

class Binding_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Binding_dialog(QWidget *parent = nullptr);
    ~Binding_dialog();
    DWORD getKey();
    void setKeyListener(key_listener* kl);

private:
    Ui::Binding_dialog *ui;
    key_listener* kl;
    DWORD key;
};

#endif // BINDING_DIALOG_H
