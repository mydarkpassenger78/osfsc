#include "videocontroller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QPushButton>
#include <QtGlobal>

VideoController::VideoController(Ui::MainWindow *ui, MainWindow *parent) : QObject(parent), m_Ui(ui)
{
    connect(m_Ui->videoPlayButton, &QToolButton::clicked, this, &VideoController::Play);
    connect(m_Ui->videoPauseButton, &QToolButton::clicked, this, &VideoController::Pause);

    connect(m_Ui->speedSlider, &QAbstractSlider::valueChanged, this, &VideoController::SpeedSliderChanged);

    connect(m_Ui->videoBack10Button, &QToolButton::clicked, this, &VideoController::Back10);
    connect(m_Ui->videoFwd10Button, &QToolButton::clicked, this, &VideoController::Fwd10);
    connect(m_Ui->videoFwdButton, &QToolButton::clicked, this, &VideoController::FwdFrame);
    connect(m_Ui->videoBackButton, &QToolButton::clicked, this, &VideoController::BackFrame);

    CreateMediaPlayer();

    NoVideo();

    m_PosAdjustNeeded = false;
}

void VideoController::CreateMediaPlayer()
{
    m_MediaPlayer = new QMediaPlayer(this);
    m_MediaPlayer->setVideoOutput(m_Ui->videoWidget);

    SetVolume();

    connect(m_MediaPlayer, &QMediaPlayer::durationChanged, this, &VideoController::DurationChanged);
    connect(m_MediaPlayer, &QMediaPlayer::positionChanged, m_Ui->videoPosition, &QSlider::setValue);
    connect(m_Ui->videoPosition, &QSlider::sliderMoved, m_MediaPlayer, &QMediaPlayer::setPosition);
    connect(m_Ui->volumeEnable, &QCheckBox::stateChanged, this, &VideoController::VolumeEnableChanged);
    connect(m_Ui->volumeSlider, &QSlider::sliderMoved, this, &VideoController::VolumeValueChanged);

    connect(m_MediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &VideoController::MediaError);
}

void VideoController::SetVolume()
{
    if (m_MediaPlayer != nullptr)
    {
        if (m_Ui->volumeEnable->isChecked())
        {
            m_Ui->volumeSlider->setEnabled(true);
            m_MediaPlayer->setVolume(m_Ui->volumeSlider->value());
        }
        else
        {
            m_Ui->volumeSlider->setEnabled(false);
            m_MediaPlayer->setVolume(0);
        }
    }
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

void VideoController::VideoSetPos(int newPosition)
{
    m_PosAdjustNeeded = true;
    m_PosAdjust = newPosition;
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

    if (m_PosAdjustNeeded)
    {
        m_MediaPlayer->setPosition(m_PosAdjust);
    }

    m_PosAdjustNeeded = false;
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

void VideoController::VolumeEnableChanged(int state)
{
    Q_UNUSED(state)

    SetVolume();
}

void VideoController::VolumeValueChanged(int value)
{
    Q_UNUSED(value)

    SetVolume();
}

static const int delta10s = 10000;

void VideoController::Back10()
{
    int position = m_Ui->videoPosition->value();
    if (m_ValidVideo)
    {
        if (position < delta10s)
        {
            m_MediaPlayer->setPosition(0);
        }
        else
        {
            m_MediaPlayer->setPosition(position-delta10s);
        }
    }
}

void VideoController::Fwd10()
{
    if (m_ValidVideo)
    {
        QVariant duration = m_MediaPlayer->metaData("Duration");

        if (duration.isValid())
        {
            int end = duration.toInt()-1;
            int position = m_Ui->videoPosition->value();

            if (position > end-delta10s)
            {
                m_MediaPlayer->setPosition(end);
            }
            else
            {
                m_MediaPlayer->setPosition(position+delta10s);
            }
        }
    }
}

void VideoController::BackFrame()
{
    if (m_ValidVideo)
    {
        QVariant rate = m_MediaPlayer->metaData("VideoFrameRate");

        if (rate.isValid())
        {
            int frameduration = 1000/rate.toInt()+1;
            int position = m_Ui->videoPosition->value();

            if (position<frameduration)
            {
                m_MediaPlayer->setPosition(0);
            }
            else
            {
                m_MediaPlayer->setPosition(position-frameduration);
            }
        }
    }
}

void VideoController::FwdFrame()
{
    if (m_ValidVideo)
    {
        QVariant duration = m_MediaPlayer->metaData("Duration");
        QVariant rate = m_MediaPlayer->metaData("VideoFrameRate");

        if (duration.isValid() && rate.isValid())
        {
            int end = duration.toInt()-1;
            int frameduration = 1000/rate.toInt()+1;
            int position = m_Ui->videoPosition->value();

            if (position > end-frameduration)
            {
                m_MediaPlayer->setPosition(end);
            }
            else
            {
                m_MediaPlayer->setPosition(position+frameduration);
            }
        }
    }
}

void VideoController::MediaError(QMediaPlayer::Error error)
{
    QMessageBox box;
    bool codecLink = false;
    QAbstractButton *codecButton = nullptr;

    box.setIcon(QMessageBox::Icon::Critical);
    switch(error)
    {
    case QMediaPlayer::NoError:
        box.setText("Media error: no error");
        break;

    case QMediaPlayer::ResourceError:
        box.setText("Media error: A media resource couldn't be resolved");
        codecLink = true;
        break;

    case QMediaPlayer::FormatError:
        box.setText("Media error: The format of a media resource isn't (fully) supported.");
        codecLink = true;
        break;

    case QMediaPlayer::NetworkError:
        box.setText("Media error: network error");
        break;

    case QMediaPlayer::AccessDeniedError:
        box.setText("Media error: access denied");
        break;

    case QMediaPlayer::ServiceMissingError:
        box.setText("Media error: A valid playback service was not found, playback cannot proceed.");
        break;

    case QMediaPlayer::MediaIsPlaylist:
        box.setText("Media error: media is a playlist");
        break;

    default:
        box.setText("Media error: unknown error");
    }

    box.addButton(QMessageBox::Ok);
#ifdef Q_OS_WINDOWS
    if (codecLink)
    {
        box.setDetailedText("Error might be resolved by installing extra media codecs, from for example https://www.codecguide.com/download_kl.htm");
        codecButton = box.addButton(tr("find codec..."), QMessageBox::ActionRole);
    }
#endif
    box.setDefaultButton(QMessageBox::Ok);
    box.exec();

    if (codecLink && (box.clickedButton() == codecButton))
    {
        QDesktopServices::openUrl (QUrl("https://www.codecguide.com/download_kl.htm"));
    }
}
