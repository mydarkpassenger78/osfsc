#include "videocontroller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>

VideoController::VideoController(Ui::MainWindow *ui, MainWindow *parent) : QObject(parent), m_Ui(ui)
{
    connect(m_Ui->videoPlayButton, &QToolButton::clicked, this, &VideoController::Play);
    connect(m_Ui->videoPauseButton, &QToolButton::clicked, this, &VideoController::Pause);

    connect(m_Ui->speedSlider, &QAbstractSlider::valueChanged, this, &VideoController::SpeedSliderChanged);
    connect(m_Ui->videoPosition, &QAbstractSlider::sliderReleased, this, &VideoController::PositionSliderReleased);

    connect(m_Ui->videoBeginButton, &QToolButton::clicked, this, &VideoController::ToBegin);
    connect(m_Ui->videoEndButton, &QToolButton::clicked, this, &VideoController::ToEnd);
    connect(m_Ui->videoFwdButton, &QToolButton::clicked, this, &VideoController::FwdFrame);
    connect(m_Ui->videoBackButton, &QToolButton::clicked, this, &VideoController::BackFrame);

    CreateMediaPlayer();

    NoVideo();
}

void VideoController::CreateMediaPlayer()
{
    m_MediaPlayer = new QMediaPlayer(this);
    m_MediaPlayer->setVideoOutput(m_Ui->videoWidget);

    connect(m_MediaPlayer, &QMediaPlayer::durationChanged, this, &VideoController::DurationChanged);
    connect(m_MediaPlayer, &QMediaPlayer::positionChanged, m_Ui->videoPosition, &QSlider::setValue);
    connect(m_Ui->videoPosition, &QSlider::sliderMoved, m_MediaPlayer, &QMediaPlayer::setPosition);
}

void VideoController::AppQuit()
{
    m_MediaPlayer->stop();
}

void VideoController::NoVideo()
{
    m_ValidVideo = false;
    m_MediaPlayer->stop();
    m_Ui->buttonsHolder->setEnabled(false);
    m_Ui->videoWidget->update();
}

void VideoController::NewVideo(QString path)
{
    // reusing the same media player for another video doesn't work well on MacOS.
    // the resulting video player can't be controlled correctly.
    // so delete current media player and create a new one
    m_MediaPlayer->stop();
    m_MediaPlayer->setMedia(QMediaContent());
    m_MediaPlayer->deleteLater();

    CreateMediaPlayer();

    m_ValidVideo = true;
    m_MediaPlayer->setVideoOutput(m_Ui->videoWidget);
    m_MediaPlayer->setMedia(QUrl::fromLocalFile(path));
    m_Ui->buttonsHolder->setEnabled(true);
    m_MediaPlayer->pause();
    m_MediaPlayer->setPosition(0);
}

void VideoController::Play()
{
    m_Ui->speedSlider->setValue(10);
}

void VideoController::Pause()
{
    m_Ui->speedSlider->setValue(0);
}

void VideoController::DurationChanged(quint64 duration)
{
    m_Ui->videoPosition->setRange(0, duration);
}

void VideoController::SpeedSliderChanged(int value)
{
    if (m_ValidVideo)
    {
        if (value == 0)
        {
            m_MediaPlayer->pause();
            m_VideoPlaying = false;
        }
        else
        {
            if (!m_VideoPlaying)
            {
                m_MediaPlayer->play();

                QWidget *window = m_Ui->videoWidget->window();

                // if I don't force a resize of the widget here, there are all weird kind of artifacts on
                // MacOS, especially bad zoom factor, only until you resize the window
                window->resize(window->size()+QSize(1,1));
                window->resize(window->size()-QSize(1,1));
            }
            m_MediaPlayer->setPlaybackRate(value*0.1);
            m_VideoPlaying = true;
        }
    }
}

void VideoController::PositionSliderReleased()
{
    if (m_ValidVideo)
    {
        m_MediaPlayer->setPosition(m_Ui->videoPosition->value());
    }
}

void VideoController::ToBegin()
{
    if (m_ValidVideo)
    {
        m_MediaPlayer->setPosition(0);
    }
}

void VideoController::ToEnd()
{
    if (m_ValidVideo)
    {
        QVariant duration = m_MediaPlayer->metaData("Duration");

        if (duration.isValid())
        {
            m_MediaPlayer->setPosition(duration.toInt()-1);
        }
    }
}
void VideoController::BackFrame()
{
    m_MediaPlayer->setPosition(m_Ui->videoPosition->value()-1000);
}

void VideoController::FwdFrame()
{
    m_MediaPlayer->setPosition(m_Ui->videoPosition->value()+1000);
}