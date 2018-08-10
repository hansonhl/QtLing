#include "Lexicon.h"
#include "Suffix.h"
#include "SuffixCollection.h"
#include "Stem.h"
#include "StemCollection.h"
#include "Signature.h"
#include "SignatureCollection.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

void CLexicon::test_json_functionality()
{
    qDebug() << "testing json functionality";
    assign_json_id();
    QJsonObject test_object;
    /*
    if (m_SuffixesFlag)
        m_Suffixes->write_json(test_object);
    else
        m_Prefixes->write_json(test_object);
    */
    /*
    if (m_SuffixesFlag)
        m_suffixal_stems->write_json(test_object);
    else
        m_prefixal_stems->write_json(test_object);
    */
    if (m_SuffixesFlag)
        m_Signatures->write_json(test_object);
    else
        m_PrefixSignatures->write_json(test_object);

    QString filename("../test_json_stems.json");
    QFile test_json_file(filename);
    if (!test_json_file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file: " << filename;
    } else {
        QJsonDocument test_json_doc(test_object);
        test_json_file.write(test_json_doc.toJson());
    }
    qDebug() << "finished testing json functionality";
}

bool CJsonInfo::check_tags(const QJsonObject& ref_json,
                                  const QList<QPair<QJsonValue::Type, QString> >& tags)
{
    QPair<QJsonValue::Type, QString> pair;
    foreach (pair, tags) {
        QJsonObject::const_iterator iter = ref_json.find(pair.second);
        if (iter == ref_json.end())
            return false;
        if (iter->type() != pair.first)
            return false;
    }
    return true;
}

QString CJsonInfo::eSortStyle_to_string(const eSortStyle sortstyle)
{
    switch (sortstyle) {
    case UNSPECIFIED: return "unspecified";
    case DEFAULT: return "default";
    case KEY: return "key";
    case REVERSE_KEY: return "reverseKey";
    case CORPUSCOUNT: return "corpusCount";
    case COUNT: return "count";
    case SIGS: return "sigs";
    case SIG_BY_STEM_COUNT: return "sigByStemCount";
    case SIG_BY_WORD_COUNT: return "sigByWordCount";
    case SIG_BY_AFFIX_COUNT: return "sigByAffixCount";
    case PARSE_SIZE: return "parseSize";
    case RULE_USE: return "ruleUse";
    case LENGTH: return "length";
    case DL_SAVINGS: return "DLSavings";
    case FREQUENCY: return "frequency";
    case MORPHEMECOUNT: return "morphemeCount";
    case MORPHEME_GOODNESS: return "morphemeGoodness";
    case STEM_SUFFIXES: return "stemSuffixes";
    case USECOUNT: return "useCount";
    case SIG_REMARK: return "sigRemark";
    case STEM_SOURCE: return "stemSource";
    case SIG_MENTORS: return "sigMentors";
    case TEMPLATE_SORT: return "templateSort";
    case TEMPLATE_SORT_COLUMN: return "templateSortColumn";
    default: return "default";
    }
}

eSortStyle CJsonInfo::string_to_eSortStyle(const QString &str)
{
    if (str == "unspecified") return UNSPECIFIED;
    else if (str == "default") return DEFAULT;
    else if (str == "key") return KEY;
    else if (str == "reverseKey") return REVERSE_KEY;
    else if (str == "corpusCount") return CORPUSCOUNT;
    else if (str == "count") return COUNT;
    else if (str == "sigs") return SIGS;
    else if (str == "sigByStemCount") return SIG_BY_STEM_COUNT;
    else if (str == "sigByWordCount") return SIG_BY_WORD_COUNT;
    else if (str == "sigByAffixCount") return SIG_BY_AFFIX_COUNT;
    else if (str == "parseSize") return PARSE_SIZE;
    else if (str == "ruleUse") return RULE_USE;
    else if (str == "length") return LENGTH;
    else if (str == "DLSavings") return DL_SAVINGS;
    else if (str == "frequency") return FREQUENCY;
    else if (str == "morphemeCount") return MORPHEMECOUNT;
    else if (str == "morphemeGoodness") return MORPHEME_GOODNESS;
    else if (str == "stemSuffixes") return STEM_SUFFIXES;
    else if (str == "useCount") return USECOUNT;
    else if (str == "sigRemark") return SIG_REMARK;
    else if (str == "stemSource") return STEM_SOURCE;
    else if (str == "sigMentors") return SIG_MENTORS;
    else if (str == "templateSort") return TEMPLATE_SORT;
    else if (str == "templateSortColumn") return TEMPLATE_SORT_COLUMN;
    else return DEFAULT;
}

