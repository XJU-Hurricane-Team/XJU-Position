#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "caliEncWindow.h"
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QResizeEvent>

/**
 * @brief Construct the main window.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    setMinimumSize(756, 369);
    ui->setupUi(this);
    resize(771, 369);

    chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->graphGroup->hide();

    serialPort = new QSerialPort(this);
    serialPort->setTextModeEnabled(false);
    recordContent = nullptr;
    searchSerialPort();

    /* Limit the text edit only input number. */
    ui->editXPos->setValidator(
        new QDoubleValidator(-9999999, 9999999, 5, this));
    ui->editYPos->setValidator(
        new QDoubleValidator(-9999999, 9999999, 5, this));
    ui->editYawPos->setValidator(
        new QDoubleValidator(-9999999, 9999999, 5, this));
    ui->editSendFreq->setValidator(new QIntValidator(-9999, 9999, this));
    ui->drawTimeGap->setValidator(new QIntValidator(-9999, 9999, this));

    QImage image("./logo.png");
    ui->logoLable->setPixmap(QPixmap::fromImage(image));
    ui->logoLable->setScaledContents(true);

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialRead()));
    connect(ui->switchSerialButton, SIGNAL(clicked()), this,
            SLOT(on_switchSerialClicked()));
    connect(ui->refreshSerialButton, SIGNAL(clicked()), this,
            SLOT(searchSerialPort()));

    connect(ui->recordButton, SIGNAL(clicked()), this,
            SLOT(on_recordClicked()));
    connect(ui->setXPos, SIGNAL(clicked()), this, SLOT(on_setXClicked()));
    connect(ui->setYPos, SIGNAL(clicked()), this, SLOT(on_setYClicked()));
    connect(ui->setYaw, SIGNAL(clicked()), this, SLOT(on_setYawClicked()));
    connect(ui->setFreq, SIGNAL(clicked()), this, SLOT(on_setFreqClicked()));
    connect(ui->setZero, SIGNAL(clicked()), this, SLOT(on_setZeroClicked()));
    connect(ui->setCaliGyro, SIGNAL(clicked()), this,
            SLOT(on_setCaliGyroClicked()));
    connect(ui->setCaliEncoder, SIGNAL(clicked()), this,
            SLOT(on_setCaliEncoderClicked()));

    connect(ui->editXPos, SIGNAL(editingFinished()), this,
            SLOT(on_setXClicked()));
    connect(ui->editYPos, SIGNAL(editingFinished()), this,
            SLOT(on_setYClicked()));
    connect(ui->editYawPos, SIGNAL(editingFinished()), this,
            SLOT(on_setYawClicked()));
    connect(ui->editSendFreq, SIGNAL(editingFinished()), this,
            SLOT(on_setFreqClicked()));
    connect(ui->drawTimeGap, SIGNAL(editingFinished()), this,
            SLOT(editDrawTime()));

    connect(&drawTimer, SIGNAL(timeout()), this, SLOT(drawTimeHandler()));
    connect(ui->showGraph, SIGNAL(clicked()), this,
            SLOT(on_showGraphClicked()));

    connect(ui->drawXPos, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));
    connect(ui->drawYPos, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));
    connect(ui->drawYaw, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));
    connect(ui->drawPtich, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));
    connect(ui->drawRoll, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));
    connect(ui->drawYawGyro, SIGNAL(clicked()), this,
            SLOT(on_changeVisibleClicked()));

    connect(ui->pauseDraw, SIGNAL(clicked()), this,
            SLOT(on_pauseDrawClicked()));
}

/**
 * @brief Destruct the main window.
 */
MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->graphGroup->setGeometry(390, 60, QWidget::width() - 380,
                                QWidget::height() - 60);
    ui->chartView->setGeometry(10, 70, ui->graphGroup->width() - 20,
                               ui->graphGroup->height() - 80);

    /* Adjust the axis. */
    if (ui->showGraph->text() == "隐藏曲线") {
        QRectF chartSize = chart->geometry();

        qreal timeMin = axisTime->min();

        axisTime->setRange(timeMin, timeMin + chartSize.width() / 50);
        axisTime->setTickCount(chartSize.width() / 75);
        axisPos->setTickCount(chartSize.height() / 50);
        axisAngle->setTickCount(chartSize.height() / 50);
    }
}

/**
 * @brief Search the serial port and add to the select box.
 */
