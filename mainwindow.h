#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QEvent>
#include <QPalette>
#include <QVector>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showBits();
    void clearBits();
    void calculateFieldValue(); // New slot for calculating field value
	void clearSelectedBitsAndResult();

private:
    QLabel *patternReminderLabel;
    QLabel *patternFieldUseLabel;
    QLineEdit *inputField;
    QLabel *bitLabels[32];
    QLabel *bitNumberLabels[32];
    QPushButton *calculateButton; // New button for calculating
    QLabel *resultLabel; // New label for displaying the result
    QVector<int> clickedBits; // Store the clicked bit indices

    bool eventFilter(QObject *obj, QEvent *event) override;
	
	// Declare new clear button
    QPushButton *clearSelectedButton; // Button to clear selected bits and result
};

#endif // MAINWINDOW_H
