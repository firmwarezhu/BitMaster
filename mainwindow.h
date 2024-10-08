#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showBits();

private:
    QLineEdit *inputField;
    QLabel *bitLabels[32];          // Array to hold 32 labels for bit values
    QLabel *bitNumberLabels[32];     // Array to hold 32 labels for bit numbers (grey text)
    QLabel *patternReminderLabel;    // Label to show the reminder for hex/decimal patterns
};

#endif // MAINWINDOW_H
