#include <QPair>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QMultiMap>
#include <QSet>
#include <QtDebug>
#include "Lexicon.h"

#include "SignatureCollection.h"
#include "StemCollection.h"
#include "SuffixCollection.h"
#include "WordCollection.h"
#include "Word.h"

//typedef  QPair<QString, CSignature*>                    sig_and_pointer;
//typedef  QPair<CSignature*, CSignature*>                pair_of_sigs;
//typedef  QPair<QString, pair_of_sigs*>                  multiparse_edge_label;
//typedef  QPair<word_list, labeled_multiparse*>        multiparse;





CLexicon::CLexicon() : m_Words(new CWordCollection), m_Stems(new CStemCollection), m_Suffixes(new CSuffixCollection), m_Signatures(new CSignatureCollection)
{
    m_Parses = new QList<QPair<QString,QString>>();
    m_Protostems = QMap<QString, int>();
    m_RawSignatures =  new CSignatureCollection();
}

QListIterator<sig_tree_edge*> * CLexicon::get_sig_tree_edge_list_iter()
{
    QListIterator<sig_tree_edge*> * iter = new QListIterator<sig_tree_edge*>(m_SigTreeEdgeList);
    return iter;

}


//linguistic methods

void CLexicon::Crab_1()
{
    FindProtostems();
    CreateStemAffixPairs();
    AssignSuffixesToStems();
    qDebug() << "finished making signatures.";
    compute_sig_tree_edges();
    compute_sig_tree_edge_map();
    qDebug() << "finished sorting multiparses";
}

void CLexicon::FindProtostems()
{  QString word, previous_word;
    QStringList * Words =  GetWordCollection()->GetSortedStringArray();
    bool StartFlag = true;
    bool DifferenceFoundFlag = false;
    QString stem;
    for (int wordno=0; wordno<Words->size(); wordno ++){
        word = Words->at(wordno);
        if (StartFlag){
            StartFlag = false;
            previous_word = word;
            continue;
        }
        DifferenceFoundFlag = false;
        int end = qMin(word.length(), previous_word.length());
        
        for (int i=0; i <end; i++){
            if (previous_word[i] != word[i]){
                stem = previous_word.left(i);
                DifferenceFoundFlag = true;
                if (!m_Protostems.contains(stem))
                {
                    m_Protostems[stem] = 1;
                }
                break;
            }
        }
        if (DifferenceFoundFlag == true)
        {
            previous_word = word;
            continue;
        }
        else {
            if (previous_word.length() < word.length()) {
                m_Protostems[previous_word] = 1;
            }
        }
        previous_word = word;
    }
    return;
}

void CLexicon::CreateStemAffixPairs()
{
    QString stem, suffix, word;
    int suffix_length;
    map_string_to_word_iter * word_iter = m_Words->get_iterator();
    while (word_iter->hasNext())   {
        word = word_iter->next().value()->GetWord();
        for (int letterno = 1; letterno < word.length(); letterno++){
            stem = word.left(letterno);
            if (m_Protostems.contains(stem)){
                suffix_length = word.length() - letterno;
                suffix = word.right(suffix_length);
                m_Parses->append(QPair<QString,QString>(stem,suffix));
                if (m_Words->contains(stem)){
                    m_Parses->append(QPair<QString,QString>(stem,QString("NULL")));
                }
            }
        }
    }
}
void   CLexicon::AssignSuffixesToStems()
{
    const int MINIMUM_NUMBER_OF_STEMS = 2;
    QPair<QString,QString> this_pair;
    CSignature* pSig;
    QString stem, affix, signature_string, word;;
    CStem* pStem;
    map_sig_to_morph_set  temp_stems_to_affixes;
    map_sig_to_morph_set  temp_signatures_to_stems;

    for (int parseno = 0; parseno < m_Parses->size(); parseno++){
        this_pair = m_Parses->at(parseno);
        stem = this_pair.first;
        affix = this_pair.second;
        if (! temp_stems_to_affixes.contains(stem)){
            morph_set * pSet = new QSet<QString>;
            temp_stems_to_affixes.insert(stem,pSet);
        }
        temp_stems_to_affixes.value(stem)->insert(affix);
    }
    // Do we need to delete the QSets, or is their memory taken care of? Probably need to delete them...and below.

    QMapIterator<QString, morph_set*>   stem_iter(temp_stems_to_affixes);
    while (stem_iter.hasNext())
    {
         stem = stem_iter.next().key();
         QStringList temp_presignature;
         foreach (affix, *temp_stems_to_affixes.value(stem)){
            temp_presignature.append(affix);           
         }
         temp_presignature.sort();
         QString sig_string = temp_presignature.join("=");
         if ( ! temp_signatures_to_stems.contains(sig_string)){
            morph_set * pStemSet = new QSet<QString>;
            temp_signatures_to_stems.insert(sig_string,pStemSet);
         }
         temp_signatures_to_stems.value(sig_string)->insert(stem);
    }

    // create signatures, stems, affixes:

    QMapIterator<QString, morph_set*> iter_sigstring_to_stems ( temp_signatures_to_stems);
    while (iter_sigstring_to_stems.hasNext())
    {
        signature_string = iter_sigstring_to_stems.next().key();
        QStringList affixes_set = signature_string.split("=");
        if (iter_sigstring_to_stems.value()->size() >= MINIMUM_NUMBER_OF_STEMS)
        {
            QListIterator<QString> affix_iter(affixes_set);
            while(affix_iter.hasNext()){
                  affix = affix_iter.next();
                  m_Suffixes->find_or_add(affix);                   
            }
            pSig = *m_Signatures<< signature_string;
            foreach (stem, *iter_sigstring_to_stems.value()){
                pStem = m_Stems->find_or_add(stem);
                pStem->add_signature (signature_string);
                pSig->add_stem_pointer(pStem);
                QStringList affixes = signature_string.split("=");

                foreach (QString affix,  affixes){
                    if (affix == "NULL"){
                        word = stem;
                    } else{
                        word = stem + affix;
                    }
                    CWord* pWord = m_Words->get_word(word);
                    pWord->add_stem_and_signature(pStem,pSig);
                }
            }
        }else{
            signature_string =  iter_sigstring_to_stems.key();
            *m_RawSignatures << signature_string;
            // We have not done anything with the individual affixes or stems yet.
        }
    }
}

