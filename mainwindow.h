#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextCodec>
#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>

#include "dirhandler.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private:
		Ui::MainWindow *ui;
		DirHandler *dh;

		QTreeWidgetItem *addCheckableItem(const QString &title);
		QFileInfoList findDirs(const QString &root);
		QStringList dirList(const QString &root);
		void handleDirList(const QString &root);
		void getAsyncDirList(const QString &root);
		quint64 dirSize(const QString &root);
		void handleDirSize(const QString &root);

	signals:
		void signalDirSize(const QString &root, quint64 size);
		void signalDirList(const QString &root, const QStringList &list = QStringList());
		void signalDirHanlder(const QString &dir);

	private slots:
		void onBtnBrowseClicked();
		void onBtnScanClicked();
		void onBtnDelClicked();
		void slotOnDirSize(const QString &root, quint64 size);
		void slotOnDirList(const QString &root, const QStringList &list);
		void slotOnDirHandler(const QString &root);
		void slotFillTree(const QStringList &dirs);
};

#endif // MAINWINDOW_H
