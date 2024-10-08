#include "mainwindow.h"
#include <QMessageBox>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Main layout setup
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10); // Spacing between elements
    layout->setContentsMargins(15, 15, 15, 15); // Margins around the layout

    setStyleSheet("background-color: #f0f0f0;"); // Light grey background

    // Reminder label
    patternReminderLabel = new QLabel("Reminder:\n- Hex: start with '0x' (e.g., 0x1F)\n- Decimal: just enter the number (e.g., 255)", this);
    patternReminderLabel->setAlignment(Qt::AlignLeft);
    patternReminderLabel->setStyleSheet("font-size: 12px; color: #333; padding: 5px;");
    layout->addWidget(patternReminderLabel);

    // Input label and field
    QLabel *inputLabel = new QLabel("Enter a Hex or Decimal value:", this);
    inputLabel->setStyleSheet("font-size: 14px; color: #333;");
    layout->addWidget(inputLabel);

    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("Enter a value...");
    inputField->setStyleSheet("padding: 10px; border-radius: 5px; border: 1px solid #ccc; font-size: 14px;");
    layout->addWidget(inputField);

    // Submit button
    QPushButton *submitButton = new QPushButton("Show 32-bit Value", this);
    submitButton->setStyleSheet(
        "background-color: #4CAF50; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        );
    layout->addWidget(submitButton);

    // Grid layout for bit number and value labels
    QGridLayout *bitLayout = new QGridLayout();
    layout->addLayout(bitLayout);

    // Grey color palette for bit number labels
    QPalette greyPalette;
    greyPalette.setColor(QPalette::WindowText, Qt::gray);

    // Setup bit number labels (31-16 in first line, 15-0 in third line)
    for (int i = 0; i < 32; i++) {
        bitNumberLabels[i] = new QLabel(QString::number(31 - i), this); // Bit number labels
        bitNumberLabels[i]->setAlignment(Qt::AlignCenter);
        bitNumberLabels[i]->setPalette(greyPalette);

        // Place bit number labels for bits 31-16 in first row, and bits 15-0 in third row
        if (i < 16) {
            bitLayout->addWidget(bitNumberLabels[i], 0, i); // First row for 31-16
        } else {
            bitLayout->addWidget(bitNumberLabels[i], 2, i - 16); // Third row for 15-0
        }
    }

    // Setup bit value labels (31-16 in second line, 15-0 in fourth line)
    for (int i = 0; i < 32; i++) {
        bitLabels[i] = new QLabel("0", this);  // Default bit value
        bitLabels[i]->setAlignment(Qt::AlignCenter);
        bitLabels[i]->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");

        // Place bit value labels for bits 31-16 in second row, and bits 15-0 in fourth row
        if (i < 16) {
            bitLayout->addWidget(bitLabels[i], 1, i); // Second row for 31-16 values
        } else {
            bitLayout->addWidget(bitLabels[i], 3, i - 16); // Fourth row for 15-0 values
        }
    }

    // Connect button to slot
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::showBits);
}

void MainWindow::showBits()
{
    QString inputText = inputField->text();
    bool ok;
    unsigned int value;

    // Check input for hex or decimal
    if (inputText.startsWith("0x") || inputText.startsWith("0X")) {
        value = inputText.toUInt(&ok, 16); // Hexadecimal
    } else {
        value = inputText.toUInt(&ok, 10); // Decimal
    }

    // Validate input
    if (!ok || value > 0xFFFFFFFF) {
        QMessageBox::critical(this, "Error", "Invalid input or out of 32-bit range.");
        return;
    }

    // Convert to 32-bit binary string
    QString binaryString = QString("%1").arg(value, 32, 2, QChar('0'));

    // Update bit labels
    for (int i = 0; i < 32; i++) {
        bitLabels[i]->setText(QString(binaryString[i])); // Update each bit value

        // Set green background if the bit is 1
        if (binaryString[i] == '1') {
            bitLabels[i]->setStyleSheet("background-color: green; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        } else {
            bitLabels[i]->setStyleSheet("background-color: none; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        }
    }
}
