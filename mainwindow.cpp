#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	ui->setupUi(this);

	// Отключить отображение заголовка.
	this->ui->treeWidget->header()->close();

	connect(this->ui->pushButton, SIGNAL(released()), SLOT(onBtnBrowseClicked()));
	connect(this->ui->scanBtn, SIGNAL(released()), SLOT(onBtnScanClicked()));
	connect(this, SIGNAL(signalDirHanlder(QString)), SLOT(slotOnDirHandler(QString)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

/**
 * @brief MainWindow::addCheckableItem - Создать элемент для дерева с галочками.
 * @param title - Текст элемента дерева.
 * @return - Элемент дерева.
 */
QTreeWidgetItem *MainWindow::addCheckableItem(const QString &title)
{
	QTreeWidgetItem *item = new QTreeWidgetItem();
	item->setText(0, title);
	item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	item->setCheckState(0, Qt::Checked);

	return item;
}

/**
 * @brief MainWindow::findDirs - Найти папки.
 * @param root - Папка, в которой искать.
 */
QFileInfoList MainWindow::findDirs(const QString &root)
{
	QDir dir(root);
	QFileInfoList dirs;
	QString types;
	if (types.trimmed().isEmpty())
	{
		types = "*";
	}

	dirs = dir.entryInfoList(QStringList(types), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::Name);

	return dirs;
}

QStringList MainWindow::dirList(const QString &root)
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
				dirs += fileInfo.absoluteFilePath();
				dirs += this->dirList(fileInfo.absoluteFilePath());
			}
		}
	}

	return dirs;
}

void MainWindow::handleDirList(const QString &root)
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
				dirs += fileInfo.absoluteFilePath();
				dirs += this->dirList(fileInfo.absoluteFilePath());
			}
		}
	}

	emit this->signalDirList(root, dirs);
}

void MainWindow::getAsyncDirList(const QString &root)
{
	QFileInfo dirInfo(root);
	if (dirInfo.isDir())
	{
		QDir dir(root);
		QFileInfoList dirInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < dirInfoList.size(); i++)
		{
			dirInfo.setFile(dirInfoList.at(i).absoluteFilePath());
			if (dirInfo.isDir())
			{
				QString absPath = dirInfo.absoluteFilePath();
				dir.setPath(absPath);

				// Если в папке содержатся файлы, значит она не пустая.
				// Если же файлов нет, то можно передавать её на дальнейшую обработку.
				// Так отсеим заранее часть непустых папок.
				if (!dir.entryInfoList(QDir::Files).size())
				{
					emit this->signalDirHanlder(absPath);
				}

				this->getAsyncDirList(absPath);
			}
		}
	}
}

quint64 MainWindow::dirSize(const QString &root)
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

	return size;
}

void MainWindow::handleDirSize(const QString &root)
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

	emit this->signalDirSize(root, size);
}

/**
 * @brief MainWindow::onBtnBrowseClicked - Обработчик нажатия на кнопку "Browse".
 */
void MainWindow::onBtnBrowseClicked()
{
	QString curDir = this->ui->lineEdit->text();
	QString choosenFolder = QFileDialog::getExistingDirectory(this, "Choose folder").trimmed();
	if (!choosenFolder.isEmpty())
	{
		this->ui->lineEdit->setText(choosenFolder);
		curDir = choosenFolder;
	}

	// Если выбранная папка не существует, то подставляем пользовательскую папку.
	if (!QFile::exists(curDir))
	{
		this->ui->lineEdit->setText(QDir::homePath());
	}
}

/**
 * @brief MainWindow::onBtnScanClicked - Запустить поиск пустых папок.
 */
void MainWindow::onBtnScanClicked()
{
	this->ui->statusBar->showMessage("Search started.");

	this->ui->treeWidget->clear();

	QString root = this->ui->lineEdit->text();
	QDir rootDir(root);
	if (rootDir.exists())
	{
//		this->getAsyncDirList(root);
		this->dh = new DirHandler(root);
		connect(this->dh, SIGNAL(signalZeroSizeDirs(QStringList)), SLOT(slotFillTree(QStringList)));

		this->dh->start();
	}

	this->ui->statusBar->showMessage("Search finished.");
}

void MainWindow::onBtnDelClicked()
{

}

void MainWindow::slotOnDirSize(const QString &root, quint64 size)
{
	if (!size)
	{
		this->ui->treeWidget->addTopLevelItem(this->addCheckableItem(root));
	}
	else
	{
		qDebug() << __FILE__ << __LINE__ << "Папка " + root + " весит " + QString::number(size) + " байт.";
	}
}

void MainWindow::slotOnDirList(const QString &root, const QStringList &list)
{
	this->ui->statusBar->showMessage("Checking: " + root);
	connect(this, SIGNAL(signalDirSize(QString,quint64)), SLOT(slotOnDirSize(QString,quint64)));
	//	this->handleDirSize(root);
}

void MainWindow::slotOnDirHandler(const QString &root)
{
	this->ui->treeWidget->addTopLevelItem(this->addCheckableItem(root));
	this->ui->statusBar->showMessage("Checking: " + root);

//	connect(this, SIGNAL(signalDirSize(QString,quint64)), SLOT(slotOnDirSize(QString,quint64)));
	//	this->handleDirSize(root);
}

void MainWindow::slotFillTree(const QStringList &dirs)
{
	QList<QTreeWidgetItem *> items;
	for (int i = 0; i < dirs.size(); i++)
	{
		items.append(this->addCheckableItem(dirs.at(i)));
	}
	this->ui->treeWidget->addTopLevelItems(items);
}
