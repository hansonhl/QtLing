#include <QApplication>
#include <QMainWindow>

#include <QMenuBar>
#include <QToolBar>
#include <QDebug>

#include "mainwindow.h"
#include "mainmenu.h"

MainMenu::MainMenu(MainWindow *parent) : QObject(parent)
{
    // -------------------------------
    // --- File menu and toolbar
    // -------------------------------

    fileMenu = parent->menuBar()->addMenu(tr("&File"));
    fileToolBar = parent->addToolBar(tr("File"));



//---------------------> --------------------------> -------------------------->
//    QAction * suffix_signature_display_action = new QAction();
//    CLexicon* lexicon = get_lexicon();
//    connect(suffix_signature_display_action, &QAction::triggered, this,  &MainWindow::display_suffix_signatures );

//    QAction * prefix_signature_display_action = new QAction();
//    connect(prefix_signature_display_action, &QAction::triggered, this,  &MainWindow::display_prefix_signatures );
//---------------------> --------------------------> -------------------------->


    // Give a data file name, store the name, and read the file.
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("../../../../QtLing/images/open.png"));
    openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, parent, &MainWindow::ask_for_filename);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // No action associated with this yet.
    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As ..."), this);
    connect(saveAsAct, &QAction::triggered, parent, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    exitAct = new QAction(exitIcon, tr("E&xit"), this);
    connect(exitAct, &QAction::triggered, parent, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    // -------------------------------
    // --- edit menu and toolbar
    // -------------------------------
    editMenu = parent->menuBar()->addMenu(tr("&Edit"));
    editToolBar = parent->addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon("../../../../QtLing/images/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
//    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon("../../../../QtLing/images/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
//    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon("../../../../QtLing/images/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
//    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    parent->menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD
    // -------------------------------
    // --- Help menu
    // -------------------------------
    helpMenu = parent->menuBar()->addMenu(tr("&Help"));
    aboutAct = helpMenu->addAction(tr("&About"), parent, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));


    // beginning of experiment using goldstandard

    // -------------------------------
    // --- Evaluation Menu
    // -------------------------------

    QIcon importIcon;
    QIcon evalIcon;

    importAct = new QAction(tr("Import &Gold Standard XML file"), this);
    importAct->setStatusTip(tr("Import and Load Gold Standard XML file"));
    importAct->setIcon(importIcon);

    evalAct = new QAction(tr("&Evaluate using Gold Standard"), this);
    evalAct->setStatusTip(tr("Evaluate output of linguistica using Gold Standard"));
    evalAct->setIcon(evalIcon);
    evalAct->setDisabled(true);

    connect(importAct, &QAction::triggered, parent, &MainWindow::gs_read_and_parse_xml);
    connect(evalAct, &QAction::triggered, parent, &MainWindow::gs_evaluate);

    evalMenu = parent->menuBar()->addMenu(tr("&Evaluate"));
    evalMenu->addAction(importAct);
    evalMenu->addAction(evalAct);
    editToolBar->addAction(importAct);

    // end of goldstandard experiment

    //fileToolBar->addButton("Sort");
}

void MainMenu::gs_loaded()
{
    // qDebug() << 136 << "MainMenu.cpp: gs_loaded";
    if (!evalAct->isEnabled()) {
        evalAct->setEnabled(true);
    }
}