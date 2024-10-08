#include "mainwindow.h"
#include <QMessageBox>
#include <QPalette>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Main layout setup
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(15, 15, 15, 15);

    setStyleSheet("background-color: #f0f0f0;");

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

    // Clear button
    QPushButton *clearButton = new QPushButton("Clear", this);
    clearButton->setStyleSheet(
        "background-color: #f44336; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }"
        );
    layout->addWidget(clearButton);

    // Calculate button
    calculateButton = new QPushButton("Calculate Field Value", this);
    calculateButton->setStyleSheet(
        "background-color: #2196F3; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
        );
    layout->addWidget(calculateButton);

    // Result label
    resultLabel = new QLabel("Result: ", this);
    resultLabel->setStyleSheet("font-size: 14px; color: #333;");
    layout->addWidget(resultLabel);

    // Grid layout for bit number and value labels
    QGridLayout *bitLayout = new QGridLayout();
    layout->addLayout(bitLayout);

    // Grey color palette for bit number labels
    QPalette greyPalette;
    greyPalette.setColor(QPalette::WindowText, Qt::gray);

    // Setup bit number labels (31-16 in first line, 15-0 in third line)
    for (int i = 0; i < 32; i++) {
        bitNumberLabels[i] = new QLabel(QString::number(31 - i), this);
        bitNumberLabels[i]->setAlignment(Qt::AlignCenter);
        bitNumberLabels[i]->setPalette(greyPalette);
        bitNumberLabels[i]->setCursor(Qt::PointingHandCursor);
        bitNumberLabels[i]->setAttribute(Qt::WA_Hover, true);
        bitNumberLabels[i]->installEventFilter(this);

        if (i < 16) {
            bitLayout->addWidget(bitNumberLabels[i], 0, i);
        } else {
            bitLayout->addWidget(bitNumberLabels[i], 2, i - 16);
        }
    }

    // Setup bit value labels
    for (int i = 0; i < 32; i++) {
        bitLabels[i] = new QLabel("0", this);
        bitLabels[i]->setAlignment(Qt::AlignCenter);
        bitLabels[i]->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");

        if (i < 16) {
            bitLayout->addWidget(bitLabels[i], 1, i);
        } else {
            bitLayout->addWidget(bitLabels[i], 3, i - 16);
        }
    }

    // Connect buttons to slots
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::showBits);
    connect(inputField, &QLineEdit::returnPressed, this, &MainWindow::showBits);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearBits);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculateFieldValue); // Connect calculate button
}

void MainWindow::showBits()
{
    QString inputText = inputField->text();
    bool ok;
    unsigned int value;

    // Check input for hex or decimal
    if (inputText.startsWith("0x") || inputText.startsWith("0X")) {
        value = inputText.toUInt(&ok, 16);
    } else {
        value = inputText.toUInt(&ok, 10);
    }

    // Validate input
    if (!ok || value > 0xFFFFFFFF) {
        QMessageBox::critical(this, "Error", "Invalid input or out of 32-bit range.");
        return;
    }

    // Convert to 32-bit binary string
    QString binaryString = QString("%1").arg(value, 32, 2, QChar('0'));

    // Update bit labels and reset clicked bits
    clickedBits.clear(); // Clear previously clicked bits
    for (int i = 0; i < 32; i++) {
        bitLabels[i]->setText(QString(binaryString[i]));

        if (binaryString[i] == '1') {
            bitLabels[i]->setStyleSheet("background-color: green; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        } else {
            bitLabels[i]->setStyleSheet("background-color: none; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        }
    }
}

void MainWindow::clearBits()
{
    inputField->clear(); // Clear the input field
    resultLabel->setText("Result: "); // Clear the result label
    clickedBits.clear(); // Clear the clicked bits

    // Reset bit value labels and clicked bit styles
    for (int i = 0; i < 32; i++) {
        bitLabels[i]->setText("0"); // Reset bit value
        bitLabels[i]->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;"); // Reset style

        // Reset clicked bit number color
        bitNumberLabels[i]->setStyleSheet("color: gray;"); // Reset clicked bit number to gray
    }
}


void MainWindow::calculateFieldValue()
{
    if (clickedBits.isEmpty()) {
        resultLabel->setText("Result: No bits selected."); // Handle case with no clicked bits
        return;
    }

    // Calculate the decimal value of the selected bits
    int decimalValue = 0;
    QString bitString;
    bool ok; // Declare 'ok' here to check conversion success

    for (int index : clickedBits) {
        bitString.append(bitLabels[index]->text()); // Collect the bit values
    }

    decimalValue = bitString.toInt(&ok, 2); // Convert to decimal

    if (ok) {
        resultLabel->setText(QString("Result: %1 = %2").arg(bitString).arg(decimalValue));
    } else {
        resultLabel->setText("Result: Conversion error.");
    }
}

// Event filter to handle bit number clicks
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        for (int i = 0; i < 32; ++i) {
            if (obj == bitNumberLabels[i]) {
                // Toggle clicked bit's index
                if (clickedBits.contains(i)) {
                    clickedBits.removeAll(i); // Remove if already clicked
                    bitNumberLabels[i]->setStyleSheet("color: gray;"); // Reset color
                } else {
                    clickedBits.append(i); // Add clicked bit index
                    bitNumberLabels[i]->setStyleSheet("color: green;"); // Highlight clicked bit
                }
                break;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
