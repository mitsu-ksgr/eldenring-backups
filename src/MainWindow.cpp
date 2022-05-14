#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QAbstractItemView>
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>
#include <QtGui>

//#include "BackupManager.h"
#include "AppConfig.h"
#include "BackupInfoTable.h"
#include "utils/FileSystem.h"


//-----------------------------------------------------------------------------
//  Utils
//-----------------------------------------------------------------------------
namespace {
#ifdef Q_OS_WIN
    constexpr const char *kDefaultSaveFilePath = "";
#else
    constexpr const char *kDefaultSaveFilePath = "/home";
#endif
};



//-----------------------------------------------------------------------------
//  MainWindow
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
    , mAppConfig(nullptr)
    , mBackupInfoTable(nullptr)
    , mBackupManager(nullptr)
{
    this->mUI->setupUi(this);
    this->init();
    this->initUI();
    this->initStyle();
}

MainWindow::~MainWindow()
{
    if (this->mBackupInfoTable) {
        delete this->mBackupInfoTable;
        this->mBackupInfoTable = nullptr;
    }
    if (this->mBackupManager) {
        delete this->mBackupManager;
        this->mBackupManager = nullptr;
    }
    if (this->mAppConfig) {
        delete this->mAppConfig;
        this->mAppConfig = nullptr;
    }
    if (this->mUI) {
        delete this->mUI;
        this->mUI = nullptr;
    }
}

void MainWindow::init()
{
    // Init AppConfig
    this->mAppConfig = new AppConfig(utils::getAppDataDirPath());

    // Init BackupManagre
    this->mBackupManager = new BackupManager(
        utils::getAppDataDirPath(),
        this->mAppConfig->getEldenRingDirPath()
    );
}

void MainWindow::initUI()
{
    // lineEdit: SaveFilePath
    this->mUI->lineEdit_SavePath->setText(
        this->mAppConfig->getEldenRingDirPath()
    );
    QObject::connect(
        this->mUI->lineEdit_SavePath, SIGNAL(textChanged(QString)),
        this, SLOT(on_lineEdit_SavePath_textChanged(QSting))
    );

    // lineEdit: BackupPath
    this->mUI->lineEdit_BackupPath->setText(
        utils::getAppDataDirPath()
    );

    // BackupInfo Table
    this->mBackupInfoTable = new BackupInfoTable(this->mBackupManager);
    this->mUI->tableView_Backups->setModel(this->mBackupInfoTable);
    this->mUI->tableView_Backups->show();

    QHeaderView *header = this->mUI->tableView_Backups->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setStretchLastSection(true);

    this->mUI->tableView_Backups->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->mUI->tableView_Backups->setSelectionMode(QAbstractItemView::SingleSelection);

    // Hide MenuBar / ToolBar
    this->mUI->menuBar->hide();
    this->mUI->mainToolBar->hide();
    this->mUI->toolBar->hide();

    // Init StatusBar
    this->mUI->statusBar->showMessage("");
}

void MainWindow::initStyle()
{
#define WILL_INCLUDE_IN_CPP(qss) #qss
    constexpr const char *style =
#include "../res/style.qss"
    ;
#undef WILL_INCLUDE_IN_CPP

    this->setStyleSheet(style);
}


//-----------------------------------------------------------------------------
//  MainWindow - Slots
//-----------------------------------------------------------------------------
void MainWindow::on_pushButton_FindSaveFilePath_clicked()
{
    // Show select directory dialog
    QString filepath = QFileDialog::getExistingDirectory(this,
        "Directory of the EldenRing save file.",
        kDefaultSaveFilePath);

    // Canceled
    if (filepath.isNull()) return;

    // Selected
    this->mAppConfig->setEldenRingDirPath(filepath);
    this->mBackupManager->updateEldenRingSaveDataDirPath(filepath);
    this->mUI->lineEdit_SavePath->setText(filepath);

    // Notification
    this->mUI->statusBar->showMessage(
        u8"Updated path of the EldenRing's save directory."
    );
}

void MainWindow::on_pushButton_Backup_clicked()
{
    // Show dialog
    QString message = QString("Make a new backup.\nEnter a backup comment:");
    bool result = false;
    QString comment = QInputDialog::getText(
        this,
        "New Backup",
        message,
        QLineEdit::Normal,
        "",
        &result
    );

    // Canceled.
    if (!result) return;

    // Backup.
    this->mBackupInfoTable->beginUpdate(1);
    this->mBackupManager->backup(comment);
    this->mBackupInfoTable->endUpdate(1);

    // Notification
    this->mUI->statusBar->showMessage(u8"Backup completed.");
}

void MainWindow::on_pushButton_Remove_clicked()
{
    // Get select index.
    QItemSelectionModel *select = this->mUI->tableView_Backups->selectionModel();
    if (!select->hasSelection()) {
        QMessageBox::warning(this, u8"Warning",
            u8"Select the backup you with to remove.");
        return;
    }
    QModelIndex index = select->currentIndex();
    BackupInfo info = this->mBackupManager->get(index.row());

    // Confirm
    QString message = QString(
        u8"Delete the following backup.\n\n"
        u8" Backup Date: %1\n"
        u8" Backup file: %2\n"
        u8" Comment    : %3\n\n"
        u8"Are you sure?"
    ).arg(info.timestamp).arg(info.path).arg(info.comment);
    const QMessageBox::StandardButton yn = QMessageBox::question(
        this, u8"Remove a backup.", message
    );

    // Canceled (No)
    if (yn != QMessageBox::Yes) return;

    // Remove backup
    this->mBackupInfoTable->beginUpdate(-1, index.row());
    this->mBackupManager->remove(index.row());
    this->mBackupInfoTable->endUpdate(-1);

    // Notification
    this->mUI->statusBar->showMessage(QString("Removed: %1").arg(info.path));
}

void MainWindow::on_pushButton_Restore_clicked()
{
    // Get select index.
    QItemSelectionModel *select = this->mUI->tableView_Backups->selectionModel();
    if (!select->hasSelection()) {
        QMessageBox::warning(this, u8"Warning",
            u8"Select the backup you with to restore.");
        return;
    }
    QModelIndex index = select->currentIndex();
    BackupInfo info = this->mBackupManager->get(index.row());

    // Confirm
    QString message = QString(
        u8"Restore the following backup.\n\n"
        u8" Backup Date: %1\n"
        u8" Backup file: %2\n"
        u8" Comment    : %3\n\n"
        u8"* Current data is automatically backuped.\n\n"
        u8"Are you sure?"
    ).arg(info.timestamp).arg(info.path).arg(info.comment);
    const QMessageBox::StandardButton yn = QMessageBox::question(
        this, u8"Remove a backup.", message
    );

    // Canceled (No)
    if (yn != QMessageBox::Yes) return;

    // Restore backup
    // Note that backup are created in restore function.
    this->mBackupInfoTable->beginUpdate(1);
    this->mBackupManager->restore(index.row());
    this->mBackupInfoTable->endUpdate(1);

    // Notification
    this->mUI->statusBar->showMessage(QString("Restored: %1").arg(info.path));
}

void MainWindow::on_lineEdit_SavePath_textChanged(QString text)
{
    this->mAppConfig->setEldenRingDirPath(text);
}

