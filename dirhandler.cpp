#include "dirhandler.h"

/**
 * @brief DirHandler::dirList - Get zero by size subfolders for the root dir.
 * @param root - Root.
 * @return - Zero by size subfolders.
 */
QStringList DirHandler::dirList(const QString &root)
{
	QStringList dirs;
	QFileInfo dirInfo(root);
	if (dirInfo.isDir())
	{
		emit this->signalCurrentDir(root);
		QDir dir(root);
		QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir() && !fileInfo.fileName().startsWith("."))
			{
				const QString absPath = fileInfo.absoluteFilePath();

				// If dirname is in blocking filters.
				if (this->inFilters(absPath))
				{
					continue;
				}
				dir.setPath(absPath);

				// Если в папке содержатся файлы, значит она не пустая.
				// Если же файлов нет, то можно передавать её на дальнейшую обработку.
				// Так отсеем заранее часть непустых папок.
				if (!dir.entryInfoList(QDir::Files).size())
				{
					// Если папка является кандидатом в число 0 по размеру, то проверяем так ли это.
					if (!this->dirSize(absPath))
					{
						// Если да, то добавляем её в список таковых.
						dirs += absPath;

						// И сигнализируем об этом.
						emit this->signalEmptyDir(absPath);
					}
				}
				else
				{
					dirs += this->dirList(absPath);
				}
			}
		}
	}

	return dirs;
}

/**
 * @brief DirHandler::dirSize - Get size of folder.
 * @param root - Root.
 * @return - Size of folder.
 */
quint64 DirHandler::dirSize(const QString &root)
{
	quint64 size = 0;
	QFileInfo dirInfo(root);
	if (dirInfo.isDir())
	{
		emit this->signalCurrentDir(root);
		QDir dir(root);
		QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir() && !fileInfo.fileName().startsWith("."))
			{
				size += this->dirSize(fileInfo.absoluteFilePath());
			}
			else
			{
				size += fileInfo.size();
			}
		}
	}

	return size;
}

/**
 * @brief DirHandler::inFilters - Check if dirname is in blocked filters.
 * @param dir - Dir name.
 * @return - true if in, false if else.
 */
bool DirHandler::inFilters(const QString &dir)
{
	for (int i = 0; i < this->filters.size(); i++)
	{
		QRegExp regExp(this->filters.at(i), Qt::CaseInsensitive);
		if (dir.contains(regExp))
		{
			return true;
		}
	}

	return false;
}

void DirHandler::run()
{
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

void DirHandler::setFilters(const QStringList &filters)
{
	this->filters = filters;
	this->filters.removeDuplicates();

	QList<QString>::iterator i;
	for (i = this->filters.begin(); i != this->filters.end(); i++)
	{
		if ((*i).isEmpty())
		{
			this->filters.removeOne((*i));
		}
		(*i).replace(".", "\\.");
		(*i).replace("*", ".*");
	}
}
