#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    binding_button = ui->bindButton;
    manual_button = ui->manualZapButton;
    progress = ui->progressBar;
    status_label = ui->progressStatus;
    image = ui->image;
    adapterlist = ui->adapterList;
    media_player = new QMediaPlayer;
    loop_player = new QMediaPlayer;
    media_player->setVolume(100);
    media_player->setMedia(QUrl("qrc:/audio/noticed.wav"));
    loop_player->setVolume(50);
    loop_player->setMedia(QUrl("qrc:/audio/working.wav"));

    getAdapters();
    this->adapter = adapterlist->currentText();

    connect(binding_button, &QPushButton::released, this, &MainWindow::handleBindingButton);
    connect(manual_button, &QPushButton::released, this, &MainWindow::handleManualButton);
    connect(adapterlist, QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
        this->adapter = adapterlist->currentText();
#ifdef QT_DEBUG
        qDebug() << this->adapter;
#endif
    });

    kl = new key_listener([this](WPARAM _, DWORD vkCode) {
        if (this->bound_key == vkCode) {
            if (ONGOING)
                return;
            QMetaObject::invokeMethod(this, "triggerZap", Qt::QueuedConnection);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete kl;
    delete media_player;
    delete loop_player;
}

void MainWindow::getAdapters() {
    // https://docs.microsoft.comu/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses?redirectedfrom=MSDN
    DWORD dwRetVal = 0;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    outBufLen = 16000;
    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 3));
    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            adapterlist->addItem(QString::fromWCharArray(pCurrAddresses->FriendlyName));
            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %lu\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                                  | FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              dwRetVal,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPTSTR) &lpMsgBuf,
                              0,
                              NULL)) {
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    free(pAddresses);
            }
        }
    }
}

void MainWindow::handleBindingButton() {
    Binding_dialog* d = new Binding_dialog(this);
    d->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    if (d->exec() == QDialog::Accepted) {
        bound_key = d->getKey();
#ifdef QT_DEBUG
        qDebug() << (char) bound_key;
#endif
        char key_label = (char) bound_key;
        QString key_string_label = QChar::fromLatin1(key_label);
        binding_button->setText(key_string_label);
        d->close();
    }
}


void MainWindow::handleManualButton() {
    triggerZap();
}

void MainWindow::triggerZap()
{
    media_player->play();
    ONGOING = true;
    progress->setValue(50);
    status_label->setText("Shutting down " + adapter);
    delay(1);
    QProcess p;
    QStringList args;
    args << "interface"
         << "set"
         << "interface" << adapter << "disable";
    p.start("netsh", args);
    p.waitForFinished(-1);
    loop_player->play();
    connect(loop_player, &QMediaPlayer::mediaStatusChanged, loop_player, &QMediaPlayer::play);
    progress->setValue(75);
    status_label->setText("Waiting for 15 seconds before restarting...");
    delay(15);
    connect(loop_player, &QMediaPlayer::mediaStatusChanged, loop_player, &QMediaPlayer::stop);
    loop_player->stop();
    progress->setValue(100);
    status_label->setText("Starting " + adapter);
    QProcess pr;
    QStringList argsr;
    argsr << "interface"
          << "set"
          << "interface" << adapter << "enable";
    pr.start("netsh", argsr);
    pr.waitForFinished(-1);
    progress->setValue(0);
    status_label->setText("Inactive");
    ONGOING = false;
    media_player->play();
}
