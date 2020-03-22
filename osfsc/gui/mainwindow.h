#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class VideoController;
class Project;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSet_Media_triggered();

    void ActivateSaveAction(bool state);

    void on_action_Open_triggered();

    void on_action_Save_As_triggered();

protected:
    void closeEvent(QCloseEvent *event);

    void ReadSettings();
    void WriteSettings();

private:
    Ui::MainWindow *m_Ui;
    VideoController *m_VideoController;
    Project *m_Project;
};
#endif // MAINWINDOW_H
