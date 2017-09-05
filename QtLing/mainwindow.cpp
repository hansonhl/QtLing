#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QTreeView>

#include "iostream"
#include <QStandardItemModel>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>
#include <stdio.h>
#include "WordCollection.h"
#include "Word.h"
#include "StemCollection.h"
#include "Stem.h"
#include "SuffixCollection.h"
#include "Suffix.h"
#include "SignatureCollection.h"
#include "Signature.h"

#include <QtWidgets>
#include <QString>
#include <QDebug>
#include <QPair>

#include <QDir>

#include <QSplitter>
#include <QTableView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMapIterator>


MainWindow::MainWindow()
    : tableView(new QTableView),  textEdit(new QPlainTextEdit)
{
    Lexicon = new CLexicon();
    treeModel = new QStandardItemModel();
    tableModel = new QStandardItemModel();
//    createHorizontalGroupBox();
//    setCentralWidget(horizontalGroupBox);
    createSplitter();
    setCentralWidget(mainSplitter);
    createActions();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}
void MainWindow::createSplitter()
{
    mainSplitter = new QSplitter();
    rightSplitter = new QSplitter(Qt::Vertical);

    littleEditor = new QPlainTextEdit;
    littleEditor->setPlainText(tr("To be HTML box."));

    tableView->setModel(tableModel);
    rightSplitter->addWidget(tableView);
    rightSplitter->addWidget(littleEditor);

    treeView = new QTreeView(rightSplitter);
    treeView->setModel(treeModel);

    connect(treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(rowClicked(const QModelIndex&)));
    mainSplitter->addWidget(treeView);
    mainSplitter->addWidget(rightSplitter);
}

void MainWindow::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *layout = new QHBoxLayout;

    verticalGroupBox = new QGroupBox(tr("Vertical layout"));
    QVBoxLayout *vLayout = new QVBoxLayout;
    littleEditor = new QPlainTextEdit;
    littleEditor->setPlainText(tr("To be HTML box."));
    vLayout->addWidget(textEdit);
    vLayout->addWidget(littleEditor);
    verticalGroupBox->setLayout(vLayout);

    QTreeView* tree = new QTreeView(horizontalGroupBox);
    tree->header()->resizeSection(0, 100);
    layout->addWidget(tree);
    layout->addWidget(verticalGroupBox);
    horizontalGroupBox->setLayout(layout);
}
void MainWindow::loadWords()
{
    CWord* word;
    QListIterator<CWord*> iter(Lexicon->GetWordCollection()->GetList());
    int row = 0;

    while (iter.hasNext())
    {
        word = iter.next();
        QStandardItem *item = new QStandardItem(word->GetWord());
        tableModel->setItem(row, 0, item);
        row++;
    }

}
void MainWindow::loadStems()
{
    CStem* stem;
    QMap<QString, CStem*>::iterator iter;
    int row = 0;
    for (iter = Lexicon->GetStemCollection()->GetBegin(); iter != Lexicon->GetStemCollection()->GetEnd(); ++iter)
    {
        stem = iter.value();
        QStandardItem *item = new QStandardItem(stem->GetStem());
        tableModel->setItem(row, 0, item);
        row++;
    }


}
void MainWindow::loadSuffixes()
{
    CSuffix* suffix;
    QListIterator<CSuffix*> iter(Lexicon->GetSuffixCollection()->GetList());
    int row = 0;
    while (iter.hasNext())
    {
        suffix = iter.next();
        QStandardItem *item = new QStandardItem(suffix->GetSuffix());
        tableModel->setItem(row, 0, item);
        row++;
    }

}
void MainWindow::loadSignatures()
{
    CSignature* sig;
    QListIterator<CSignature*> iter(*Lexicon->GetSignatures()->GetSortedSignatures());
    int row = 0;
    while (iter.hasNext())
    {
        sig = iter.next();
        QStandardItem *item = new QStandardItem(sig->GetSignature());
        tableModel->setItem(row, 0, item);
        row++;
    }

}
void MainWindow::loadProtostems()
{
    QMap<CStringSurrogate,int>::iterator iter;

    CStringSurrogate ss;
    int freq;
    QMap<CStringSurrogate,int> protostems = Lexicon->m_Protostems;

    int row = 0;
    for (iter = protostems.begin(); iter != protostems.end(); ++iter)
    {
        ss = iter.key();
        freq = iter.value();
        QString str = tr("%0").arg(freq);
        QStandardItem *itemCol1 = new QStandardItem(ss.Display());
        QStandardItem *itemCol2 = new QStandardItem(str);

        tableModel->setItem(row, 0, itemCol1);
        tableModel->setItem(row, 1, itemCol2);
        row++;
    }

}

void MainWindow::rowClicked(const QModelIndex &index)
{
    QStandardItem *item = treeModel->itemFromIndex(index);
    tableModel->clear();
    QString key = item->text();
    if (key == "Words")
        loadWords();
    else if (key == "Stems")
        loadStems();
    else if (key == "Suffixes")
        loadSuffixes();
    else if (key == "Signatures")
        loadSignatures();
    else if (key == "Protostems")
        loadProtostems();
    else
        qDebug() << "Invalid selection: rowClicked";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}
void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}
void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}
void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}
void MainWindow::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("../../../../QtLing/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("../../../../QtLing/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("../../../../QtLing/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon("../../../../QtLing/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon("../../../../QtLing/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon("../../../../QtLing/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);

    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}
void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}
bool MainWindow::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
void MainWindow::createTreeModel()
{
    QStandardItem *parent = treeModel->invisibleRootItem();
    QStandardItem *wordItem = new QStandardItem(QString("Words"));
    QStandardItem *stemItem = new QStandardItem(QString("Stems"));
    QStandardItem *suffixItem = new QStandardItem(QString("Suffixes"));
    QStandardItem *sigItem = new QStandardItem(QString("Signatures"));
    QStandardItem *prStemItem = new QStandardItem(QString("Protostems"));


    parent->appendRow(wordItem);
    parent->appendRow(stemItem);
    parent->appendRow(suffixItem);
    parent->appendRow(sigItem);
    parent->appendRow(prStemItem);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line.simplified(); // get rid of extra spaces
        QStringList words = line.split(" ");
        QString word = words[0];
        *Lexicon->GetWordCollection() << word;
    }
    Lexicon->GetWordCollection()->sort_word_list();



    Lexicon->Crab_1();
    DisplaySignatures();
    
    createTreeModel();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}
#endif


void MainWindow::DisplaySignatures()
{
    CSignature* pSig;
    Lexicon->GetSignatures()->sort();
    QList<CSignature*>* pSortedSignatures = Lexicon->GetSignatures()->GetSortedSignatures();

    QListIterator<CSignature*> sig_iter (*pSortedSignatures);
    while (sig_iter.hasNext()){
       pSig = sig_iter.next();
       textEdit->appendPlainText(pSig->display());
       textEdit->appendPlainText(pSig->display_stems()  );

    }

    
}
