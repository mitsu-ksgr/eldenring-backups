/**
 * AppConfig.h
 *
 */

#ifndef __src__AppConfig_h__
#define __src__AppConfig_h__

#include <QString>

class AppConfig
{
public:
    AppConfig(const QString &config_dir_path);
    ~AppConfig();

    void setEldenRingDirPath(const QString &path);
    QString getEldenRingDirPath() const;

private:
    void loadConfigs();
    void saveConfigs();

    QString mConfigDirPath;
    QString mEldenRingDirPath;
};


#endif /* __src__AppConfig_h__ */


