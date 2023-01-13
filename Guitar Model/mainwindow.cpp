#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("cw");

    auto display = [=]() {
        int index = ui->projectionComboBox->currentIndex();
        bool invisible = ui->invisibleCheckBox->isChecked();
        bool coordinates = ui->coordinatesCheckBox->isChecked();
        ui->openGLWidget->changeDisplayOptions(index, invisible, coordinates);
    };

    auto objectParams = [=]() {
        QVector3D scale = {
            ui->xScaleSlider->value() / 100.0f,
            ui->yScaleSlider->value() / 100.0f,
            ui->zScaleSlider->value() / 100.0f
        };

        QVector3D translate = {
            ui->xTranslateSlider->value() / 100.0f - 1.0f,
            ui->yTranslateSlider->value() / 100.0f - 1.0f,
            ui->zTranslateSlider->value() / 100.0f - 1.0f
        };

        ui->openGLWidget->changeObjectOptions(scale, translate);
    };

    auto lightParams = [=]() {
        int type = ui->lightTypeComboBox->currentIndex();
        QVector3D position = {
            ui->xLightTranslateSlider->value() / 100.0f - 1.0f,
            ui->yLightTranslateSlider->value() / 100.0f - 1.0f,
            ui->zLightTranslateSlider->value() / 100.0f - 1.0f
        };
        float ambientParam = ui->lightAmbientSlider->value() / 100.0f;
        QVector3D ambient = {ambientParam, ambientParam, ambientParam};
        QVector3D diffuse = {
            (float)ui->lightDiffuseSpinBox1->value(),
            (float)ui->lightDiffuseSpinBox2->value(),
            (float)ui->lightDiffuseSpinBox3->value()
        };
        ui->openGLWidget->changeLightOptions(type, position, ambient, diffuse);
    };

    auto materialParams = [=]() {
        float ambientParam = ui->materialAmbientSlider->value() / 100.0f;
        QVector3D ambient = {ambientParam, ambientParam, ambientParam};
        QVector3D diffuse = {
            (float)ui->materialDiffuseSpinBox1->value(),
            (float)ui->materialDiffuseSpinBox2->value(),
            (float)ui->materialDiffuseSpinBox3->value()
        };
        QVector3D specular = {
            (float)ui->materialSpecularSpinBox1->value(),
            (float)ui->materialSpecularSpinBox2->value(),
            (float)ui->materialSpecularSpinBox3->value()
        };
        float shininess = (float)ui->materialShininessSlider->value();
        ui->openGLWidget->changeObjectMaterialOprions(ambient, diffuse, specular, shininess);
    };

    connect(ui->projectionComboBox, &QComboBox::currentIndexChanged, ui->openGLWidget, display);
    connect(ui->invisibleCheckBox, &QCheckBox::stateChanged, ui->openGLWidget, display);
    connect(ui->coordinatesCheckBox, &QCheckBox::stateChanged, ui->openGLWidget, display);

    connect(ui->xScaleSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);
    connect(ui->yScaleSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);
    connect(ui->zScaleSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);
    connect(ui->xTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);
    connect(ui->yTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);
    connect(ui->zTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, objectParams);

    connect(ui->xLightTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->yLightTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->zLightTranslateSlider, &QSlider::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->lightTypeComboBox, &QComboBox::currentIndexChanged, ui->openGLWidget, lightParams);
    connect(ui->lightAmbientSlider, &QSlider::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->lightDiffuseSpinBox1, &QDoubleSpinBox::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->lightDiffuseSpinBox2, &QDoubleSpinBox::valueChanged, ui->openGLWidget, lightParams);
    connect(ui->lightDiffuseSpinBox3, &QDoubleSpinBox::valueChanged, ui->openGLWidget, lightParams);

    connect(ui->materialAmbientSlider, &QSlider::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialDiffuseSpinBox1, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialDiffuseSpinBox2, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialDiffuseSpinBox3, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialSpecularSpinBox1, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialSpecularSpinBox2, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialSpecularSpinBox3, &QDoubleSpinBox::valueChanged, ui->openGLWidget, materialParams);
    connect(ui->materialShininessSlider, &QSlider::valueChanged, ui->openGLWidget, materialParams);
}

MainWindow::~MainWindow()
{
    delete ui;
}