/******************************************/
/*                 LEXICON                */
/******************************************/
void CLexicon::assign_json_id()
{
    if (m_SuffixesFlag) {
        m_Suffixes->assign_json_id();
        m_suffixal_stems->assign_json_id();
        m_Signatures->assign_json_id();
    } else {
        m_Prefixes->assign_json_id();
        m_prefixal_stems->assign_json_id();
        m_PrefixSignatures->assign_json_id();
    }
}


/******************************************/
/*          SUFFIXES and PREFIXES         */
/******************************************/

void CSuffix::write_json(QJsonObject &ref_json) const
{
    ref_json["suffix"] = m_key;
    ref_json["frequency"] = m_frequency;
    ref_json["count"] = m_sig_count;
    ref_json["id"] = m_json_id;
}


void CSuffix::read_json(const QJsonObject &ref_json)
{
    CJsonInfo::JsonTagList tag_list =
    {
        QPair<QJsonValue::Type, QString>(QJsonValue::String, "suffix"),
        QPair<QJsonValue::Type, QString>(QJsonValue::Double, "frequency"),
        QPair<QJsonValue::Type, QString>(QJsonValue::Double, "count"),
        QPair<QJsonValue::Type, QString>(QJsonValue::Double, "id")
    };
    if (!CJsonInfo::check_tags(ref_json, tag_list)) {
        qDebug() << "Invalid json object read!";
        return;
    }
    m_key = ref_json["suffix"].toString();
    m_frequency = ref_json["frequency"].toInt();
    m_sig_count = ref_json["count"].toInt();
    m_json_id = ref_json["id"].toInt();
}

void CPrefix::write_json(QJsonObject &ref_json) const
{
    ref_json["prefix"] = m_key;
    ref_json["frequency"] = m_frequency;
    ref_json["count"] = m_sig_count;
    ref_json["id"] = m_json_id;
}

void CPrefix::read_json(const QJsonObject &ref_json)
{
    m_key = ref_json["prefix"].toString();
    m_frequency = ref_json["frequency"].toInt();
    m_sig_count = ref_json["count"].toInt();
    m_json_id = ref_json["id"].toInt();
}


/******************************************/
/*        SUFFIX/PREFIX COLLECTION        */
/******************************************/

void CSuffixCollection::assign_json_id()
{
    int curr_json_id = 0;
    foreach (CSuffix* p_suffix, m_SuffixMap) {
        p_suffix->set_json_id(curr_json_id++);
    }
}

void CSuffixCollection::write_json(QJsonObject &ref_json)
{
    QJsonArray arr_suffixes;
    foreach (CSuffix* p_suffix, m_SuffixMap) {
        QJsonObject json_suffix;
        p_suffix->write_json(json_suffix);
        arr_suffixes.append(json_suffix);
    }

    QJsonArray arr_sorted_suffixes;
    foreach (CSuffix* p_suffix, m_SortedList) {
        arr_sorted_suffixes.append(p_suffix->get_json_id());
    }
    ref_json["count"] = m_SuffixMap.size();
    ref_json["suffixes"] = arr_suffixes;
    ref_json["sortedSuffixes"] = arr_sorted_suffixes;
}