void MainWindow::searchSerialPort() {
    ui->serialBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->serialBox->addItem(info.portName());
    }
}

/**
 * @brief Parsing data by byte.
 */
void MainWindow::parseSerialByte(uint8_t byte) {
    static uint8_t recvCount = 0;
    static uint8_t dataIndex = 0;

    static uint8_t dataBuffer[24];

    switch (recvCount) {
        case 0: {
            if (byte == 0x0D) {
                recvCount++;
            } else {
                recvCount = 0;
            }
        } break;

        case 1: {
            if (byte == 0x0A) {
                dataIndex = 0;
                recvCount++;
            } else if (byte == 0x0D) {
            } else {
                recvCount = 0;
            }
        } break;

        case 2: {
            dataBuffer[dataIndex] = byte;
            dataIndex++;
            if (dataIndex >= 24) {
                dataIndex = 0;
                recvCount++;
            }
        } break;

        case 3: {
            if (byte == 0x0A) {
                recvCount++;
            } else {
                recvCount = 0;
            }
        } break;

        case 4: {
            if (byte == 0x0D) {
                memcpy(&recvData, &dataBuffer, sizeof(recvData));
                recvComplete = true;
            }
            recvCount = 0;

        } break;

        default: {
            recvCount = 0;
        } break;
    }
}

/**
 * @brief Read the serial port data and display in the screen.
 */
void MainWindow::serialRead() {
    QByteArray recvBuf = serialPort->readAll();

    for (int i = 0; i < recvBuf.length(); ++i) {
        parseSerialByte(recvBuf[i]);
    }

    if (!recvComplete) {
        return;
    }

    recvComplete = false;
    /* Display the data line. */
    ui->dispXPos->setText(QString::number((double)recvData.posX));
    ui->dispYPos->setText(QString::number((double)recvData.posY));
    ui->dispYaw->setText(QString::number((double)recvData.yaw));
    ui->dispPitch->setText(QString::number((double)recvData.pitch));
    ui->dispRoll->setText(QString::number((double)recvData.roll));
    ui->dispYawGyro->setText(QString::number((double)recvData.yawGyro));

    /* Calculate the report frequency. */
    static QDateTime lastTime;
    QDateTime nowTime;
    nowTime = QDateTime::currentDateTime();
    int timeGap = nowTime.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch();
    if (timeGap == 0) {
        return;
    }
    int freq = 1000 / timeGap;
    ui->dispFreq->setText("回传频率: " + QString::number(freq) + " Hz");
    lastTime = nowTime;

    /* If record, write to stream. */
    if (recordContent != nullptr) {
        (*recordContent) << nowTime.toString("mm:ss.zzz") << ","
                         << ui->dispXPos->text() << "," << ui->dispYPos->text()
                         << "," << ui->dispYaw->text() << ","
                         << ui->dispPitch->text() << "," << ui->dispRoll->text()
                         << "," << ui->dispYawGyro->text() << "\n";
    }
}

/**
 * @brief Connect the serial port.
 */
void MainWindow::on_switchSerialClicked() {
    serialPort->setPortName(ui->serialBox->currentText());
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);

    QPalette pa;

    if (ui->switchSerialButton->text() == "打开串口") {
        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->serialStatus->setText("已打开: " +
                                      ui->serialBox->currentText());
            ui->switchSerialButton->setText("关闭串口");
            ui->dataGroup->setEnabled(true);
            pa.setColor(QPalette::WindowText, Qt::green);
            ui->serialStatus->setPalette(pa);
            ui->refreshSerialButton->setDisabled(true);
            recvComplete = false;
        } else {
            ui->serialStatus->setText("打开: " + ui->serialBox->currentText() +
                                      "失败");
            pa.setColor(QPalette::WindowText, Qt::red);
            ui->serialStatus->setPalette(pa);
            ui->refreshSerialButton->setEnabled(true);
        }
    } else {
        serialPort->close();
        ui->switchSerialButton->setText("打开串口");
        ui->serialStatus->setText("未连接串口");

        pa.setColor(QPalette::WindowText, Qt::black);
        ui->serialStatus->setPalette(pa);

        ui->dataGroup->setDisabled(true);
        ui->refreshSerialButton->setEnabled(true);

        if (ui->recordButton->text() == "停止记录") {
            on_recordClicked();
        }

        if (ui->showGraph->text() == "隐藏曲线") {
            on_showGraphClicked();
        }
    }
}

