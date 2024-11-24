/**
 * @file    caliencoder.cpp
 * @author  Deadline039
 * @brief   Calibration Encoder Window
 * @version 0.1
 * @date    2024-10-25
 */

#include "caliEncWindow.h"
#include "ui_caliEncWindow.h"

#include <QByteArray>
#include <QFileDialog>
#include <QMessageBox>

/**
 * @brief Construct the Window.
 *
 * @param parent Parents
 * @param serialPort Serial Port
 */
CaliEncWindow::CaliEncWindow(QWidget *parent, QSerialPort *serialPort)
    : QWidget(parent), ui(new Ui::CaliEncWindow) {
    ui->setupUi(this);

    this->serialPort = serialPort;
    this->recordFile = nullptr;
    this->recordContent = nullptr;

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialRead()));
    connect(ui->recordButton, SIGNAL(clicked()), this,
            SLOT(on_recordClicked()));
    connect(ui->resetButton, SIGNAL(clicked()), SLOT(on_resetClicked()));
}

/**
 * @brief Destroy the Window.
 */
CaliEncWindow::~CaliEncWindow() {
    delete ui;
}

/**
 * @brief Read the serial port.
 */
void CaliEncWindow::serialRead() {
    QByteArray recvBuf = serialPort->readAll();
    qint32 encoder0, encoder1;

    if (sscanf(recvBuf, "%d,%d", &encoder0, &encoder1) != 2) {
        return;
    }

    ui->dispEncoder0->setText(QString::number(encoder0));
    ui->dispEncoder1->setText(QString::number(encoder1));

    if (recordContent != nullptr) {
        QDateTime nowTime;
        nowTime = QDateTime::currentDateTime();
        (*recordContent) << nowTime.toString("mm:ss.zzz") << ","
                         << ui->dispEncoder0->text() << ","
                         << ui->dispEncoder1->text() << "\n";
    }
}

/**
 * @brief Record button clicked
 */
void CaliEncWindow::on_recordClicked() {
    if (ui->recordButton->text() == "开始记录") {
        QFileDialog fileDialog;
        QString fileName = fileDialog.getSaveFileName(
            this, "Save File", "", "Comma-separated values (*.csv)");

        recordFile = new QFile(fileName);
        if (!recordFile->open((QIODevice::WriteOnly) | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "打开文件失败");
            delete recordFile;
            return;
        }
        recordContent = new QTextStream(recordFile);
        /* Write the table head. */
        (*recordContent) << "Time,Encoder0, Encoder1\n";

        ui->recordButton->setText("停止记录");
    } else {
        recordFile->close();
        delete recordFile;
        delete recordContent;
        recordContent = nullptr;
        ui->recordButton->setText("开始记录");
    }
}

/**
 * @brief Reset the encoder count.
 */
void CaliEncWindow::on_resetClicked() {
    serialPort->write("RESET", 6);
}