/**
 * BackupInfoTable.cpp
 *
 */
#include "BackupInfoTable.h"

#include <QAbstractTableModel>
#include <QDir>
#include <QFile>

#include "utils/FileSystem.h"


//-----------------------------------------------------------------------------
//  BackupInfoTable
//-----------------------------------------------------------------------------
BackupInfoTable::BackupInfoTable(BackupInfoStore *store, QObject *parent)
    : QAbstractTableModel(parent)
    , mBackupInfoStore(store)
{
}

BackupInfoTable::~BackupInfoTable()
{
}


//-----------------------------------------------------------------------------
//  BackupInfoTable - QAbstractTableModel methods
//-----------------------------------------------------------------------------
int BackupInfoTable::rowCount(const QModelIndex & /*parent*/) const
{
    return this->mBackupInfoStore->size();
}

int BackupInfoTable::columnCount(const QModelIndex & /*parent*/) const
{
    // HEADER: TimeStamp | Backup path | Comment
    return 3;
}

QVariant BackupInfoTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section)
        {
        case 0: return QString("Timestamp");
        case 1: return QString("Backup");
        case 2: return QString("Comment");
        }
    }
    return QVariant();
}

QVariant BackupInfoTable::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    const BackupInfo info = this->mBackupInfoStore->get(index.row()); // throw std::out_of_range
    switch (index.column()) {
        case 0: return QString(info.timestamp);
        case 1: return utils::lastName(QString(info.path));
        case 2: return QString(info.comment);
    }

    return QVariant();
}

bool BackupInfoTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;
    if (!this->checkIndex(index)) return false;

    // Only comment(index:2) can be edited.
    if (index.column() != 2) return false;

    // Edit
    this->mBackupInfoStore->update(index.row(), value.toString());
    emit editCompleted(value.toString());

    return true;
}

Qt::ItemFlags BackupInfoTable::flags(const QModelIndex &index) const
{
    // Only comment(index:2) can be edited.
    if (index.column() == 2) {
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    }

    // Otherwise, editing is prohibited.
    return QAbstractTableModel::flags(index);
}


void BackupInfoTable::beginUpdate(const int var, const size_t index)
{
    bool is_add = (var >= 0);
    const size_t size = this->mBackupInfoStore->size();

    if (is_add) {
        this->beginInsertRows(QModelIndex(), size, size);
    } else {
        this->beginRemoveRows(QModelIndex(), index, index);
    }

}
void BackupInfoTable::endUpdate(const int var)
{
    bool is_add = (var >= 0);
    if (is_add) {
        this->endInsertRows();
    } else {
        this->endRemoveRows();
    }
}