void CSuffixCollection::read_json_1(const QJsonObject &ref_json, CJsonInfo& ref_pointers)
{
    int count = ref_json["count"].toInt();
    m_SuffixMap.clear();
    m_SortedList.clear();
    m_SortedList.reserve(count);
    QList<CSuffix*>& id_list = ref_pointers.suffixes;
    id_list.clear();
    id_list.reserve(count);
    const QJsonArray& ref_suffixes = ref_json["suffixes"].toArray();
    foreach (QJsonValue val_suffix, ref_suffixes) {
        const QJsonObject& obj_suffix = val_suffix.toObject();
        const QString& str_suffix = obj_suffix["suffix"].toString();
        CSuffix* p_suffix = new CSuffix(str_suffix);
        p_suffix->read_json(obj_suffix);
        m_SuffixMap.insert(str_suffix, p_suffix);
        id_list[obj_suffix["id"].toInt()] = p_suffix;
    }
    sort_by_count();
}

void CPrefixCollection::assign_json_id()
{
    int curr_json_id = 0;
    foreach (CPrefix* p_prefix, m_PrefixMap) {
        p_prefix->set_json_id(curr_json_id++);
    }
}

void CPrefixCollection::write_json(QJsonObject &ref_json)
{
    ref_json["count"] = m_PrefixMap.size();

    QJsonArray arr_prefixes;
    foreach (CPrefix* p_prefix, m_PrefixMap) {
        QJsonObject json_prefix;
        p_prefix->write_json(json_prefix);
        arr_prefixes.append(json_prefix);
    }
    QJsonArray arr_sorted_prefixes;
    foreach (CPrefix* p_prefix, m_SortedList) {
        arr_sorted_prefixes.append(p_prefix->get_json_id());
    }

    ref_json["prefixes"] = arr_prefixes;
    ref_json["sortedPrefixes"] = arr_sorted_prefixes;
}

void CPrefixCollection::read_json_1(const QJsonObject &ref_json, CJsonInfo& ref_pointers)
{
    int count = ref_json["count"].toInt();
    m_PrefixMap.clear();
    m_SortedList.clear();
    m_SortedList.reserve(count);
    QList<CPrefix*>& id_list = ref_pointers.prefixes;
    id_list.clear();
    id_list.reserve(count);
    const QJsonArray& ref_prefixes = ref_json["prefixes"].toArray();
    foreach (QJsonValue val_prefix, ref_prefixes) {
        const QJsonObject& obj_prefix = val_prefix.toObject();
        const QString& str_prefix = obj_prefix["prefix"].toString();
        CPrefix* p_prefix = new CPrefix(str_prefix);
        p_prefix->read_json(obj_prefix);
        m_PrefixMap.insert(str_prefix, p_prefix);
        id_list[obj_prefix["id"].toInt()] = p_prefix;
    }
    sort_by_count();
}

/******************************************/
/*                  STEMS                 */
/******************************************/

void CStem::write_json(QJsonObject &ref_json) const
{

    /*
    ref_json["wordListStartPoint"] = m_word_list_start_point;
    ref_json["wordListEndPoint"] = m_word_list_end_point;
    */
    QJsonArray arr_signatures;
    CSignature* p_sig;
    foreach (p_sig, m_Signatures) {
        QJsonObject obj_sig;
        obj_sig["sig"] = p_sig->get_key();
        obj_sig["id"] = p_sig->get_json_id();
        arr_signatures.append(obj_sig);
    }

    QJsonArray arr_parasignature;
    CSuffix* p_suffix;
    foreach (p_suffix, m_parasignature) {
        QJsonObject obj_affix;
        obj_affix["affix"] = p_suffix->get_key();
        obj_affix["id"] = p_suffix->get_json_id();
        arr_parasignature.append(obj_affix);
    }
    ref_json["id"] = m_json_id;
    ref_json["stem"] = m_key;
    ref_json["count"] = m_count;
    ref_json["sigs"] = arr_signatures;
    ref_json["parasig"] = arr_parasignature;
    /* need to change this part */
    /*
    QJsonArray arr_autobiography;
    foreach (auto string, m_Autobiography) {
        arr_autobiography.append(string);
    }
    ref_json["autobiography"] = arr_autobiography;
    */
}

