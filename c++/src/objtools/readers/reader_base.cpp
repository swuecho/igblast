/*  $Id: reader_base.cpp 440817 2014-07-17 12:28:31Z ludwigf $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Author:  Frank Ludwig
 *
 * File Description:
 *   Basic reader interface.
 *
 */

#include <ncbi_pch.hpp>
#include <corelib/ncbistd.hpp>
#include <corelib/ncbiapp.hpp>
#include <corelib/ncbithr.hpp>
#include <corelib/ncbiutil.hpp>
#include <corelib/ncbiexpt.hpp>
#include <corelib/stream_utils.hpp>

#include <util/static_map.hpp>
#include <util/line_reader.hpp>

#include <serial/iterator.hpp>
#include <serial/objistrasn.hpp>

// Objects includes
#include <objects/general/Int_fuzz.hpp>
#include <objects/general/Object_id.hpp>
#include <objects/general/User_object.hpp>
#include <objects/general/User_field.hpp>
#include <objects/general/Dbtag.hpp>

#include <objects/seqloc/Seq_id.hpp>
#include <objects/seqloc/Seq_loc.hpp>
#include <objects/seqloc/Seq_interval.hpp>
#include <objects/seqloc/Seq_point.hpp>

#include <objects/seqset/Seq_entry.hpp>
#include <objects/seq/Seq_annot.hpp>
#include <objects/seq/Annotdesc.hpp>
#include <objects/seq/Annot_descr.hpp>
#include <objects/seq/Seq_descr.hpp>
#include <objects/seqfeat/SeqFeatData.hpp>

#include <objects/seqfeat/Seq_feat.hpp>
#include <objects/seqfeat/BioSource.hpp>
#include <objects/seqfeat/Org_ref.hpp>
#include <objects/seqfeat/OrgName.hpp>
#include <objects/seqfeat/SubSource.hpp>
#include <objects/seqfeat/OrgMod.hpp>
#include <objects/seqfeat/Gene_ref.hpp>
#include <objects/seqfeat/Cdregion.hpp>
#include <objects/seqfeat/Code_break.hpp>
#include <objects/seqfeat/Genetic_code.hpp>
#include <objects/seqfeat/Genetic_code_table.hpp>
#include <objects/seqfeat/RNA_ref.hpp>
#include <objects/seqfeat/Trna_ext.hpp>
#include <objects/seqfeat/Imp_feat.hpp>
#include <objects/seqfeat/Gb_qual.hpp>
#include <objects/seqfeat/Feat_id.hpp>

#include <objtools/readers/read_util.hpp>
#include <objtools/readers/reader_exception.hpp>
#include <objtools/readers/line_error.hpp>
#include <objtools/readers/message_listener.hpp>
#include <objtools/readers/reader_base.hpp>
#include <objtools/readers/bed_reader.hpp>
#include <objtools/readers/microarray_reader.hpp>
#include <objtools/readers/wiggle_reader.hpp>
#include <objtools/readers/gff3_reader.hpp>
#include <objtools/readers/gvf_reader.hpp>
#include <objtools/readers/vcf_reader.hpp>
#include <objtools/readers/rm_reader.hpp>
#include <objtools/readers/fasta.hpp>
#include <objtools/readers/readfeat.hpp>
#include <objtools/error_codes.hpp>
#include <objtools/readers/ucscregion_reader.hpp>

#include <algorithm>

#include "reader_data.hpp"

#define NCBI_USE_ERRCODE_X   Objtools_Rd_RepMask

BEGIN_NCBI_SCOPE
BEGIN_objects_SCOPE // namespace ncbi::objects::

//  ----------------------------------------------------------------------------
CReaderBase*
CReaderBase::GetReader(
    CFormatGuess::EFormat format,
    TReaderFlags flags )
