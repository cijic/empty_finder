#ifndef DIRHANDLER_H
#define DIRHANDLER_H

#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QStringListIterator>

class DirHandler : public QThread
{
	Q_OBJECT
	private:
		QString root;
		QStringList filters;

		QStringList dirList(const QString &root);
		quint64		dirSize(const QString &root);
		bool		inFilters(const QString &dir);
		void		run();

	public:
		explicit DirHandler(const QString &root, QObject *parent = 0);
		void setFilters(const QStringList &filters);

	signals:
		void signalZeroSizeDirs(const QStringList &dirs);
		void signalEmptyDir(const QString &dir);
		void signalCurrentDir(const QString &dir);

	public slots:

};

#endif // DIRHANDLER_H
