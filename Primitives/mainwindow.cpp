#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("lab1-2");
    auto scissorsTest = [=]() {
        int x = ui->xSlider->value();
        int y = ui->ySlider->value();
        int w = ui->widthSlider->value();
        int h = ui->heightSlider->value();
        ui->openGLWidget->changeScissorsTest(x, y, w, h);
    };
    auto alphaTest = [=]() {
        std::string func = ui->alphaTestComboBox->currentText().toStdString();
        int ref = ui->alphaTestSlider->value();
        ui->openGLWidget->changeAlphaTest(func, ref);
    };
    auto blendTest = [=]() {
        std::string sfactor = ui->sFactorComboBox->currentText().toStdString();
        std::string dfactor = ui->dFactorComboBox->currentText().toStdString();
        ui->openGLWidget->changeBlendTest(sfactor, dfactor);
    };
    connect(ui->primitivesComboBox, &QComboBox::currentTextChanged, ui->openGLWidget,
        [=]{
            ui->openGLWidget->changePrimitive(ui->primitivesComboBox->currentText().toStdString());
        }
    );
    connect(ui->xSlider, &QSlider::valueChanged, ui->openGLWidget, scissorsTest);
    connect(ui->ySlider, &QSlider::valueChanged, ui->openGLWidget, scissorsTest);
    connect(ui->widthSlider, &QSlider::valueChanged, ui->openGLWidget,scissorsTest);
    connect(ui->heightSlider, &QSlider::valueChanged, ui->openGLWidget, scissorsTest);
    connect(ui->alphaTestComboBox, &QComboBox::currentTextChanged, ui->openGLWidget, alphaTest);
    connect(ui->alphaTestSlider, &QSlider::valueChanged, ui->openGLWidget, alphaTest);
    connect(ui->sFactorComboBox, &QComboBox::currentTextChanged, ui->openGLWidget, blendTest);
    connect(ui->dFactorComboBox, &QComboBox::currentTextChanged, ui->openGLWidget, blendTest);
}

MainWindow::~MainWindow() {
    delete ui;
}
