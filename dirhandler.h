#ifndef DIRHANDLER_H
#define DIRHANDLER_H

#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

class DirHandler : public QThread
{
	Q_OBJECT
	private:
		QString root;


		QStringList dirList(const QString &root);
		quint64		dirSize(const QString &root);
		void run();

	public:
		explicit DirHandler(const QString &root, QObject *parent = 0);

	signals:
		void signalZeroSizeDirs(const QStringList &dirs);
		void signalEmptyDir(const QString &dir);
		void signalCurrentDir(const QString &dir);

	public slots:

};

#endif // DIRHANDLER_H