void CStem::read_json_1(const QJsonObject &ref_json)
{
    m_key = ref_json["stem"].toString();
    m_count = ref_json["count"].toInt();
    m_word_list_start_point = ref_json["wordListStartPoint"].toInt();
    m_word_list_end_point = ref_json["wordListEndPoint"].toInt();
    /*
    const QJsonArray& arr_autobiography = ref_json["autobiography"].toArray();
    foreach (QJsonValue val_autobiography, arr_autobiography) {
        m_Autobiography.append(val_autobiography.toString());
    }
    */
}

void CStem::read_json_2(const QJsonObject &ref_json, const CJsonInfo &ref_pointers)
{
    if (ref_pointers.suffix_flag) {
        const QJsonArray& arr_sigs = ref_json["sigs"].toArray();
        foreach (QJsonValue val_sig, arr_sigs) {
            const QJsonObject& obj_sig = val_sig.toObject();
            CSignature* p_sig = ref_pointers.suffix_signatures[obj_sig["id"].toInt()];
            add_signature(p_sig);
        }
        const QJsonArray& arr_parasig = ref_json["parasig"].toArray();
        foreach (QJsonValue val_parasig, arr_parasig) {
            const QJsonObject& obj_suf = val_parasig.toObject();
            CSuffix* p_suf = ref_pointers.suffixes[obj_suf["id"].toInt()];
            m_parasignature.append(p_suf);
        }
    } else {
        const QJsonArray& arr_sigs = ref_json["sigs"].toArray();
        foreach (QJsonValue val_sig, arr_sigs) {
            const QJsonObject& obj_sig = val_sig.toObject();
            CSignature* p_sig = ref_pointers.prefix_signatures[obj_sig["id"].toInt()];
            add_signature(p_sig);
        }
        m_parasignature.clear();
    }
}

/******************************************/
/*            STEM COLLECTION             */
/******************************************/

void CStemCollection::assign_json_id()
{
    int curr_json_id = 0;
    foreach (CStem* p_stem, *m_StringToStemMap) {
        p_stem->set_json_id(curr_json_id++);
    }
}


void CStemCollection::write_json(QJsonObject &ref_json)
{
    QJsonArray arr_stems;
    foreach (CStem* p_stem, *m_StringToStemMap) {
        QJsonObject json_stem;
        p_stem->write_json(json_stem);
        arr_stems.append(json_stem);
    }
    QJsonArray arr_sorted_stems;
    foreach (CStem* p_stem, m_SortList) {
        arr_sorted_stems.append(p_stem->get_json_id());
    }
    ref_json["count"] = m_StringToStemMap->size();
    ref_json["stems"] = arr_stems;
    ref_json["sortedStems"] = arr_sorted_stems;
    ref_json["sortValidFlag"] = m_SortValidFlag;
    ref_json["sortStyle"] = CJsonInfo::eSortStyle_to_string(m_SortStyle);
}

void CStemCollection::read_json_1(const QJsonObject& ref_json, CJsonInfo &ref_pointers)
{
    int count = ref_json["count"].toInt();
    m_StringToStemMap->clear();
    QList<CStem*>& stem_ptr_list = ref_pointers.suffix_flag?
                ref_pointers.suffix_stems:
                ref_pointers.prefix_stems;
    stem_ptr_list.clear();
    stem_ptr_list.reserve(count);
    const QJsonArray& ref_stems = ref_json["stems"].toArray();
    foreach (QJsonValue val_stem, ref_stems) {
        const QJsonObject& obj_stem = val_stem.toObject();
        const QString& str_stem = obj_stem["stem"].toString();
        CStem* p_stem = new CStem(str_stem);
        p_stem->read_json_1(obj_stem);
        m_StringToStemMap->insert(str_stem, p_stem);
        stem_ptr_list[obj_stem["id"].toInt()] = p_stem;
    }

    m_SortList.clear();
    m_SortList.reserve(count);
    const QJsonArray& ref_sortedstems = ref_json["sortedStems"].toArray();
    for (int arr_i = 0; arr_i < count; arr_i++) {
        const int stemid = ref_sortedstems[arr_i].toInt();
        m_SortList[arr_i] = stem_ptr_list[stemid];
    }
    m_SortValidFlag = ref_json["sortValidFlag"].toBool();
    m_SortStyle = CJsonInfo::string_to_eSortStyle(ref_json["sortStyle"].toString());
}

