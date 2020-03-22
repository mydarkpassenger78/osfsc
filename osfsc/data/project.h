#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(Ui::MainWindow *ui, QObject *parent = nullptr);
    bool IsModified() { return (m_Modified); }

signals:
    void NoVideo();
    void NewVideo(QString path);
    void ActivateSaveAction(bool hasFileName);

public slots:
    void New();
    void Open(QString fileName);
    void Save();
    void SaveAs(QString fileName);
    void ReadSettings();
    void WriteSettings();
    void SetVideo(QString path);
    void Modified();

private:
    QString m_FileName;
    QString m_VideoFileName;
    bool m_Modified;

    static const quint32 c_Magic;
    static const quint32 c_CurrentVersion;
};

#endif // PROJECT_H
