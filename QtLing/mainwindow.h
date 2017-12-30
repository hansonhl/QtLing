#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QMap>
#include <QList>
#include <QObject>
#include <QTreeView>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QGraphicsView>
#include "Lexicon.h"

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QPlainTextEdit;
class QSessionManager;
class QSplitter;
class QStandardItemModel;
class QTextEdit;
class QTreeView;
class MainWindow;
class lxa_graphics_view;
class lxa_graphics_scene;
QT_END_NAMESPACE

class lxaWindow : public QWidget
{   QList<QList<CSignature*>*>  m_signature_lattice;
    MainWindow *    m_main_window;
    double          m_scale;
    double          m_xshift;
    double          m_yshift;

public:
    lxaWindow( MainWindow * );
    virtual void    paintEvent(QPaintEvent * );
    void            drawSignatures(QPainter& painter);
    void            assign_scene_positions_to_signatures(CSignatureCollection* );
    void            expand();
    void            contract();
    void            move_down();
    void            move_up();
    void            reset_scale_and_translation();
};

class LxaStandardItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    LxaStandardItemModel();
    LxaStandardItemModel(QString shortname);
    ~LxaStandardItemModel();

private:
    QString                 m_ShortName;
    QString                 m_Description;
    eSortStyle              m_sort_style;
    CSignatureCollection*   m_Signatures;

public:

    void        sort(int column_no, Qt::SortOrder order);
    void        load_words(CWordCollection*);
    void        load_stems(CStemCollection * p_stems);
    void        load_suffixes(CSuffixCollection * p_suffixes);
    void        load_signatures(CSignatureCollection * p_signatures, eSortStyle = SIG_BY_STEM_COUNT);
    void        load_parasignatures(CSignatureCollection * p_signatures);
    void        sort_signatures(eSortStyle);
    void        load_sig_graph_edges(QMap<QString, sig_graph_edge*> *);
    void        load_passive_signatures (CSignatureCollection* p_passive_signatures);
    void        load_positive_signatures(CSignatureCollection* p_positive_signatures);
    void        load_positive_prefix_signatures(CSignatureCollection* p_positive_prefix_signatures);
    void        load_hypotheses(QList<CHypothesis*>*);
// add component 11
};


class UpperTableView : public QTableView
{
    Q_OBJECT
    QString         m_content;
    MainWindow*     m_parent_window;
    eDocumentType   m_document_type;
    eSortStyle      m_signature_sort_style;
public:
    UpperTableView ();
    UpperTableView (MainWindow*, eSortStyle = DEFAULT);
    QString         get_content()                   {return m_content;}
    void            set_content_type(QString text)  {m_content = text;}
    MainWindow*     get_parent_window()             {return m_parent_window;}
    eDocumentType   get_document_type()             {return m_document_type;}
    void            set_document_type(eDocumentType type) {m_document_type = type;}
public slots:
    void ShowModelsUpperTableView(const QModelIndex& );
    eSortStyle      get_signature_sort_style () { return m_signature_sort_style;}
    void            set_signature_sort_style (eSortStyle style) {m_signature_sort_style = style;}
    signals:
    void please_display_this_signature(QString sig);
};


class LowerTableView : public QTableView
{
    Q_OBJECT
    MainWindow*             m_parent_window;
    int                     m_number_of_columns;
    QStandardItemModel*     m_my_current_model;
    CLexicon *              m_lexicon;
    int                     m_current_sortstyle;
    int                     m_number_of_sortstyles;
public:
    LowerTableView();
    LowerTableView(MainWindow*);
    //CLexicon * p_lexicon;
    MainWindow* get_parent_window() {return m_parent_window;}
    void change_current_model(QStandardItemModel*);
public slots:
        void display_this_item(const QModelIndex&);
        void table_signature(CSignature*) ;
        void table_word (CWord*);
        void table_one_signature(CSignature*, QStringList stem_list);
        void table_passive_signature(CSignature*);
        void graphics_sig_graph_edges(CSignature* , CLexicon*);
        int  get_number_of_columns () {return m_number_of_columns;}
        CLexicon* get_lexicon() {return m_lexicon;}
        //void    advance_sortstyle() {m_sort}

signals:
};


class LeftSideTreeView : public QTreeView
{
    Q_OBJECT

    MainWindow * m_parent_window;

public:
    LeftSideTreeView(MainWindow*  window);
    //void set_parent(MainWindow* window) {m_parent_window = window;}
    void rowClicked(const QModelIndex& index);
};

//-----------------------------------------------------------------//
class MainWindow : public QMainWindow
//-----------------------------------------------------------------//
{
    Q_OBJECT
    friend class                            LeftSideTreeView;
    friend class                            UpperTableView;
    friend class                            LowerTableView;
    friend class                            lxaWindow;

    QList<CLexicon*>                        m_lexicon_list;
    QString                                 m_name_of_data_file;
    QProgressBar *                          m_ProgressBar;
    QMap<QString, LxaStandardItemModel*>    m_Models;
    eDisplayType                            m_graphic_display_type;

    QSplitter *                             m_mainSplitter;
    QSplitter *                             m_rightSplitter;
    QSplitter *                             m_top_rightSplitter;
    UpperTableView *                        m_tableView_upper_left;
    UpperTableView *                        m_tableView_upper_right;
    LowerTableView *                        m_tableView_lower;
    LeftSideTreeView *                      m_leftTreeView;
    lxa_graphics_scene *                    m_graphics_scene;
    lxa_graphics_view *                     m_graphics_view;
    bool                                    m_graphic_display_flag;
    QStandardItemModel *                    m_treeModel;

    QGroupBox *                             horizontalGroupBox;
    QGroupBox *                             verticalGroupBox;
    QString                                 curFile;



public:
    MainWindow();

    void        display_suffix_signatures();
    void        DisplaySignatures();
    CLexicon*   get_lexicon() {return m_lexicon_list.last();  }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void read_file_do_crab();
    void do_crab2();
    void do_crab();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void print_prefix_signatures();

#ifndef QT_NO_SESSIONMANAGER
    void                    commitData(QSessionManager &);
#endif

private:

    void createHorizontalGroupBox();
    void createSplitter();
    void ask_for_filename();
    void read_dx1_file();
    void read_text_file();

//          Qt-style modesl
    void                    load_stem_model();
    void                    load_affix_model();
    void                    load_signature_model();
    void                    load_protostem_model();
    void                    load_multiparse_model();
    void                    load_raw_signature_model();
    void                    load_sig_tree_edge_model();
    void                    load_residual_signature_model();
    void                    load_singleton_signature_model();
    void                    load_singleton_stem_model();
    void                    load_StemSet2_model();
    void                    load_subsignature_model();
    void                    load_word_model();
    void                    createTreeModel();

    void                    sort_upper_table();

//          windows
    void                    createActions();
    void                    createStatusBar();

//          settings
    void                    readSettings();
    void                    writeSettings();

//          working with files
    bool                    ask_to_save();
    bool                    saveFile(const QString &fileName);
    void                    setCurrentFile(const QString &fileName);
    QString                 strippedName(const QString &fullFileName);
    void                    cycle_through_graphic_displays();

    lxa_graphics_view*      get_graphics_view()     {return  m_graphics_view;}
    lxa_graphics_scene*     get_graphics_scene()    {return m_graphics_scene;}

protected:
   void keyPressEvent(QKeyEvent *e);

};



#endif