void CLexicon::find_good_signatures_inside_raw_signature(bool FindSuffixesFlag){



}





// Replace parse pairs from current signature structure. This allows us to delete the old signature structure, and replace them with the set of parse-pairs that exactly descreibe the current signature structure
void CLexicon::replace_parse_pairs_from_current_signature_structure(bool FindSuffixesFlag) {
    m_Parses->clear();
    m_Parse_map.clear();
    QString         sig_string;
    CSignature*     pSig;
    CStem*          pStem;
    QList<CStem*> * stem_list;
    QMapIterator<QString,CSignature*>  * sig_iter =  get_signatures()->get_map_iterator();
    while (sig_iter->hasNext()){
        pSig = sig_iter->next().value();
        stem_list =  pSig->get_stems();
    }
}






void CLexicon::compute_sig_tree_edges()
{
    CWord* pWord;
    map_string_to_word * WordMap = m_Words->GetMap();
    QMapIterator<QString,CWord*> word_iter(*WordMap);
    //edge_between_signatures  *  this_edge;
    sig_tree_edge * p_SigTreeEdge;

    int tempcount = 0;

    while (word_iter.hasNext())   {
        pWord = word_iter.next().value();
        int number_of_signatures = pWord->GetSignatures()->size();
        if ( number_of_signatures > 2){
            for (int signo1=0; signo1 < number_of_signatures; signo1++){
                stem_sig_pair* pair1 =  pWord->GetSignatures()->value(signo1);
                CStem * stem1       = pair1->first;
                CSignature* sig1    = pair1->second;
                for (int signo2=signo1 + 1; signo2 < number_of_signatures; signo2++){
                    stem_sig_pair * pair2 = pWord->GetSignatures()->value(signo2);
                    CStem *  stem2       = pair2->first;
                    CSignature* sig2    = pair2->second;
                    if ( stem1->get_key().length() > stem2->get_key().length() ){
                        morph_t difference = stem1->get_key().mid(stem2->get_key().length());
                        p_SigTreeEdge = new sig_tree_edge (sig1,sig2,difference, pWord->get_key());
                    } else{
                        morph_t difference = stem2->get_key().mid(stem1->get_key().length());
                        p_SigTreeEdge =  new sig_tree_edge (sig2,sig1,difference, pWord->get_key());
                    }
                    m_SigTreeEdgeList.append(p_SigTreeEdge);
                    tempcount ++;
                    qDebug() << p_SigTreeEdge->morph << p_SigTreeEdge->sig_1->get_key();
                }
            }
        }
    }
    qDebug() << "Sig tree edge count"<<tempcount;
}











void CLexicon::compute_sig_tree_edge_map() {

morph_t         edge_label;
word_t          this_word;
sig_tree_edge * p_sig_tree_edge,
              * p_sig_tree_edge_2,
              * p_sig_tree_edge_3;
QMap<QString, sig_tree_edge*> * p_EdgeMap = & m_SigTreeEdgeMap;

QListIterator<sig_tree_edge*> sig_tree_edge_iter (m_SigTreeEdgeList);
while (sig_tree_edge_iter.hasNext())
{
    p_sig_tree_edge = sig_tree_edge_iter.next();
    edge_label = p_sig_tree_edge->morph;
    this_word  = p_sig_tree_edge->word;
    if (p_EdgeMap->contains(edge_label)){
        p_sig_tree_edge_3 = p_EdgeMap->value(edge_label);
        p_sig_tree_edge_3->words.append(this_word);
    } else {
        sig_tree_edge * p_sig_tree_edge_2 = new sig_tree_edge(
            p_sig_tree_edge->sig_1,
            p_sig_tree_edge->sig_2,
            p_sig_tree_edge->morph
           );
        p_sig_tree_edge_2->words.append(this_word);
        p_EdgeMap->insert(edge_label, p_sig_tree_edge_2);
    }
}
}













