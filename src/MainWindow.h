/**
 *  MainWindow.h
 *
 */

#ifndef __src__MainWindow_h__
#define __src__MainWindow_h__

#include <QMainWindow>

#include "AppConfig.h"
#include "BackupInfoTable.h"
#include "BackupManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_FindSaveFilePath_clicked();
    void on_pushButton_Backup_clicked();
    void on_pushButton_Restore_clicked();
    void on_pushButton_Remove_clicked();
    void on_lineEdit_SavePath_textChanged(QString text);


private:
    void init();
    void initUI();
    void initStyle();

    Ui::MainWindow  *mUI;
    AppConfig       *mAppConfig;
    BackupInfoTable *mBackupInfoTable;
    BackupManager   *mBackupManager;
};


#endif /* __src__MainWindow_h__ */

