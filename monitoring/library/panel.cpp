#include "panel.hpp"

Panel::Panel(QWidget* parent) {
    setParent(parent);

    param p = loadParameter();

    mode = 0;

    panelLayout = new QVBoxLayout(this);
    panelLayout->setContentsMargins(10, 10, 10, 10);
    panelLayout->setAlignment(Qt::AlignTop);

    buttonLayout = new QGridLayout();
    panelLayout->addLayout(buttonLayout);

    vector<string> buttonKey{"generate", "reset", "connect", "save"};
    vector<string> buttonName{"Generate", "Reset", "Connect", "Save"};
    for (int i = 0; i < (int)buttonName.size(); i++) {
        button[buttonKey[i]] = new QPushButton(buttonName[i].c_str());
        button[buttonKey[i]]->setFixedWidth(130);
        buttonLayout->addWidget(button[buttonKey[i]], i/2, i%2, Qt::AlignCenter);
    }

    check = new QCheckBox("Static");
    panelLayout->addWidget(check);
    check->setCheckState(Qt::Checked);

    vector<string> labelKey{"slider", "path", "heuristic", "node", "radius", "curvature"};
    vector<string> labelName{"Bezier (t):", "Path:", "Heuristic:", "Node Distance:", "Robot Radius:", "Bezier Curvature:"};
    for (int i = 0; i < (int)labelName.size(); i++) {
        label[labelKey[i]] = new QLabel(labelName[i].c_str());
    }
    vector<string> comboKey{"path", "heuristic"};
    for (int i = 0; i < 2; i++) {
        comboBox[comboKey[i]] = new QComboBox();
    }
    vector<string> spinKey{"node", "radius", "curvature"};
    for (int i = 0; i < 3; i++) {
        spinBox[spinKey[i]] = new QSpinBox();
    }

    slider = new QSlider(Qt::Horizontal);
    panelLayout->addWidget(label["slider"]);
    panelLayout->addWidget(slider);
    slider->setMinimum(0);
    slider->setMaximum(100);

    parameterLayout = new QFormLayout();
    panelLayout->addLayout(parameterLayout);
    parameterLayout->addRow(label["path"], comboBox["path"]);
    parameterLayout->addRow(label["heuristic"], comboBox["heuristic"]);
    parameterLayout->addRow(label["node"], spinBox["node"]);
    parameterLayout->addRow(label["radius"], spinBox["radius"]);
    parameterLayout->addRow(label["curvature"], spinBox["curvature"]);

    comboBox["path"]->addItems(QStringList{"1", "2", "3", "4", "5"});
    comboBox["path"]->setCurrentIndex(p.path_number);
    comboBox["heuristic"]->addItems(QStringList{"Manhattan", "Chebyshev", "Octile", "Euclidean"});
    comboBox["heuristic"]->setCurrentIndex(p.heuristic_type);

    spinBox["node"]->setMinimum(10);
    spinBox["node"]->setMaximum(200);
    spinBox["node"]->setValue(p.node_distance);
    spinBox["radius"]->setMinimum(10);
    spinBox["radius"]->setMaximum(100);
    spinBox["radius"]->setValue(p.robot_radius/2);
    spinBox["curvature"]->setMinimum(1);
    spinBox["curvature"]->setMaximum(10);
    spinBox["curvature"]->setValue(p.bezier_curvature);

    updateMode(mode);
}

Panel::~Panel() {
    // delete panelLayout;
    // delete buttonLayout;
    // delete parameterLayout;

    // for (auto it = button.begin(); it != button.end(); it++) {
    //     delete it->second;
    // }
    // delete check;
    // delete slider;
    // for (auto it = comboBox.begin(); it != comboBox.end(); it++) {
    //     delete it->second;
    // }
    // for (auto it = spinBox.begin(); it != spinBox.end(); it++) {
    //     delete it->second;
    // }
    // for (auto it = label.begin(); it != label.end(); it++) {
    //     delete it->second;
    // }
}

QCheckBox* Panel::getCheckBox() {
    return check;
}

QSlider* Panel::getSlider() {
    return slider;
}

QComboBox* Panel::getComboBox(string key) {
    return comboBox[key];
}

QSpinBox* Panel::getSpinBox(string key) {
    return spinBox[key];
}

void Panel::updateMode(int mode) {
    this->mode = mode;
    resetPanel();
    switch (mode) {
        case 0: // mode Simulate 
        {
            vector<string> buttonKey{"generate", "reset", "connect", "save"};
            vector<string> buttonName {"Generate", "Reset", "Connect", "Start"};
            for (int i = 0; i < (int)buttonName.size(); i++) {
                button[buttonKey[i]]->setText(buttonName[i].c_str());
            }
            label["slider"]->setVisible(false);
            slider->setVisible(false);
            // button["button3"]->setEnabled(false);
            break;
        }
        case 1: // mode A-Star
        {
            // vector<string> buttonKey{"generate", "reset"};
            // vector<string> buttonName {"Next", "Reset"};
            // for (int i = 0; i < (int)buttonName.size(); i++) {
            //     button[buttonKey[i]]->setText(buttonName[i].c_str());
            // }
            // button["connect"]->setVisible(false);
            // button["save"]->setVisible(false);
            // check->setVisible(false);
            // label["slider"]->setVisible(false);
            // slider->setVisible(false);
            // label["path"]->setVisible(false);
            // comboBox["path"]->setVisible(false);
            // label["radius"]->setVisible(false);
            // spinBox["radius"]->setVisible(false);
            // label["curvature"]->setVisible(false);
            // spinBox["curvature"]->setVisible(false);
            break;
        }
        case 2: // mode Bezier
        {
            // vector<string> buttonKey{"generate"};
            // vector<string> buttonName {"Generate"};
            // for (int i = 0; i < (int)buttonName.size(); i++) {
            //     button[buttonKey[i]]->setText(buttonName[i].c_str());
            // }
            // button["reset"]->setVisible(false);
            // button["connect"]->setVisible(false);
            // button["save"]->setVisible(false);
            // check->setVisible(false);
            // label["path"]->setVisible(false);
            // comboBox["path"]->setVisible(false);
            // label["radius"]->setVisible(false);
            // spinBox["radius"]->setVisible(false);
            // label["heuristic"]->setVisible(false);
            // spinBox["heuristic"]->setVisible(false);
            break;
        }
    }
}

void Panel::resetPanel() {
    for (auto it = button.begin(); it != button.end(); it++) {
        it->second->setVisible(true);
        it->second->setEnabled(true);
    }
    check->setVisible(true);
    slider->setVisible(true);
    slider->setEnabled(true);
    for (auto it = comboBox.begin(); it != comboBox.end(); it++) {
        it->second->setVisible(true);
        it->second->setEnabled(true);
    }
    for (auto it = spinBox.begin(); it != spinBox.end(); it++) {
        it->second->setVisible(true);
        it->second->setEnabled(true);
    }
    for (auto it = label.begin(); it != label.end(); it++) {
        it->second->setVisible(true);
    }

    param p = loadParameter();
    comboBox["path"]->setCurrentIndex(p.path_number);
    comboBox["heuristic"]->setCurrentIndex(p.heuristic_type);
    spinBox["node"]->setValue(p.node_distance);
    spinBox["radius"]->setValue(p.robot_radius/2);
    spinBox["curvature"]->setValue(p.bezier_curvature);
}