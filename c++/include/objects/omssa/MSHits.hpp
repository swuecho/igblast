/* $Id: MSHits.hpp 103491 2007-05-04 17:18:18Z kazimird $
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
 */

/// @MSHits.hpp
/// User-defined methods of the data storage class.
///
/// This file was originally generated by application DATATOOL
/// using the following specifications:
/// 'omssa.asn'.
///
/// New methods or data members can be added to it if needed.
/// See also: MSHits_.hpp


#ifndef OBJECTS_OMSSA_MSHITS_HPP
#define OBJECTS_OMSSA_MSHITS_HPP


// generated includes
#include <objects/omssa/MSHits_.hpp>
#include <objects/omssa/MSModSpecSet.hpp>

// generated classes

BEGIN_NCBI_SCOPE

BEGIN_objects_SCOPE // namespace ncbi::objects::

/////////////////////////////////////////////////////////////////////////////

class NCBI_OMSSA_EXPORT CMSHits : public CMSHits_Base
{
    typedef CMSHits_Base Tparent;
public:
    // constructor
    CMSHits(void);
    // destructor
    ~CMSHits(void);

    ///
    ///  Makes a string of mods, positions for display purposes
    ///  
    void MakeModString(string& StringOut, CRef <CMSModSpecSet> Modset) const;

    ///
    ///  Makes a peptide AA string, lower case for mods
    ///  
    void MakePepString(string& StringOut) const;

private:
    // Prohibit copy constructor and assignment operator
    CMSHits(const CMSHits& value);
    CMSHits& operator=(const CMSHits& value);

};

/////////////////// CMSHits inline methods

// constructor
inline
CMSHits::CMSHits(void)
{
}


/////////////////// end of CMSHits inline methods


END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE

#endif // OBJECTS_OMSSA_MSHITS_HPP
/* Original file checksum: lines: 94, chars: 2509, CRC32: 193768f4 */
