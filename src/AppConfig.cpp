/**
 * AppConfig.cpp
 *
 * Manage the application config.
 * - EldenRing dir path
 */
#include "AppConfig.h"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

#include "utils/FileSystem.h"


//-----------------------------------------------------------------------------
//  Utils
//-----------------------------------------------------------------------------
namespace {
#ifdef Q_OS_WIN
    const char *kEldenRingPath = u8"AppData\\Roaming\\EldenRing";
#else   // for debug
    const char *kEldenRingPath = u8"AppData/Roaming/EldenRing";
#endif

    const char *kConfigFileName             = u8"config.ini";
    const char *kConfigGroupName            = u8"AppConfig";
    const char *kConfig_EldenRingDirPath    = u8"eldenring-dir-path";

    QString getDefaultEldenRingDirPath() {
        return utils::joinPaths(QDir::homePath(), QString(kEldenRingPath));
    }
};


//-----------------------------------------------------------------------------
//  BackupManager
//-----------------------------------------------------------------------------
AppConfig::AppConfig(const QString &config_dir_path)
    : mConfigDirPath(config_dir_path)
    , mEldenRingDirPath(getDefaultEldenRingDirPath())
{
    utils::mkdir(this->mConfigDirPath);
    this->loadConfigs();
}

AppConfig::~AppConfig()
{
    this->saveConfigs();
}

void AppConfig::loadConfigs()
{
    QString path = utils::joinPaths(this->mConfigDirPath, kConfigFileName);
    QSettings qs(path, QSettings::IniFormat, nullptr);

    qs.beginGroup(kConfigGroupName);
    this->mEldenRingDirPath = qs.value(
            kConfig_EldenRingDirPath, getDefaultEldenRingDirPath()
    ).toString();
    qs.endGroup();
}

void AppConfig::saveConfigs()
{
    QString path = utils::joinPaths(this->mConfigDirPath, kConfigFileName);
    QSettings qs(path, QSettings::IniFormat, nullptr);

    qs.beginGroup(kConfigGroupName);
    qs.setValue(kConfig_EldenRingDirPath, this->mEldenRingDirPath);
    qs.endGroup();
}


//-----------------------------------------------------------------------------
//  BackupManager - getter/setter
//-----------------------------------------------------------------------------
QString AppConfig::getEldenRingDirPath() const
{
    return this->mEldenRingDirPath;
}

void AppConfig::setEldenRingDirPath(const QString &path)
{
    // TODO: should I check for the existence of dir?
    this->mEldenRingDirPath = QString(path);
}

