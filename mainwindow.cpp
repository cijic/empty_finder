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
	connect(this->ui->stopBtn, SIGNAL(released()), SLOT(onBtnStopClicked()));
	connect(this->ui->delBtn, SIGNAL(released()), SLOT(onBtnDelClicked()));
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
	this->ui->stopBtn->setEnabled(true);
	this->ui->delBtn->setEnabled(false);

	QString root = this->ui->lineEdit->text();
	QDir rootDir(root);
	if (rootDir.exists())
	{
		this->dh = new DirHandler(root);

		QStringList blockingFilters;
		blockingFilters.append(this->ui->filtersLineEdit->text().split(";"));
		this->dh->setFilters(blockingFilters);
		connect(this->dh, SIGNAL(signalZeroSizeDirs(QStringList)), SLOT(slotFillTree(QStringList)));
		connect(this->dh, SIGNAL(signalEmptyDir(QString)), SLOT(slotFillTree(QString)));
		connect(this->dh, SIGNAL(signalCurrentDir(QString)), SLOT(slotCurrentDir(QString)));

		this->dh->start();
	}
}

/**
 * @brief MainWindow::onBtnDelClicked - Delete button handler.
 */
void MainWindow::onBtnDelClicked()
{
	this->ui->scanBtn->setEnabled(false);
	this->ui->stopBtn->setEnabled(false);

	QTreeWidgetItemIterator it(this->ui->treeWidget);
	while (*it)
	{
		if ((*it)->checkState(0) == Qt::Checked)
		{
			QString path = (*it)->text(0);
			QDir dir(path);
			if (dir.exists())
			{
				// Если папка удачно удалена, то
				if (dir.rmpath(path))
				{
					// Удаляем папку из списка.
					delete (*it);
				}
				else
				{
					qDebug() << __FILE__ << __LINE__ << "Folder " + path + " wasn't removed.";
				}
			}
		}

		it++;
	}

	if (!this->ui->treeWidget->topLevelItemCount())
	{
		this->ui->stopBtn->setEnabled(false);
		this->ui->delBtn->setEnabled(false);
	}
	this->ui->scanBtn->setEnabled(true);
}

/**
 * @brief MainWindow::onBtnStopClicked - Stop button handler.
 */
void MainWindow::onBtnStopClicked()
{
	this->ui->stopBtn->setEnabled(false);
	this->dh->terminate();
}

/**
 * @brief MainWindow::slotFillTree - Fill tree with data, if it empty.
 * @param dirs - List of dirs.
 */
void MainWindow::slotFillTree(const QStringList &dirs)
{
	this->ui->statusBar->showMessage("Search finished.");
	this->ui->stopBtn->setEnabled(false);

	// If empty dirs were found
	if (!this->ui->treeWidget->topLevelItemCount())
	{
		// Fill tree with dir list.
		QList<QTreeWidgetItem *> items;
		for (int i = 0; i < dirs.size(); i++)
		{
			items.append(this->addCheckableItem(dirs.at(i)));
		}
		this->ui->treeWidget->addTopLevelItems(items);
	}

	if (this->ui->treeWidget->topLevelItemCount())
	{
		// Enable delete button.
		this->ui->delBtn->setEnabled(true);
	}
}

/**
 * @brief MainWindow::slotFillTree - Add element to tree.
 * @param dir - Dirname.
 */
void MainWindow::slotFillTree(const QString &dir)
{
	this->ui->treeWidget->addTopLevelItem(this->addCheckableItem(dir));
}

/**
 * @brief MainWindow::slotCurrentDir - Slot on signal, when checking dir.
 * @param dir - Dirname.
 */
void MainWindow::slotCurrentDir(const QString &dir)
{
	this->ui->statusBar->showMessage("Checking: " + dir);
}
