#include "binding_dialog.h"
#include "ui_binding_dialog.h"

Binding_dialog::Binding_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Binding_dialog)
{
    ui->setupUi(this);
    kl = new key_listener([this](WPARAM _, DWORD vkCode) {
        if (vkCode) {
            this->key = vkCode;
            QMetaObject::invokeMethod(this, "accept", Qt::QueuedConnection);
        }
    });
}

Binding_dialog::~Binding_dialog()
{
    delete ui;
    delete kl;
}

DWORD Binding_dialog::getKey()
{
    return key;
}
