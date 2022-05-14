/**
 * FileSystem.h
 *
 */

#ifndef __src_utils_FileSystem_h__
#define __src_utils_FileSystem_h__

#include <QString>


namespace utils
{
    /**
     *  Return the directory path where the application file is stored.
     */
    QString getAppDataDirPath();

    /**
     * Create directories like 'mkdir -p'.
     */
    bool mkdir(const QString &path);

    /**
     *  Join path.
     */
    QString joinPaths(const QString &path, const QString &child);

    /**
     * Returns the name of the last element of the path.
     */
    QString lastName(const QString &path);

    /**
     *  File exists?
     */
    bool isExists(const QString &path);

    /**
     *  Copy directory.
     */
    void copyDirectory(const QString &src, const QString &dst);

    /**
     *  Remove directory.
     */
    void removeDirectory(const QString &path);

    /**
     * Simple CSV Escape.
     *
     * Do only the following.
     * - Escapes double quatation makrs
     * - Enclose the entire string in double quotation marks.
     *
     * @param str one-line string.
     */
    QString toCSVEscaped(const QString &str);
};


#endif /* __src_utils_FileSystem_h__ */

