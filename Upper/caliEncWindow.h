/**
 * @file    caliencoder.h
 * @author  Deadline039
 * @brief   Calibration Encoder Window
 * @version 0.1
 * @date    2024-10-25
 */

#ifndef UPPER_CALIENCWINDOW_H
#define UPPER_CALIENCWINDOW_H

#include <QFile>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <stdint.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class CaliEncWindow;
}
QT_END_NAMESPACE

class CaliEncWindow : public QWidget {
    Q_OBJECT

  public slots:
    void serialRead();
    void on_recordClicked();
    void on_resetClicked();

  public:
    explicit CaliEncWindow(QWidget *parent, QSerialPort *serialPort);
    ~CaliEncWindow() override;

  private:
    Ui::CaliEncWindow *ui;
    QSerialPort *serialPort;
    QFile *recordFile;
    QTextStream *recordContent;
};

#endif /* UPPER_CALIENCWINDOW_H */
