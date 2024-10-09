#include "mainwindow.h"
#include <QMessageBox>
#include <QPalette>
#include <QMouseEvent>
#include <QDebug> 
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

    QHBoxLayout *horizontalLayoutrow1 = new QHBoxLayout();
    QHBoxLayout *horizontalLayoutrow2 = new QHBoxLayout();

    // Submit button
    QPushButton *submitButton = new QPushButton("Show 32-bit Value", this);
    submitButton->setStyleSheet(
        "background-color: #4CAF50; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        );
    //layout->addWidget(submitButton);

    // Clear button
    QPushButton *clearButton = new QPushButton("Clear", this);
    clearButton->setStyleSheet(
        "background-color: #f44336; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }"
        );
    //layout->addWidget(clearButton);

    horizontalLayoutrow1->addWidget(submitButton);
    horizontalLayoutrow1->addWidget(clearButton);

    // Calculate button
    calculateButton = new QPushButton("Calculate Field Value", this);
    calculateButton->setStyleSheet(
        "background-color: #2196F3; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
        );
    //layout->addWidget(calculateButton);
	
	// Add a new button for clearing selected bits and the field result
    clearSelectedButton = new QPushButton("Clear Selected Bits and Result", this);
    clearSelectedButton->setStyleSheet(
        "background-color: #FF9800; color: white; border: none; padding: 10px 15px; border-radius: 5px; font-size: 14px;"
        "font-weight: bold; cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #F57C00; }"
    );
    //layout->addWidget(clearSelectedButton);

    horizontalLayoutrow2->addWidget(calculateButton);
    horizontalLayoutrow2->addWidget(clearSelectedButton);

    // Connect the new button to the slot
    connect(clearSelectedButton, &QPushButton::clicked, this, &MainWindow::clearSelectedBitsAndResult);

    // Grid layout for bit number and value labels
    QGridLayout *bitLayout = new QGridLayout();
    layout->addLayout(bitLayout);

	// Black color palette for bit number labels
    QPalette grayPalette;
    grayPalette.setColor(QPalette::WindowText, Qt::gray);
	
	// Bold font
	QFont boldFont;
	boldFont.setBold(true);  // Set the font to bold

    // Setup bit number labels (31-16 in first line, 15-0 in third line)
    for (int i = 0; i < 32; i++) {
        bitNumberLabels[31 - i] = new QLabel(QString::number(31 - i), this);
        bitNumberLabels[31 - i]->setAlignment(Qt::AlignCenter);
        bitNumberLabels[31 - i]->setPalette(grayPalette);
		bitNumberLabels[31 - i]->setFont(boldFont);        // Apply bold font
        bitNumberLabels[31 - i]->setCursor(Qt::PointingHandCursor);
        bitNumberLabels[31 - i]->setAttribute(Qt::WA_Hover, true);
        bitNumberLabels[31 - i]->installEventFilter(this);

        if (i < 16) {
            bitLayout->addWidget(bitNumberLabels[31 - i], 0, i);
        } else {
            bitLayout->addWidget(bitNumberLabels[31 - i], 2, i - 16);
        }
    }

    // Setup bit value labels
    for (int i = 0; i < 32; i++) {
        bitLabels[31 - i] = new QLabel("0", this);
        bitLabels[31 - i]->setAlignment(Qt::AlignCenter);
        bitLabels[31 - i]->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");

        if (i < 16) {
            bitLayout->addWidget(bitLabels[31 - i], 1, i);
        } else {
            bitLayout->addWidget(bitLabels[31 - i], 3, i - 16);
        }
    }

    // Result label
    resultLabel = new QLabel("Field Result: ", this);
    resultLabel->setStyleSheet(
        "font-size: 14px; "
        "color: #333; "
        "padding: 5px;"              // Optional padding inside the label
        );
    layout->addWidget(resultLabel);

    layout->addSpacing(20);

    layout->addLayout(horizontalLayoutrow1);
    // Reminder label
    patternFieldUseLabel = new QLabel("Field calculation:  Click bit numbers to select a field", this);
    patternFieldUseLabel->setAlignment(Qt::AlignLeft);
    patternFieldUseLabel->setStyleSheet("font-size: 14px; color: #333; padding: 5px;");
    layout->addWidget(patternFieldUseLabel);

    layout->addLayout(horizontalLayoutrow2);
    layout->addSpacing(20);

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

    clearSelectedBitsAndResult(); // fix the issue selected bitNumberLabels is not restore to gray

    for (int i = 0; i < 32; i++) {
        bitLabels[31 - i]->setText(QString(binaryString[i]));

        if (binaryString[i] == '1') {
            bitLabels[31 - i]->setStyleSheet("background-color: green; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        } else {
            bitLabels[31 - i]->setStyleSheet("background-color: none; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px;");
        }
    }
}

void MainWindow::clearBits()
{
    inputField->clear(); // Clear the input field
    resultLabel->setText("Field Result: "); // Clear the result label
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
        resultLabel->setText("Field Result: No bits selected."); // Handle case with no clicked bits
        return;
    }

    // Sort the clickedBits vector in ascending order
    std::sort(clickedBits.begin(), clickedBits.end());
	
	// Print the content of clickedBits
    qDebug() << "clickedBits content:" << clickedBits;
	
    // Calculate the decimal value of the selected bits
    int decimalValue = 0;
    QString bitString;
    bool ok; // Declare 'ok' here to check conversion success


    
    // Reverse the loop to collect the bit values from MSB to LSB
    for (int i = clickedBits.size() - 1; i >= 0; --i) {
        int index = clickedBits[i];
        bitString.append(bitLabels[index]->text()); // Collect the bit values
        // Print the content of the bitLabel for the clicked bit
        qDebug() << "Bit" << index << "content:" << bitLabels[index]->text();
    }
	
    decimalValue = bitString.toInt(&ok, 2); // Convert to decimal

    if (ok) {
        resultLabel->setText(QString("Field Result: %1 = %2").arg(bitString).arg(decimalValue));
    } else {
        resultLabel->setText("Field Result: Conversion error.");
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
                    bitNumberLabels[i]->setStyleSheet("background-color: none;"); // Reset background color
                } else {
                    clickedBits.append(i); // Add clicked bit index
                    bitNumberLabels[i]->setStyleSheet("background-color: yellow;"); // Highlight clicked bit
                }
                break;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}


void MainWindow::clearSelectedBitsAndResult()
{
    // Clear only the selected bits and reset the result label
    clickedBits.clear(); // Clear the clicked bits

    for (int i = 0; i < 32; i++) {
        // Reset the style of the clicked bit numbers to grey
        bitNumberLabels[i]->setStyleSheet("color: gray;");
    }

    resultLabel->setText("Field Result: "); // Clear the field calculation result
}
