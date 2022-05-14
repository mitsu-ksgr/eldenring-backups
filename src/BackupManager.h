/**
 *  BackupManager.h
 *
 *  - Perform backup.
 *  - Restore save data from backups.
 *  - Manage backup infos.
 *      - save info to file
 */

#ifndef __src__BackupManager_h__
#define __src__BackupManager_h__

#include <vector>

#include <QString>
#include <QAbstractTableModel>

#include "BackupInfo.h"

class BackupManager final : public BackupInfoStore
{
public:
    BackupManager(
        const QString &app_data_dir_path,
        const QString &eldenring_savedata_path);
    ~BackupManager();

    void updateEldenRingSaveDataDirPath(const QString &path);
    bool backup(const QString &comment);
    bool restore(const size_t index);

    // BackupInfoStore
    size_t size() const override;
    BackupInfo get(const size_t index) const override;
    void add(const BackupInfo &info) override;
    void remove(const size_t index) override;
    bool update(const size_t index, const QString &comment) override;


private:
    void init();
    bool loadBackupInfo();
    bool saveBackupInfo();
    bool removeBackup(const size_t index);

    QString mEnableAutoSave;
    QString mEldenRingSaveDataPath;
    QString mBackupInfoPath;
    QString mBackupDirPath;
    std::vector<BackupInfo> mRecords;
};


#endif /* __src__BackupManager_h__ */


