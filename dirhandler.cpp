#include "dirhandler.h"

QStringList DirHandler::dirList(const QString &root)
{
	QStringList dirs;
	QFileInfo dirInfo(root);
	if (dirInfo.isDir())
	{
		QDir dir(root);
		QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir())
			{
				const QString absPath = fileInfo.absoluteFilePath();
				dir.setPath(absPath);

				// Если в папке содержатся файлы, значит она не пустая.
				// Если же файлов нет, то можно передавать её на дальнейшую обработку.
				// Так отсеим заранее часть непустых папок.
				if (!dir.entryInfoList(QDir::Files).size())
				{
					dirs += absPath;
				}

				dirs += this->dirList(absPath);
			}
		}
	}

//	qDebug() << __FILE__ << __LINE__ << dirs;
	return dirs;
}

quint64 DirHandler::dirSize(const QString &root)
{
	quint64 size = 0;
	QFileInfo dirInfo(root);
	if (dirInfo.isDir())
	{
		QDir dir(root);
		QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs |  QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir())
			{
				size += this->dirSize(fileInfo.absoluteFilePath());
			}
			else
			{
				size += fileInfo.size();
			}
		}
	}

	qDebug() << __FILE__ << __LINE__ << root << size;
	return size;
}

void DirHandler::run()
{
	qDebug() << __FILE__ << __LINE__ << this->root;
	QStringList result;
	QStringList dirs = this->dirList(this->root);
	for (int i = 0; i < dirs.size(); i++)
	{
		QString d = dirs.at(i);
		// Если размер папки равен нулю.
		if (!this->dirSize(d))
		{
			result += d;
		}
	}

	emit this->signalZeroSizeDirs(result);
}

DirHandler::DirHandler(const QString &root, QObject *parent) :
	QThread(parent)
{
	this->root = root;
}
