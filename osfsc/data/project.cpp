#include "project.h"
#include <QMap>
#include <QFile>
#include <QDataStream>
#include <QVariant>
#include <QSettings>
#include "../gui/ui_mainwindow.h"

const quint32 Project::c_Magic = 0x84b26d3e;
const quint32 Project::c_CurrentVersion = 0x100;

Project::Project(Ui::MainWindow *ui, QObject *parent) : QObject(parent)
{
    connect(ui->action_New, &QAction::triggered, this, &Project::New);
    connect(ui->actionSave, &QAction::triggered, this, &Project::Save);
}

void Project::New()
{
    m_FileName.clear();
    m_VideoFileName.clear();
    m_Modified = false;

    emit NoVideo();
    emit ActivateSaveAction(false);
}

void Project::Open(QString fileName)
{
    quint32 magic, version;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    in >> magic >> version;
    if ((magic == c_Magic) && (version == c_CurrentVersion))
    {
        m_FileName = fileName;
        QMap<QString,QVariant> fileContents;
        in >> fileContents;

        if (fileContents.contains("video"))
        {
            m_VideoFileName = fileContents["video"].toString();

            emit NewVideo(m_VideoFileName);
        }
        else
        {
            m_VideoFileName.clear();
            emit NoVideo();
        }
    }
    else
    {
        // error in file, or file not present
        New();
    }

    m_Modified = false;
    emit ActivateSaveAction(false);
}

void Project::Save()
{
    if (!m_FileName.isEmpty())
    {
        QFile file(m_FileName);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);

        out << c_Magic << c_CurrentVersion;

        QMap<QString, QVariant> fileContents;

        if (!m_VideoFileName.isEmpty())
        {
            fileContents.insert("video", m_VideoFileName);
        }

        out << fileContents;

        m_Modified = false;
        emit ActivateSaveAction(false);
    }
}

void Project::SaveAs(QString fileName)
{
    m_FileName = fileName;
    Save();
}

void Project::ReadSettings()
{
    QSettings settings;

    if(settings.contains("project"))
    {
        Open(settings.value("project").toString());
    }
}

void Project::WriteSettings()
{
    QSettings settings;

    if (m_FileName.isEmpty())
    {
        settings.remove("project");
    }
    else
    {
        settings.setValue("project", m_FileName);
    }
}

void Project::SetVideo(QString path)
{
    m_VideoFileName = path;

    if(path.isEmpty())
    {
        emit NoVideo();
    }
    else
    {
        emit NewVideo(path);
    }

    Modified();
}

void Project::Modified()
{
    m_Modified = true;
    if (!m_FileName.isEmpty())
    {
        emit ActivateSaveAction(true);
    }
}