static struct {
    char head[4]; /*!< Frame head */
    union {
        float fpData;    /*!< The byte of float data */
        int32_t intData; /*!< The byte of int data */
    } data;
} __attribute__((packed)) sendData;

/**
 * @brief Set X position.
 */
void MainWindow::on_setXClicked() {
    memcpy(sendData.head, SEND_PREFIX "X", sizeof(sendData.head));

    sendData.data.fpData = ui->editXPos->text().toFloat();

    serialPort->write((const char *)&sendData, sizeof(sendData));
}

/**
 * @brief Set Y position.
 */
void MainWindow::on_setYClicked() {
    memcpy(sendData.head, SEND_PREFIX "Y", sizeof(sendData.head));

    sendData.data.fpData = ui->editYPos->text().toFloat();
    serialPort->write((const char *)&sendData, sizeof(sendData));
}

/**
 * @brief Set yaw angle.
 */
void MainWindow::on_setYawClicked() {
    memcpy(sendData.head, SEND_PREFIX "J", sizeof(sendData.head));

    sendData.data.fpData = ui->editYawPos->text().toFloat();
    serialPort->write((const char *)&sendData, sizeof(sendData));
}

/**
 * @brief Set report frequency.
 */
void MainWindow::on_setFreqClicked() {
    int freq = ui->editSendFreq->text().toInt();

    if (freq < 1) {
        freq = 1;
    } else if (freq > 200) {
        freq = 200;
    }

    ui->editSendFreq->setText(QString::number(freq));

    memcpy(sendData.head, SEND_PREFIX "F", sizeof(sendData.head));

    sendData.data.intData = freq;
    serialPort->write((const char *)&sendData, sizeof(sendData));
}

/**
 * @brief Set to Zero.
 */
void MainWindow::on_setZeroClicked() {
    memcpy(sendData.head, SEND_PREFIX "0", sizeof(sendData.head));

    /* Only send 4 bytes. */
    serialPort->write("ACT0", sizeof(sendData.head));
}

/**
 * @brief Enter the gyroscope calibration mode.
 */
void MainWindow::on_setCaliGyroClicked() {
    memcpy(sendData.head, SEND_PREFIX "R", sizeof(sendData.head));
    /* Only send 4 bytes. */
    serialPort->write((const char *)&sendData, sizeof(sendData.head));

    QMessageBox::information(this, "提示",
                             "已进入陀螺仪校准模式，请打开维特智能上位机调试");
    on_switchSerialClicked();
}

/**
 * @brief Enter the encoder calibration mode.
 */
void MainWindow::on_setCaliEncoderClicked() {
    memcpy(sendData.head, SEND_PREFIX "E", sizeof(sendData.head));
    /* Only send 4 bytes. */
    serialPort->write((const char *)&sendData, sizeof(sendData.head));

    disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(serialRead()));
    CaliEncWindow *w = new CaliEncWindow(nullptr, this->serialPort);
    w->setWindowModality(Qt::ApplicationModal);
    w->show();
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialRead()));
}

/**
 * @brief Record and Save the data.
 */
