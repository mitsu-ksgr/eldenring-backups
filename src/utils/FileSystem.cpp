/**
 * FileSystem.cpp
 *
 * - Utilities for filesystem.
 */
#include "FileSystem.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>

namespace utils
{
    QString getAppDataDirPath()
    {
        return QStandardPaths::writableLocation(
            QStandardPaths::AppLocalDataLocation
        );
    }

    bool mkdir(const QString &path)
    {
        QDir dir(path);
        return QDir().mkpath(dir.absolutePath());
    }

    QString joinPaths(const QString &path, const QString &child)
    {
        return path + QDir::separator() + child;
    }

    QString lastName(const QString &path)
    {
        const int idx = path.lastIndexOf(QDir::separator());
        if (idx == -1) return path;
        return path.mid(idx + 1);
    }

    bool isExists(const QString &path)
    {
        QFileInfo info(path);
        return info.exists();
    }

    void copyDirectory(const QString &src, const QString &dst)
    {
        QDir cur(src);
        if (!cur.exists())
            return;

        // Copy dirs
        foreach (QString dir, cur.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString s_child = joinPaths(src, dir);
            QString d_child = joinPaths(dst, dir);
            cur.mkpath(d_child);
            copyDirectory(s_child, d_child);
        }

        // Copy files
        foreach (QString file, cur.entryList(QDir::Files)) {
            QFile::copy(joinPaths(src, file), joinPaths(dst, file));
        }

        return;
    }

    void removeDirectory(const QString &path)
    {
        QDir dir(path);
        if (!dir.exists()) return;
        dir.removeRecursively();
    }

    QString toCSVEscaped(const QString &str)
    {
        QString ret;
        const size_t len = str.length();
        ret.reserve(int(len + 2));  // Add two `"`

        ret += "\"";
        for (size_t i = 0; i < len; ++i) {
            if (str.at(i) == QLatin1Char('"')) {
                ret += "\"\"";
            } else {
                ret += str.at(i);
            }
        }
        ret += "\"";

        return ret;
    }
};

