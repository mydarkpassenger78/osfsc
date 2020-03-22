#include <QFileDialog>
#include <QCloseEvent>
#include <QSettings>


#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "videocontroller.h"
#include "../data/project.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_Ui(new Ui::MainWindow)
{
    m_Ui->setupUi(this);

    m_VideoController = new VideoController(m_Ui, this);
    m_Project = new Project(m_Ui, this);

    connect(m_Project, &Project::NoVideo, m_VideoController, &VideoController::NoVideo);
    connect(m_Project, &Project::NewVideo, m_VideoController, &VideoController::NewVideo);
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("select video"),"",tr("Any file (*.*)"));

    if (!fileName.isEmpty())
    {
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
    settings.endGroup();
}

void MainWindow::WriteSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("splitter", m_Ui->splitter->saveState());
    settings.endGroup();
}

void MainWindow::ActivateSaveAction(bool state)
{
    m_Ui->actionSave->setEnabled(state);
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("select project"),"",tr("OSFSC projects (*.ofc)"));

    if (!fileName.isEmpty())
    {
        m_Project->Open(fileName);
    }
}

void MainWindow::on_action_Save_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("select project"),"",tr("OSFSC projects (*.ofc)"));

    if (!fileName.isEmpty())
    {
        m_Project->SaveAs(fileName);
    }
}
