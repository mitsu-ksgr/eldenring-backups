/**
 * BackupInfoTable.h
 *
 */

#ifndef __src__BackupInfoTable_h__
#define __src__BackupInfoTable_h__

#include <vector>

#include <QAbstractTableModel>

#include "BackupInfo.h"


class BackupInfoTable : public QAbstractTableModel
{
    Q_OBJECT;

public:
    explicit BackupInfoTable(BackupInfoStore *store, QObject *parent = nullptr);
    ~BackupInfoTable() override;

    void setBackupInfoStore(const BackupInfoStore *store);
    void beginUpdate(const int var, const size_t index = 0);
    void endUpdate(const int var);

    // QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

signals:
    void editCompleted(const QString &);


private:
    BackupInfoStore *mBackupInfoStore;
};


#endif /* __src__BackupInfoTable_h__ */

