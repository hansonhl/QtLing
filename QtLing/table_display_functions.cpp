#include <QStandardItemModel>

#include "mainwindow.h"
#include "graphics.h"
#include "lxamodels.h"

/**
 * @brief MainWindow::display_suffix_signatures
 * This is called by a QAction.
 */
void MainWindow::display_suffixes()
{
    m_tableView_upper_left->setModel(m_Models["Suffixes"]);
    m_tableView_upper_left->set_data_type ( e_data_suffixes );
    m_tableView_upper_left->resizeColumnsToContents();
 }

/**
 * @brief MainWindow::display_epositive_suffix_signatures
 */
void MainWindow::display_epositive_suffix_signatures()
{
    if (m_Models["EPositive signatures"]->rowCount() > 0){
        m_tableView_upper_left->setModel(m_Models["EPositive signatures"]);
        m_tableView_upper_left->set_data_type(e_data_epositive_suffixal_signatures );
        m_tableView_upper_left->set_data_type(e_data_epositive_suffixal_signatures);

        m_current_graphics_scene->clear_all();
        m_current_graphics_scene->assign_lattice_positions_to_signatures(get_lexicon()->get_signatures(), e_data_epositive_suffixal_signatures);
        m_current_graphics_scene->create_and_place_signatures();
    }
}

/**
 * @brief MainWindow::display_suffix_signatures
 * This is called by a QAction.
 */
void MainWindow::display_suffix_signatures()
{

    m_tableView_upper_left->setModel(m_Models["Signatures"]);
    m_tableView_upper_left->set_data_type( e_data_suffixal_signatures );
    m_tableView_upper_left->resizeColumnsToContents();
    m_tableView_upper_left->set_data_type( e_data_suffixal_signatures);

    CLexicon* lexicon = get_lexicon();
    m_tableView_upper_right->setModel(m_Models["Signatures 2"]);
    m_tableView_upper_right->set_data_type( e_data_suffixal_signatures );
    m_tableView_upper_left->set_data_type( e_data_suffixal_signatures );
    m_tableView_upper_right->resizeColumnsToContents();


    //lexicon->dump_signatures_to_debug();

     m_current_graphics_scene->clear_all();
     m_current_graphics_scene->assign_lattice_positions_to_signatures(lexicon->get_signatures(), e_data_suffixal_signatures);
     m_current_graphics_scene->create_and_place_signatures();




}



/**
 * @brief MainWindow::display_prefix_signatures
 * This is called by a QAction.
 */
void MainWindow::display_prefix_signatures()
{   CLexicon* lexicon = get_lexicon();

    m_tableView_upper_left->setModel(m_Models["Prefix signatures"]);
    m_tableView_upper_left->set_data_type ( e_data_prefixal_signatures );
    m_tableView_upper_left->resizeColumnsToContents();

    m_tableView_upper_right->setModel(m_Models["Prefix signatures 2"]);
    m_tableView_upper_right->set_data_type ( e_data_prefixal_signatures );
    m_tableView_upper_right->resizeColumnsToContents();

    m_current_graphics_scene->clear_all();
    m_current_graphics_scene->assign_lattice_positions_to_signatures(lexicon->get_prefix_signatures(), e_data_prefixal_signatures);
    m_current_graphics_scene->create_and_place_signatures();
}



/**
 * @brief MainWindow::display_hypotheses
 * This is called by a QAction.
 */
void MainWindow::display_hypotheses()
{    

    m_tableView_upper_left->setModel(m_Models["Hypotheses"]);
    m_tableView_upper_left->set_data_type ( e_data_hypotheses );
    m_tableView_upper_left->resizeColumnsToContents();

    m_tableView_upper_right->setModel(m_Models["Hypotheses 2"]);
    m_tableView_upper_right->set_data_type ( e_data_hypotheses );
    m_tableView_upper_right->resizeColumnsToContents();


}
