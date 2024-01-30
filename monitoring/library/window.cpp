#include "window.hpp"

Window::Window() {
    setWindowTitle("test window");
    param p = loadParameter();
    setFixedSize(p.screen_width + 2*p.screen_padding + 300,
        p.screen_height + 2*p.screen_padding + 20);

    // static view
    mode = menuBar()->addMenu("Mode");
    view = menuBar()->addMenu("View");

    vector<string> modeActionName {"Simulate", "A-Star", "Bezier"};
    for (int i = 0; i < (int)modeActionName.size(); i++) {
        modeAction.push_back(new QAction(modeActionName[i].c_str(), this));
        modeAction.back()->setCheckable(true);
        if (i == 0) modeAction.back()->setChecked(true);
        mode->addAction(modeAction.back());
    }

    vector<string> viewActionName {"Node", "Visited Node", "Robot Area", "A-Star Path", "Bezier Path"};
    for (int i = 0; i < (int)viewActionName.size(); i++) {
        viewAction.push_back(new QAction(viewActionName[i].c_str(), this));
        viewAction.back()->setCheckable(true);
        viewAction.back()->setChecked(true);
        view->addAction(viewAction.back());
    }

    // widget
    mainWidget = new QWidget(this);
    field = new Field(this);
    panel = new Panel(this);
    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    setCentralWidget(mainWidget);
    mainWidget->setLayout(mainLayout);
    mainLayout->addWidget(field);
    mainLayout->addWidget(panel);

    // connect
}
