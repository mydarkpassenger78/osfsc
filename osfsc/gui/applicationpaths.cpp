#include "applicationpaths.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>
#include <QDir>

ApplicationPaths::ApplicationPaths()
{
}

QString ApplicationPaths::GetPath(PathType pathType, QSettings *settings)
{
    QString result;
    QString key;

    if (settings == nullptr)
    {
        settings = new QSettings;
        settings->beginGroup("paths");
    }

    switch(pathType)
    {
        case VIDEO_PATH:
            key = "video";
        break;

        case SCRIPT_PATH:
            key = "script";
        break;

        case PROJECT_PATH:
            key = "project";
        break;

    default:
        qWarning("unknown path type parameter");
        delete settings;
        return result; // empty string
    }

    if (settings->contains(key))
    {
        result = settings->value(key).toString();
        delete settings;
        return result;
    }

    // settings not found, se if we can resort to another path
    switch(pathType)
    {
        case VIDEO_PATH:
            delete settings;
        break;

        case SCRIPT_PATH:
            result = GetPath(VIDEO_PATH, settings);
        break;

        case PROJECT_PATH:
            result = GetPath(VIDEO_PATH, settings);
        break;

        default:
            delete settings;
        break;
    }

    return result;
}

void ApplicationPaths::WritePathInt(PathType pathType, QString dirPath)
{
    QString key;

    switch(pathType)
    {
        case VIDEO_PATH:
            key = "video";
        break;

        case SCRIPT_PATH:
            key = "script";
        break;

        case PROJECT_PATH:
            key = "project";
        break;

    default:
        qWarning("unknown path type parameter");
        return;
    }
    QSettings settings;
    settings.beginGroup("paths");
    settings.setValue(key, dirPath);
}

void ApplicationPaths::WritePath(PathType pathType, QFileDialog *dialog)
{
    WritePathInt(pathType, dialog->directory().absolutePath());
}

void ApplicationPaths::WritePath(PathType pathType,QString filePath)
{
    QDir dirPath(filePath);
    dirPath.cdUp();

    WritePathInt(pathType, dirPath.absolutePath());
}
