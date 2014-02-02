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

	signals:

	private slots:
		void onBtnBrowseClicked();
		void onBtnScanClicked();
		void onBtnDelClicked();
		void onBtnStopClicked();
		void slotFillTree(const QStringList &dirs);
		void slotFillTree(const QString &dir);
		void slotCurrentDir(const QString &dir);
};

#endif // MAINWINDOW_H