void CStemCollection::read_json_2(const QJsonObject& ref_json, const CJsonInfo &ref_pointers)
{
    const QList<CStem*>& stem_ptr_list = ref_pointers.suffix_flag?
                ref_pointers.suffix_stems:
                ref_pointers.prefix_stems;
    const QJsonArray& ref_stems = ref_json["stems"].toArray();
    foreach (QJsonValue val_stem, ref_stems) {
        const QJsonObject& obj_stem = val_stem.toObject();
        CStem* p_stem = stem_ptr_list[obj_stem["id"].toInt()];
        p_stem->read_json_2(obj_stem, ref_pointers);
    }
}

/******************************************/
/*                SIGNATURE               */
/******************************************/

void CSignature::write_json(QJsonObject &ref_json) const
{
    QJsonArray arr_suffixes;
    foreach (CSuffix* p_suffix, *m_Suffixes) {
        QJsonObject obj_suffix;
        obj_suffix["suffix"] = p_suffix->get_key();
        obj_suffix["id"] = p_suffix->get_json_id();
        arr_suffixes.append(obj_suffix);
    }


    QJsonArray arr_prefixes;
    foreach (CPrefix* p_prefix, *m_Prefixes) {
        QJsonObject obj_prefix;
        obj_prefix["prefix"] = p_prefix->get_key();
        obj_prefix["id"] = p_prefix->get_json_id();
        arr_prefixes.append(obj_prefix);
    }


    QJsonArray arr_stems;
    foreach (CStem* p_stem, *m_Stems) {
        QJsonObject obj_stem;
        obj_stem["stem"] = p_stem->get_key();
        obj_stem["id"] = p_stem->get_json_id();
        arr_stems.append(obj_stem);
    }
    ref_json["id"] = m_json_id;
    ref_json["sig"] = m_Signature;
    ref_json["stemEntropy"] = m_stem_entropy;
    ref_json["suffixes"] = arr_suffixes;
    ref_json["prefixes"] = arr_prefixes;
    ref_json["stems"] = arr_stems;
}


/******************************************/
/*          SIGNATURE COLLECTION          */
/******************************************/

void CSignatureCollection::assign_json_id()
{
    int curr_json_id = 0;
    foreach (CSignature* p_sig, m_SignatureMap) {
        p_sig->set_json_id(curr_json_id++);
    }
}

void CSignatureCollection::write_json(QJsonObject& ref_json) const
{
    QJsonArray arr_sigs;
    QJsonArray arr_containment_map;
    foreach (CSignature* p_sig, m_SignatureMap) {
        QJsonObject obj_sig;
        p_sig->write_json(obj_sig);
        arr_sigs.append(obj_sig);

        QJsonArray arr_containment_list;
        if (m_ContainmentMap.contains(p_sig)) {
            foreach (CSignature* p_sig2, *(m_ContainmentMap[p_sig])) {
                arr_containment_list.append(p_sig2->get_json_id());
            }
        }
        arr_containment_map.append(arr_containment_list);
    }

    QJsonArray arr_sorted_sigs;
    foreach (CSignature* p_sig, m_SortList) {
        arr_sorted_sigs.append(p_sig->get_json_id());
    }

    QJsonArray arr_minimal_cover;
    foreach (CSignature* p_sig, m_minimal_cover) {
        arr_minimal_cover.append(p_sig->get_json_id());
    }

    ref_json["count"] = m_SignatureMap.size();
    ref_json["containmentMap"] = arr_containment_map;
    ref_json["minimalCover"] = arr_minimal_cover;
    ref_json["mimimalCoverMinNumOfStems"] = m_minimum_number_of_stems_for_minimal_cover;
    ref_json["sigs"] = arr_sigs;
    ref_json["sortedSigs"] = arr_sorted_sigs;
    ref_json["sortValidFlag"] = m_SortValidFlag;
    ref_json["sortStyle"] = CJsonInfo::eSortStyle_to_string(m_SortStyle);
}
