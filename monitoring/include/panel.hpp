#ifndef __PANEL_HPP__
#define __PANEL_HPP__

#include <vector>
#include <map>
#include <iostream>

#include <QtWidgets/QWidget>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>

#include "utils.hpp"

using namespace std;

class Panel : public QWidget {
    public:
    Panel(QWidget* parent=nullptr);
    ~Panel();

    QCheckBox* getCheckBox();
    QSlider* getSlider();
    QComboBox* getComboBox(string);
    QSpinBox* getSpinBox(string);

    void updateMode(int);
    void resetPanel();

    // public slots:
    // void handleGenerateButton();
    // void handleResetButton();
    // void handleConnectButton();
    // void handleSaveButton();

    private:
    int mode;

    QVBoxLayout* panelLayout;
    QGridLayout* buttonLayout;
    QFormLayout* parameterLayout;

    map<string, QPushButton*> button;
    QCheckBox* check;
    QSlider* slider;
    map<string, QComboBox*> comboBox;
    map<string, QSpinBox*> spinBox;
    map<string, QLabel*> label;
};

#endif