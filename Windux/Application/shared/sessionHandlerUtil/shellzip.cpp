/**
  * @file shellzip.cpp
  * Utility class for creating a zip using windows shell.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Randy Blankley
  */

#include "shellzip.h"

// this class will work only under windows
#ifdef Q_OS_WIN

#include <windows.h>
#include <ole2.h>
#include <shldisp.h>
#include <tchar.h>

#include <QFile>
#include <QString>
#include <QStringList>

// this is defined under vs2010 but missing from mingw
#ifndef __Folder_INTERFACE_DEFINED__
#define __Folder_INTERFACE_DEFINED__

#define INTERFACE Folder
DECLARE_INTERFACE_(Folder, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT*) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT,LCID,LPTYPEINFO*) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID,LPOLESTR*,UINT,LCID,DISPID*) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*) PURE;
    STDMETHOD(get_Title)(THIS_ BSTR*) PURE;
    STDMETHOD(get_Application)(THIS_ LPDISPATCH*) PURE;
    STDMETHOD(get_Parent)(THIS_ LPDISPATCH*) PURE;
    STDMETHOD(get_ParentFolder)(THIS_ LPVOID*) PURE; // should not be void, but not using it...
    STDMETHOD(Items)(THIS_ LPVOID*) PURE; // should not be void, but not using it...
    STDMETHOD(ParseName)(THIS_ LPVOID*) PURE; // should not be void, but not using it...
    STDMETHOD(NewFolder)(THIS_ BSTR,VARIANT) PURE;
    STDMETHOD(MoveHere)(THIS_ VARIANT,VARIANT) PURE;
    STDMETHOD(CopyHere)(THIS_ VARIANT,VARIANT) PURE;
    STDMETHOD(GetDetailsOf)(THIS_ VARIANT,INT,BSTR*) PURE;
};
#undef INTERFACE

typedef Folder *LPFOLDER;

#endif // __Folder_INTERFACE_DEFINED__

// this is defined under vs2010 but missing from mingw
#ifndef __IShellDispatch_INTERFACE_DEFINED__
#define __IShellDispatch_INTERFACE_DEFINED__

#define INTERFACE IShellDispatch
DECLARE_INTERFACE_(IShellDispatch, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT*) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT,LCID,LPTYPEINFO*) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID,LPOLESTR*,UINT,LCID,DISPID*) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*) PURE;
    STDMETHOD(get_Application)(THIS_ LPDISPATCH*) PURE;
    STDMETHOD(get_Parent)(THIS_ LPDISPATCH*) PURE;
    STDMETHOD(NameSpace)(THIS_ VARIANT,LPFOLDER*) PURE;
    STDMETHOD(BrowseForFolder)(THIS_ LONG,BSTR,LONG,VARIANT,LPFOLDER*) PURE;
    STDMETHOD(Windows)(THIS_ LPDISPATCH*) PURE;
    STDMETHOD(Open)(THIS_ VARIANT) PURE;
    STDMETHOD(Explore)(THIS_ VARIANT) PURE;
    STDMETHOD(MinimizeAll)(THIS) PURE;
    STDMETHOD(UndoMinimizeALL)(THIS) PURE;
    STDMETHOD(FileRun)(THIS) PURE;
    STDMETHOD(CascadeWindows)(THIS) PURE;
    STDMETHOD(TileVertically)(THIS) PURE;
    STDMETHOD(TileHorizontally)(THIS) PURE;
    STDMETHOD(ShutdownWindows)(THIS) PURE;
    STDMETHOD(Suspend)(THIS) PURE;
    STDMETHOD(EjectPC)(THIS) PURE;
    STDMETHOD(SetTime)(THIS) PURE;
    STDMETHOD(TrayProperties)(THIS) PURE;
    STDMETHOD(Help)(THIS) PURE;
    STDMETHOD(FindFiles)(THIS) PURE;
    STDMETHOD(FindComputer)(THIS) PURE;
    STDMETHOD(RefreshMenu)(THIS) PURE;
    STDMETHOD(ControlPanelItem)(THIS_ BSTR) PURE;
};
#undef INTERFACE

typedef IShellDispatch *LPSHELLDISPATCH;

