
#include <stdafx.h>
#pragma hdrstop
#include "id3_field.h"
#include "propspec.h"

//+-------------------------------------------------------------------------
//
//  Member:     CFullPropSpec::CFullPropSpec, public
//
//  Synopsis:   Default constructor
//
//  Effects:    Defines property with null guid and propid 0
//
//--------------------------------------------------------------------------

CFullPropSpec::CFullPropSpec()
{
    memset( &m_guidPropSet, 0, sizeof( m_guidPropSet ) );
    m_psProperty.ulKind = PRSPEC_PROPID;
    m_psProperty.propid = 0;
}

//+-------------------------------------------------------------------------
//
//  Member:     CFullPropSpec::CFullPropSpec, public
//
//  Synopsis:   Construct propid based propspec
//
//  Arguments:  [guidPropSet]  -- Property set
//              [pidProperty] -- Property
//
//--------------------------------------------------------------------------

CFullPropSpec::CFullPropSpec( GUID const & guidPropSet, PROPID pidProperty )
:	m_guidPropSet( guidPropSet )
{
    m_psProperty.ulKind = PRSPEC_PROPID;
    m_psProperty.propid = pidProperty;
}

//+-------------------------------------------------------------------------
//
//  Member:     CFullPropSpec::CFullPropSpec, public
//
//  Synopsis:   Construct name based propspec
//
//  Arguments:  [guidPropSet] -- Property set
//              [wcsProperty] -- Property
//
//--------------------------------------------------------------------------

CFullPropSpec::CFullPropSpec( GUID const & guidPropSet,
                              WCHAR const * wcsProperty )
:	m_guidPropSet( guidPropSet )
{
    m_psProperty.ulKind = PRSPEC_PROPID;
    SetProperty( wcsProperty );
}

//+-------------------------------------------------------------------------
//
//  Member:     CFullPropSpec::CFullPropSpec, public
//
//  Synopsis:   Copy constructor
//
//  Arguments:  [src] -- Source property spec
//
//--------------------------------------------------------------------------

CFullPropSpec::CFullPropSpec( CFullPropSpec const & src )
{
    m_psProperty.ulKind = src.m_psProperty.ulKind;

    if ( m_psProperty.ulKind == PRSPEC_LPWSTR )
    {
        if ( src.m_psProperty.lpwstr )
        {
            m_psProperty.ulKind = PRSPEC_PROPID;
            SetProperty( src.m_psProperty.lpwstr );
        }
        else
            m_psProperty.lpwstr = 0;

    }
    else
    {
        m_psProperty.propid = src.m_psProperty.propid;
    }
}

inline void * operator new( size_t size, void * p )
{
    return( p );
}

//+-------------------------------------------------------------------------
//
//  Member:     CFullPropSpec::operator=, public
//
//  Synopsis:   Assignment operator
//
//  Arguments:  [Property] -- Source property
//
//--------------------------------------------------------------------------

CFullPropSpec & CFullPropSpec::operator=( CFullPropSpec const & Property )
{
    //
    // Clean up.
    //

    CFullPropSpec::~CFullPropSpec();

    new (this) CFullPropSpec( Property );

    return *this;
}


CFullPropSpec::~CFullPropSpec()
{
    if ( m_psProperty.ulKind == PRSPEC_LPWSTR &&
         m_psProperty.lpwstr )
    {
        CoTaskMemFree( m_psProperty.lpwstr );
    }
}


void CFullPropSpec::SetProperty( PROPID pidProperty )
{
    if ( m_psProperty.ulKind == PRSPEC_LPWSTR &&
         0 != m_psProperty.lpwstr )
    {
        CoTaskMemFree( m_psProperty.lpwstr );
    }

    m_psProperty.ulKind = PRSPEC_PROPID;
    m_psProperty.propid = pidProperty;
}

BOOL CFullPropSpec::SetProperty( WCHAR const * wcsProperty )
{
    if ( m_psProperty.ulKind == PRSPEC_LPWSTR &&
         0 != m_psProperty.lpwstr )
    {
        CoTaskMemFree( m_psProperty.lpwstr );
    }

    m_psProperty.ulKind = PRSPEC_LPWSTR;

    int len = (wcslen( wcsProperty ) + 1) * sizeof( WCHAR );

    m_psProperty.lpwstr = (WCHAR *)CoTaskMemAlloc( len );

    if ( 0 != m_psProperty.lpwstr )
    {
        memcpy( m_psProperty.lpwstr,
                wcsProperty,
                len );
        return( TRUE );
    }
    else
    {
        m_psProperty.lpwstr = 0;
        return( FALSE );
    }
}

int CFullPropSpec::operator==( CFullPropSpec const & prop ) const
{
    if ( memcmp( &prop.m_guidPropSet,
                 &m_guidPropSet,
                 sizeof( m_guidPropSet ) ) != 0 ||
         prop.m_psProperty.ulKind != m_psProperty.ulKind )
    {
        return( 0 );
    }

    switch( m_psProperty.ulKind )
    {
    case PRSPEC_LPWSTR:
        return( _wcsicmp( GetPropertyName(), prop.GetPropertyName() ) == 0 );
        break;

    case PRSPEC_PROPID:
        return( GetPropertyPropid() == prop.GetPropertyPropid() );
        break;

    default:
        return( 0 );
        break;
    }
}

int CFullPropSpec::operator!=( CFullPropSpec const & prop ) const
{
    if (*this == prop)
        return( 0 );
    else
        return( 1 );
}