//  ----------------------------------------------------------------------------
{
    switch ( format ) {
    default:
        return 0;
    case CFormatGuess::eBed:
        return new CBedReader(flags);
    case CFormatGuess::eBed15:
        return new CMicroArrayReader(flags);
    case CFormatGuess::eWiggle:
        return new CWiggleReader(flags);
    case CFormatGuess::eGtf:
    case CFormatGuess::eGtf_POISENED:
        return new CGff3Reader(flags);
    case CFormatGuess::eGff3:
        return new CGff3Reader(flags);
    case CFormatGuess::eGvf:
        return new CGvfReader(flags);
    case CFormatGuess::eVcf:
        return new CVcfReader(flags);
    case CFormatGuess::eRmo:
        return new CRepeatMaskerReader(flags);
    case CFormatGuess::eFasta:
        return new CFastaReader(flags);
    case CFormatGuess::eFiveColFeatureTable:
        return new CFeature_table_reader(flags);
    case CFormatGuess::eUCSCRegion:
        return new CUCSCRegionReader(flags);
    }
}

//  ----------------------------------------------------------------------------
CReaderBase::CReaderBase(
    TReaderFlags flags) :
//  ----------------------------------------------------------------------------
    m_uLineNumber(0),
    m_iFlags(flags)
{
    m_pTrackDefaults = new CTrackData;
}

//  ----------------------------------------------------------------------------
CReaderBase::~CReaderBase()
//  ----------------------------------------------------------------------------
{
    delete m_pTrackDefaults;
}

//  ----------------------------------------------------------------------------
CRef< CSerialObject >
CReaderBase::ReadObject(
    CNcbiIstream& istr,
    IMessageListener* pMessageListener ) 
//  ----------------------------------------------------------------------------
{
    CStreamLineReader lr( istr );
    return ReadObject( lr, pMessageListener );
}

//  ----------------------------------------------------------------------------
CRef< CSeq_annot >
CReaderBase::ReadSeqAnnot(
    CNcbiIstream& istr,
    IMessageListener* pMessageListener ) 
//  ----------------------------------------------------------------------------
{
    CStreamLineReader lr( istr );
    return ReadSeqAnnot( lr, pMessageListener );
}

//  ----------------------------------------------------------------------------
CRef< CSeq_annot >
CReaderBase::ReadSeqAnnot(
    ILineReader&,
    IMessageListener* ) 
//  ----------------------------------------------------------------------------
{
    return CRef<CSeq_annot>();
}
                
//  ----------------------------------------------------------------------------
CRef< CSeq_entry >
CReaderBase::ReadSeqEntry(
    CNcbiIstream& istr,
    IMessageListener* pMessageListener ) 
//  ----------------------------------------------------------------------------
{
    CStreamLineReader lr( istr );
    return ReadSeqEntry( lr, pMessageListener );
}

//  ----------------------------------------------------------------------------
CRef< CSeq_entry >
CReaderBase::ReadSeqEntry(
    ILineReader&,
    IMessageListener* ) 
//  ----------------------------------------------------------------------------
{
    return CRef<CSeq_entry>();
}
               
//  ----------------------------------------------------------------------------
void
CReaderBase::ProcessError(
    CObjReaderLineException& err,
    IMessageListener* pContainer )
//  ----------------------------------------------------------------------------
{
    err.SetLineNumber( m_uLineNumber );
    if (!pContainer) {
        err.Throw();
    }
    if (!pContainer->PutError(err)) {
        AutoPtr<CObjReaderLineException> pErr(
            CObjReaderLineException::Create(
            eDiag_Fatal,
            0,
            "Error allowance exceeded",
            ILineError::eProblem_GeneralParsingError) );
        pErr->Throw();
        //err.Throw();
    }
}

//  ----------------------------------------------------------------------------
void
CReaderBase::ProcessWarning(
    CObjReaderLineException& err,
    IMessageListener* pContainer )
//  ----------------------------------------------------------------------------
{
    err.SetLineNumber( m_uLineNumber );
    if (!pContainer) {
        cerr << m_uLineNumber << ": " << err.SeverityStr() << err.Message() 
            << endl;
        return;
    }
    if (!pContainer->PutError(err)) {
        err.Throw();
    }
}

//  ----------------------------------------------------------------------------
void
CReaderBase::ProcessError(
    CLineError& err,
    IMessageListener* pContainer )
//  ----------------------------------------------------------------------------
{
    if (!pContainer  ||  !pContainer->PutError(err)) {
        err.Throw();
    }
 }