#endif 	// __IShellDispatch_INTERFACE_DEFINED__

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
shellZip::shellZip( const QString& filename, QObject *parent ) :
    _Mybase( parent ),
    filename_( filename ),
    valid_( false )
{
    filename_.replace( "/", "\\" );

    // initialize the zip file
    valid_ = createZipFile( filename_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
shellZip::~shellZip()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool shellZip::add( const QStringList& list )
{
    bool success = false;

    WCHAR temp[2048];
    HRESULT hr;

    // sanity check
    if ( !valid_ )
        return false;

    // init COM
    hr = CoInitialize( NULL );

    if ( SUCCEEDED( hr ) )
    {
        IShellDispatch *pISD( NULL );

        CLSID clsid;
        IID iid;

        // retrieve class id and iid for shell dispatch
        CLSIDFromProgID( L"Shell.Application", &clsid );

        WCHAR s[] = L"{D8F015C0-C278-11CE-A49E-444553540000}";
        IIDFromString( s, &iid );

        // allocate shell dispatch interface
        hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, iid, (void**) &pISD );

        if (( SUCCEEDED( hr ) ) && ( pISD ))
        {
            Folder *pFolder( NULL );

            // destination file
            MultiByteToWideChar( CP_ACP, 0, qPrintable( filename_ ), -1, temp, sizeof(temp) / sizeof(WCHAR) );
            BSTR dest( SysAllocString( temp ) );

            VARIANT vDir;
            VariantInit( &vDir );
            vDir.vt = VT_BSTR;
            vDir.bstrVal = dest;

            // destination is our zip file
            hr = pISD->NameSpace( vDir, &pFolder );

            if (( SUCCEEDED( hr ) ) && ( pFolder ))
            {
                // assume success
                success = true;

                // zip options
                // 0x0004 Do not display a progress dialog box.
                // 0x0008 Give the file being operated on a new name in a move, copy, or rename operation if a file with the target name already exists.
                // 0x0010 Respond with "Yes to All" for any dialog box that is displayed.
                // 0x0040 Preserve undo information, if possible.
                // 0x0080 Perform the operation on files only if a wildcard file name (*.*) is specified.
                // 0x0100 Display a progress dialog box but do not show the file names.
                // 0x0200 Do not confirm the creation of a new directory if the operation requires one to be created.
                // 0x0400 Do not display a user interface if an error occurs.
                // 0x0800 Do not copy the security attributes of the file.
                // 0x1000 Only operate in the local directory. Don't operate recursively into subdirectories.
                VARIANT vOptions;
                VariantInit( &vOptions );
                vOptions.vt = VT_I4;
                vOptions.lVal = 4 + 16 + 1024;

                // process each file
                foreach ( const QString& f, list )
                {
                    QString sourceFilename( f );
                    sourceFilename.replace( "/", "\\" );

                    // source file
                    MultiByteToWideChar( CP_ACP, 0, qPrintable( sourceFilename ), -1, temp, sizeof(temp) / sizeof(WCHAR) );
                    BSTR source( SysAllocString( temp ) );

                    VARIANT vFile;
                    VariantInit( &vFile );
                    vFile.vt = VT_BSTR;
                    vFile.bstrVal = source;

                    // zip!!
                    hr = pFolder->CopyHere( vFile, vOptions );

                    // allow shell time to process the command
                    Sleep( 1000 );

                    if ( !SUCCEEDED( hr ) )
                        success = false;

                    // cleanup
                    VariantClear( &vFile );
                    SysFreeString( source );
                }

                // cleanup
                VariantClear( &vOptions );

                // release folder
                pFolder->Release();
            }

            // cleanup
            VariantClear( &vDir );
            SysFreeString( dest );

            // release shell dispatch interface
            pISD->Release();
        }

        // uninit COM
        CoUninitialize();
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool shellZip::createZipFile( const QString& filename )
{
    bool result = false;

    // sanity check
    if ( filename.isEmpty() )
        return false;

    QFile f( filename );

    // open file for writing
    if ( f.open( QFile::WriteOnly ) )
    {
        char buffer[] = {80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        // write data to file
        if ( sizeof(buffer) == f.write( buffer, sizeof(buffer) ) )
            result = true;

        f.close();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // Q_OS_WIN