void MainWindow::on_recordClicked() {
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
        (*recordContent) << "Time,X,Y,Yaw,Pitch,Roll,YawGyro\n";

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
 * @brief show the Graph.
 */
void MainWindow::on_showGraphClicked() {
    const QString label[6] = {"X", "Y", "Yaw", "Pitch", "Roll", "YawGyro"};

    if (ui->showGraph->text() == "显示曲线") {
        /* Adjust ui to adapt the graph. */
        ui->showGraph->setText("隐藏曲线");

        ui->dataGroup->resize(361, 551);
        ui->setGroup->setGeometry(10, 280, 341, 261);

        /* Set up the axis. */
        ui->graphGroup->setHidden(false);

        axisTime = new QValueAxis();
        axisTime->setRange(0, 10);
        chart->addAxis(axisTime, Qt::AlignBottom);

        axisPos = new QValueAxis();
        axisPos->setRange(-10, 10);
        chart->addAxis(axisPos, Qt::AlignLeft);

        axisAngle = new QValueAxis();
        axisAngle->setRange(-180, 180);
        chart->addAxis(axisAngle, Qt::AlignRight);

        for (int i = 0; i < 6; ++i) {
            series[i] = new QSplineSeries(this);
            chart->addSeries(series[i]);
            series[i]->setName(label[i]);
        }
        /* Change the visible. */
        on_changeVisibleClicked();

        /* x, y position value. */
        for (int i = 0; i < 2; ++i) {
            series[i]->attachAxis(axisTime);
            series[i]->attachAxis(axisPos);
        }

        /* yaw, pitch, roll, yawGyro value. */
        for (int i = 2; i < 6; ++i) {
            series[i]->attachAxis(axisTime);
            series[i]->attachAxis(axisAngle);
        }

        /* Set up the timer. */
        graphTime = 0.0f;
        drawTimer.setInterval(ui->drawTimeGap->text().toInt());
        drawTimer.start();

        /* Adjust the size. */
        setMinimumSize(1184, 635);

        minTimeIndex = 0;
    } else {
        setMinimumSize(771, 369);

        ui->showGraph->setText("显示曲线");

        ui->dataGroup->resize(721, 291);
        ui->setGroup->setGeometry(368, 8, 341, 261);
        ui->graphGroup->setHidden(true);

        chart->removeAxis(axisTime);
        delete axisTime;
        chart->removeAxis(axisPos);
        delete axisPos;
        chart->removeAxis(axisAngle);
        delete axisAngle;

        for (auto &serie : series) {
            chart->removeSeries(serie);
            delete serie;
        }

        drawTimer.stop();
    }
}

/**
 * @brief Draw the line in a period of time.
 */
void MainWindow::drawTimeHandler() {
    qreal minPos = -10.0f;
    qreal maxPos = 10.0f;

    series[0]->append(graphTime, ui->dispXPos->text().toFloat());
    series[1]->append(graphTime, ui->dispYPos->text().toFloat());
    series[2]->append(graphTime, ui->dispYaw->text().toFloat());
    series[3]->append(graphTime, ui->dispPitch->text().toFloat());
    series[4]->append(graphTime, ui->dispRoll->text().toFloat());
    series[5]->append(graphTime, ui->dispYawGyro->text().toFloat());

    graphTime += (float)drawTimer.interval() / 1000.0f;

    if (axisTime->max() <= graphTime) {
        if (ui->autoScroll->isChecked()) {
            axisTime->setRange(graphTime - 10, graphTime);
            minTimeIndex += 1;
        }
        if (ui->autoScale->isChecked()) {
            axisTime->setRange(axisTime->min(), graphTime);
        }
    }

    QPointF point;

    for (int i = minTimeIndex; i < series[0]->count(); ++i) {
        point = series[0]->at(i);
        if (point.y() > maxPos) {
            maxPos = point.y();
        }
        if (point.y() < minPos) {
            minPos = point.y();
        }

        point = series[1]->at(i);
        if (point.y() > maxPos) {
            maxPos = point.y();
        }
        if (point.y() < minPos) {
            minPos = point.y();
        }
    }
    axisPos->setRange(minPos, maxPos);
}

/**
 * @brief Edit the draw time gap.
 */
void MainWindow::editDrawTime() {
    int timeGap = ui->drawTimeGap->text().toInt();
    if (timeGap < 1) {
        timeGap = 1;
    } else if (timeGap > 1000) {
        timeGap = 1000;
    }

    ui->drawTimeGap->setText(QString::number(timeGap));
    drawTimer.setInterval(timeGap);
    if (timeGap < 50) {
        chart->setAnimationOptions(QChart::NoAnimation);
    } else {
        chart->setAnimationOptions(QChart::SeriesAnimations);
    }
}

/**
 * @Show or hide the draw line.
 */
void MainWindow::on_changeVisibleClicked() {
    series[0]->setVisible(ui->drawXPos->isChecked());
    series[1]->setVisible(ui->drawYPos->isChecked());
    series[2]->setVisible(ui->drawYaw->isChecked());
    series[3]->setVisible(ui->drawPtich->isChecked());
    series[4]->setVisible(ui->drawRoll->isChecked());
    series[5]->setVisible(ui->drawYawGyro->isChecked());
}

/**
 * @brief Pause or continue draw.
 */
void MainWindow::on_pauseDrawClicked() {
    if (ui->pauseDraw->text() == "暂停记录") {
        ui->pauseDraw->setText("继续记录");
        drawTimer.stop();
    } else {
        ui->pauseDraw->setText("暂停记录");
        drawTimer.start();
    }
}