//  ----------------------------------------------------------------------------
void
CReaderBase::ProcessWarning(
    CLineError& err,
    IMessageListener* pContainer )
//  ----------------------------------------------------------------------------
{
    if (!pContainer) {
        cerr << m_uLineNumber << ": " << err.SeverityStr() << err.Message() 
            << endl;
        return;
    }
    if (!pContainer->PutError(err)) {
        err.Throw();
    }
 }

//  ----------------------------------------------------------------------------
void CReaderBase::x_SetBrowserRegion(
    const string& strRaw,
    CAnnot_descr& desc,
    IMessageListener* pEC)
//  ----------------------------------------------------------------------------
{
    CRef<CSeq_loc> location( new CSeq_loc );

    string strChrom;
    string strInterval;
    if ( ! NStr::SplitInTwo( strRaw, ":", strChrom, strInterval ) ) {
        AutoPtr<CObjReaderLineException> pErr(
            CObjReaderLineException::Create(
                eDiag_Error,
                0,
                "Bad browser line: cannot parse browser position" ) );
        ProcessError(*pErr, pEC);
    }
    CRef<CSeq_id> id( new CSeq_id( CSeq_id::e_Local, strChrom ) );

    if (NStr::Compare(strInterval, "start-stop") == 0 )
    {
        location->SetWhole(*id);
    }
    else
    {
        string strFrom;
        string strTo;
        if ( ! NStr::SplitInTwo( strInterval, "-", strFrom, strTo ) ) {
            AutoPtr<CObjReaderLineException> pErr(
                CObjReaderLineException::Create(
                eDiag_Error,
                0,
                "Bad browser line: cannot parse browser position" ) );
            ProcessError(*pErr, pEC);
        }  
        try
        {
            int n_from,n_to;

            n_from = NStr::StringToInt(strFrom, NStr::fAllowCommas);
            n_to   = NStr::StringToInt(strTo, NStr::fAllowCommas);

            CSeq_interval& interval = location->SetInt();
            interval.SetFrom(n_from-1);
            interval.SetTo(n_to-1);
            interval.SetStrand( eNa_strand_unknown );
            location->SetId( *id );

        }
        catch (const CStringException&)
        {
            AutoPtr<CObjReaderLineException> pErr(
                CObjReaderLineException::Create(
                eDiag_Error,
                0,
                "Bad browser line: cannot parse browser position" ) );
            ProcessError(*pErr, pEC);
            location.Release();
        }
    }

    if (location.NotEmpty())
    {
        CRef<CAnnotdesc> region( new CAnnotdesc() );
        region->SetRegion( *location );
        desc.Set().push_back( region );
    }
}
    
//  ----------------------------------------------------------------------------
bool CReaderBase::x_ParseBrowserLine(
    const string& strLine,
    CRef<CSeq_annot>& annot,
    IMessageListener* pEC)
//  ----------------------------------------------------------------------------
{
    if ( ! NStr::StartsWith( strLine, "browser" ) ) {
        return false;
    }
    CAnnot_descr& desc = annot->SetDesc();
    
    vector<string> fields;
    NStr::Tokenize( strLine, " \t", fields, NStr::eMergeDelims );
    for ( vector<string>::iterator it = fields.begin(); it != fields.end(); ++it ) {
        if ( *it == "position" ) {
            ++it;
            if ( it == fields.end() ) {
                AutoPtr<CObjReaderLineException> pErr(
                    CObjReaderLineException::Create(
                    eDiag_Error,
                    0,
                    "Bad browser line: incomplete position directive" ) );
                ProcessError(*pErr, pEC);
            }
            x_SetBrowserRegion(*it, desc, pEC);
        }
    }

    return true;
}

//  ----------------------------------------------------------------------------
void CReaderBase::x_AssignTrackData(
    CRef<CSeq_annot>& annot )
