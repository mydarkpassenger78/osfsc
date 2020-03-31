#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QObject>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow;

#include <QMediaPlayer>

class VideoController : public QObject
{
    Q_OBJECT
public:
    explicit VideoController(Ui::MainWindow *ui, MainWindow *parent);

signals:
    void ProjectModified();

public slots:
    void AppQuit();
    void NoVideo();
    void NewVideo(QString path);
    void VideoSetPos(int position);
    void Play();
    void Pause();
    void VolumeEnableChanged(int state);
    void VolumeValueChanged(int value);

    void DurationChanged(quint64 duration);
    void SpeedSliderChanged(int value);

    void MediaError(QMediaPlayer::Error error);

    void Back10();
    void Fwd10();
    void BackFrame();
    void FwdFrame();

private:
    void CreateMediaPlayer();
    void SetVolume();

    QMediaPlayer *m_MediaPlayer;
    Ui::MainWindow *m_Ui;
    bool m_ValidVideo;
    bool m_VideoPlaying;

    bool m_PosAdjustNeeded;
    int m_PosAdjust;
};

#endif // VIDEOCONTROLLER_H
