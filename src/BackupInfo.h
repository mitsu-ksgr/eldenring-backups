/**
 * BackupInfo.h
 *
 */

#ifndef __src__BackupInfo_h__
#define __src__BackupInfo_h__

#include <QString>

struct BackupInfo
{
public:
    QString timestamp;
    QString path;
    QString comment;
};


class BackupInfoStore
{
public:
    virtual ~BackupInfoStore() {};

    virtual size_t size() const = 0;
    virtual BackupInfo get(const size_t index) const = 0;
    virtual void add(const BackupInfo &info) = 0;
    virtual void remove(const size_t index) = 0;
    virtual bool update(const size_t index, const QString &comment) = 0;
};

#endif /* __src__BackupInfo_h__ */