//  ----------------------------------------------------------------------------
{
    CAnnot_descr& desc = annot->SetDesc();
    CRef<CUser_object> trackdata( new CUser_object() );
    trackdata->SetType().SetStr( "Track Data" );
   
    if ( !m_pTrackDefaults->Description().empty() ) {
        annot->SetTitleDesc(m_pTrackDefaults->Description());
    }
    if ( !m_pTrackDefaults->Name().empty() ) {
        annot->SetNameDesc(m_pTrackDefaults->Name());
    }
    map<string,string>::const_iterator cit = m_pTrackDefaults->Values().begin();
    while ( cit != m_pTrackDefaults->Values().end() ) {
        trackdata->AddField( cit->first, cit->second );
        ++cit;
    }
    if ( trackdata->CanGetData() && ! trackdata->GetData().empty() ) {
        CRef<CAnnotdesc> user( new CAnnotdesc() );
        user->SetUser( *trackdata );
        desc.Set().push_back( user );
    }
}

//  ----------------------------------------------------------------------------
bool CReaderBase::x_ParseTrackLine(
    const string& strLine,
    CRef<CSeq_annot>& annot,
    IMessageListener* pEC)
//  ----------------------------------------------------------------------------
{
    vector<string> parts;
    CReadUtil::Tokenize( strLine, " \t", parts );
    if ( !CTrackData::IsTrackData( parts ) ) {
        return false;
    }
    m_pTrackDefaults->ParseLine( parts );
    return true;
}

//  ----------------------------------------------------------------------------
void CReaderBase::x_SetTrackData(
    CRef<CSeq_annot>& annot,
    CRef<CUser_object>& trackdata,
    const string& strKey,
    const string& strValue )
//  ----------------------------------------------------------------------------
{
    trackdata->AddField( strKey, strValue );
}

//  ----------------------------------------------------------------------------
bool CReaderBase::xParseComment(
    const CTempString& record,
    CRef<CSeq_annot>& annot ) /* throws CObjReaderLineException */
//  ----------------------------------------------------------------------------
{
    if (NStr::StartsWith(record, "#")) {
        return true;
    }
    return false;
}
 
//  ----------------------------------------------------------------------------
void CReaderBase::x_AddConversionInfo(
    CRef<CSeq_annot >& annot,
    IMessageListener *pMessageListener )
//  ----------------------------------------------------------------------------
{
    if ( !annot || !pMessageListener ) {
        return;
    }
    if (0 == pMessageListener->LevelCount( eDiag_Critical )  &&
        0 == pMessageListener->LevelCount( eDiag_Error ) &&
        0 == pMessageListener->LevelCount( eDiag_Warning ) &&
        0 == pMessageListener->LevelCount( eDiag_Info )) {
        return;
    }


    CRef<CAnnotdesc> user( new CAnnotdesc() );
    user->SetUser( *x_MakeAsnConversionInfo( pMessageListener ) );
    annot->SetDesc().Set().push_back( user );
}

//  ----------------------------------------------------------------------------
void CReaderBase::x_AddConversionInfo(
    CRef<CSeq_entry >& entry,
    IMessageListener *pMessageListener )
//  ----------------------------------------------------------------------------
{
    if ( !entry || !pMessageListener ) {
        return;
    }
    CRef<CSeqdesc> user( new CSeqdesc() );
    user->SetUser( *x_MakeAsnConversionInfo( pMessageListener ) );
    entry->SetDescr().Set().push_back( 
        user );
}

//  ----------------------------------------------------------------------------
CRef<CUser_object> CReaderBase::x_MakeAsnConversionInfo(
    IMessageListener* pMessageListener )
//  ----------------------------------------------------------------------------
{
    CRef<CUser_object> conversioninfo( new CUser_object() );
    conversioninfo->SetType().SetStr( "Conversion Info" );    
    conversioninfo->AddField( 
        "critical errors", int ( pMessageListener->LevelCount( eDiag_Critical ) ) );
    conversioninfo->AddField( 
        "errors", int ( pMessageListener->LevelCount( eDiag_Error ) ) );
    conversioninfo->AddField( 
        "warnings", int ( pMessageListener->LevelCount( eDiag_Warning ) ) );
    conversioninfo->AddField( 
        "notes", int ( pMessageListener->LevelCount( eDiag_Info ) ) );
    return conversioninfo;
}

END_objects_SCOPE
END_NCBI_SCOPE
