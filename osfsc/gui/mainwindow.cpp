#include <QFileDialog>
#include <QCloseEvent>
#include <QSettings>


#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "videocontroller.h"
#include "../data/project.h"
#include "applicationpaths.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_Ui(new Ui::MainWindow)
{
    m_Ui->setupUi(this);

    m_VideoController = new VideoController(m_Ui, this);
    m_Project = new Project(m_Ui, this);

    connect(m_Project, &Project::NoVideo, m_VideoController, &VideoController::NoVideo);
    connect(m_Project, &Project::NewVideo, m_VideoController, &VideoController::NewVideo);
    connect(m_Project, &Project::VideoSetPos, m_VideoController, &VideoController::VideoSetPos);
    connect(m_Project, &Project::ActivateSaveAction, this, &MainWindow::ActivateSaveAction);

    connect(m_VideoController, &VideoController::ProjectModified, m_Project, &Project::Modified);

    ReadSettings();
//    m_MediaPlayer = new QMediaPlayer(ui->videoWidget);
}

MainWindow::~MainWindow()
{
    delete m_Ui;
}


void MainWindow::on_actionSet_Media_triggered()
{
    QString defaultPath = ApplicationPaths::GetPath(ApplicationPaths::VIDEO_PATH);
    QString fileName = QFileDialog::getOpenFileName(this, tr("select video"),defaultPath,tr("Any file (*.*)"));

    if (!fileName.isEmpty())
    {
        ApplicationPaths::WritePath(ApplicationPaths::VIDEO_PATH, fileName);

        m_Project->SetVideo(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_VideoController->AppQuit();
    WriteSettings();
    event->accept();
}

void MainWindow::ReadSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", size()).toSize());
    move(settings.value("pos", pos()).toPoint());
    m_Ui->splitter->restoreState(settings.value("splitter").toByteArray());
    m_Ui->volumeEnable->setCheckState((Qt::CheckState)settings.value("volumecheck", Qt::Unchecked).toInt());
    m_Ui->volumeSlider->setValue(settings.value("volumeValue", 0).toInt());

    settings.endGroup();

    m_Project->ReadSettings();
}

void MainWindow::WriteSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("splitter", m_Ui->splitter->saveState());
    settings.setValue("volumecheck", (int)m_Ui->volumeEnable->checkState());
    settings.setValue("volumeValue", m_Ui->volumeSlider->value());
    settings.endGroup();

    m_Project->WriteSettings();
}

void MainWindow::ActivateSaveAction(bool state)
{
    m_Ui->actionSave->setEnabled(state);
}

void MainWindow::on_action_Open_triggered()
{
    QString defaultPath = ApplicationPaths::GetPath(ApplicationPaths::PROJECT_PATH);
    QString fileName = QFileDialog::getOpenFileName(this, tr("select project"),defaultPath,tr("OSFSC projects (*.ofc)"));

    if (!fileName.isEmpty())
    {
        ApplicationPaths::WritePath(ApplicationPaths::PROJECT_PATH, fileName);

        m_Project->Open(fileName);
    }
}

void MainWindow::on_action_Save_As_triggered()
{
    QString defaultPath = ApplicationPaths::GetPath(ApplicationPaths::PROJECT_PATH);
    QString fileName = QFileDialog::getSaveFileName(this, tr("select project"),defaultPath,tr("OSFSC projects (*.ofc)"));

    if (!fileName.isEmpty())
    {
        ApplicationPaths::WritePath(ApplicationPaths::PROJECT_PATH, fileName);

        m_Project->SaveAs(fileName);
    }
}
