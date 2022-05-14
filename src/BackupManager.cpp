/**
 * BackupManager.cpp
 *
 */
#include "BackupManager.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "lib/csv.h"
#include "utils/FileSystem.h"


//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
namespace {
    constexpr const char *kBackupDirName        = u8"backups";
    constexpr const char *kBackupInfoFileName   = u8"eldenring-backups.csv";

    // BackupInfo CSV
    namespace BackupInfoCSV {
        constexpr const unsigned int ColumnCount = 3;
        constexpr const char *Header_TimeStamp   = u8"timestamp";
        constexpr const char *Header_Path        = u8"path";
        constexpr const char *Header_Comment     = u8"comment";
    };
};


//-----------------------------------------------------------------------------
//  BackupManager Class
//-----------------------------------------------------------------------------
BackupManager::BackupManager(
        const QString &app_data_dir_path,
        const QString &eldenring_savedata_path)
    : mEldenRingSaveDataPath(eldenring_savedata_path)
    , mBackupInfoPath("")
    , mBackupDirPath("")
    , mRecords({})
{
    // Setup backup info path.
    this->mBackupInfoPath = utils::joinPaths(app_data_dir_path, kBackupInfoFileName);

    // Setup backups dir
    this->mBackupDirPath = utils::joinPaths(app_data_dir_path, kBackupDirName);
    utils::mkdir(this->mBackupDirPath);

    // Load info file
    this->loadBackupInfo();
}

BackupManager::~BackupManager()
{
}

void BackupManager::updateEldenRingSaveDataDirPath(const QString &path)
{
    this->mEldenRingSaveDataPath = path;
}


//-----------------------------------------------------------------------------
//  BackupManager - Functions for the BackupInfo file
//-----------------------------------------------------------------------------
bool BackupManager::loadBackupInfo()
{
    if (!utils::isExists(this->mBackupInfoPath)) {
        return false; // File not found.
    }

    // Load info from csv file.
    io::CSVReader<
        BackupInfoCSV::ColumnCount,
        io::trim_chars<' '>,
        io::double_quote_escape<',','\"'>,
        io::throw_on_overflow,
        io::empty_line_comment
    > in(this->mBackupInfoPath.toStdString());
    in.read_header(io::ignore_extra_column,
        BackupInfoCSV::Header_TimeStamp,
        BackupInfoCSV::Header_Path,
        BackupInfoCSV::Header_Comment);

    std::string timestamp, path, comment;
    while(in.read_row(timestamp, path, comment)) {
        this->mRecords.push_back({
            QString(timestamp.c_str()),
            QString(path.c_str()),
            QString(comment.c_str())
        });
    }

    return true;
}

/**
 * Save strategy:
 * - 1. Backup old file.
 * - 2. Save latest info to file.
 * - 3. If the save is successful, remove the old file.
 *      Otherwise, the old file resotred.
 */
bool BackupManager::saveBackupInfo()
{
    QString oldfilepath = this->mBackupInfoPath + ".bk";
    // QString("%1%2").arg(this->mBackupInfoPath)

    // If info file already exists, temporarily save them.
    if (utils::isExists(this->mBackupInfoPath)) {
        if (!QFile::rename(this->mBackupInfoPath, oldfilepath)) {
            // TODO: show error message: can't remove old file.
            return false;
        }
    }

    // Save infos
    QFile file(this->mBackupInfoPath);
    if (!file.open(QIODevice::WriteOnly)) {
        // restore old file.
        if (!QFile::rename(oldfilepath, this->mBackupInfoPath)) {
            return false; // TODO: show error message: can't write data.
        }
        return false; // TODO: show errro message: can't write data, and failed restrore.
    }

    // write CSV header
    namespace CSV = BackupInfoCSV;
    QTextStream ts(&file);
    ts << CSV::Header_TimeStamp << ",";
    ts << CSV::Header_Path << ",";
    ts << CSV::Header_Comment << '\n';

    // write data
    for (auto &info : this->mRecords) {
        ts << utils::toCSVEscaped(info.timestamp) << ",";
        ts << utils::toCSVEscaped(info.path) << ",";
        ts << utils::toCSVEscaped(info.comment) << '\n';
    }
    file.close();
    // TODO: check success or failed. if failed, restore with old file.

    // Remove old file
    if (!QFile::remove(oldfilepath)) {
        // TODO: show alert message: save succeeded, but can't remove old file.
    }

    return true;
}


//-----------------------------------------------------------------------------
//  BackupManager - Backup / Restore
//-----------------------------------------------------------------------------
/**
 *  Backup strategy:
 *  - 1. Copy the EldenRing Savefile directory, directory by directory.
 *  - 2. Save backup info to the backup info file.
 */
bool BackupManager::backup(const QString &comment)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString path = utils::joinPaths(
            this->mBackupDirPath, dt.toString("yyyyMMdd_hhmmsszzz"));

    BackupInfo info = {
        dt.toString("yyyy-MM-dd hh:mm:ss"),
        path,
        comment
    };

    utils::copyDirectory(this->mEldenRingSaveDataPath, info.path);
    this->mRecords.push_back(info);
    this->saveBackupInfo();

    return true;
}

/**
 *  Restore Strategy.
 *  - 1. Backup current save data.
 *  - 2. Replace current save data with backup data.
 *      - 2-1. Remove current save data.
 *      - 2-2. Copy a backup data to the EldenRing Path
 */
bool BackupManager::restore(const size_t index)
{
    BackupInfo &info = this->mRecords.at(index);

    // Backup current save data.
    this->backup(
        QString(u8"Auto-backup before restore %1")
        .arg(utils::lastName(info.path))
    );

    // Replace current save data with backup data.
    utils::removeDirectory(this->mEldenRingSaveDataPath);
    utils::copyDirectory(info.path, this->mEldenRingSaveDataPath);

    return true;
}

bool BackupManager::removeBackup(const size_t index)
{
    BackupInfo info = this->mRecords.at(index);
    utils::removeDirectory(info.path);
    this->mRecords.erase(this->mRecords.begin() + index);
    this->saveBackupInfo();
    return true;
}


//-----------------------------------------------------------------------------
//  BackupManager - BackupInfoStore
//-----------------------------------------------------------------------------
size_t BackupManager::size() const
{
    return this->mRecords.size();
}

BackupInfo BackupManager::get(const size_t index) const
{
    return this->mRecords.at(index);
}

void BackupManager::add(const BackupInfo &/*info*/)
{
    // do nothing, use backup()
}

void BackupManager::remove(const size_t index)
{
    this->removeBackup(index);
}

bool BackupManager::update(const size_t index, const QString &comment)
{
    try {
        BackupInfo &info = this->mRecords.at(index);
        info.comment = comment;
        this->saveBackupInfo();
        return true;

    } catch (std::out_of_range &/*e*/) {
        return false;
    }
}

