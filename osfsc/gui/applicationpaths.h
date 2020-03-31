#ifndef APPLICATIONPATHS_H
#define APPLICATIONPATHS_H

class QSettings;
class QFileDialog;

#include <QString>

class ApplicationPaths
{
public:
    ApplicationPaths();

    typedef enum _pathtype { VIDEO_PATH, SCRIPT_PATH, PROJECT_PATH } PathType;

    static QString GetPath(PathType pathType, QSettings *settings = nullptr);

    static void WritePath(PathType pathType, QString filePath);
    static void WritePath(PathType pathType, QFileDialog *dialog);

private:
    static void WritePathInt(PathType pathType, QString dirPath);
};

#endif // APPLICATIONPATHS_H
