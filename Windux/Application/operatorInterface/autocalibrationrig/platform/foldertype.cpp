/**
 * @file foldertype.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "foldertype.h"

#include <QDir>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool folderType::isCachedFolderType( folderType::type t )
{
    // these file types will *not* be cached locally from remote systems
    if (( Apps == t ) ||
        ( DBs == t ) ||
        ( Fonts == t ) ||
        ( Python == t ) ||
        ( Upgrades == t ))
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void folderType::appendFolderName( QDir& d, type t, bool system )
{
    QString folder;

    // get sub folder
    switch ( t )
    {
    case Apps:
        folder = "apps";
        break;
    case Audio:
        folder = "audio";
        break;
    case DBs:
        folder = "db";
        break;
    case Documents:
        folder = "docs";
        break;
    case Download:
        folder = "download";
        break;
    case Firmware:
        folder = "firmware";
        break;
    case Fonts:
        folder = "fonts";
        break;
    case Html:
        folder = "html";
        break;
    case Images:
        folder = "images";
        break;
    case Layouts:
        folder = "layouts";
        break;
    case Licenses:
        folder = "license";
        break;
    case Playlists:
        folder = "playlists";
        break;
    case Python:
        folder = "python";
        break;
    case Videos:
        folder = "videos";
        break;
    case Upgrades:
        folder = "upgrade";
        break;
    case DownloadHolding:
        folder = "downloadHolding";
        break;
    case CategoriesImages:
        folder = "images/categories";
        break;
    case FlashkeyImages:
        folder = "images/softkeys";
        break;
    case KeyboardImages:
        folder = "images/keyboards";
        break;
    case LabelImages:
        folder = "images/label";
        break;
    case PlanogramImages:
        folder = "images/planograms";
        break;
    case ProductsImages:
        folder = "images/products";
        break;
    case SelfServeImages:
        folder = "images/selfServe";
        break;
    case SlideshowImages:
        folder = "images/slideshow";
        break;
    case SpecialsImages:
        folder = "images/specials";
        break;
    default:
        break;
    }

    // system folder
    if ( system )
        if ( RootFolder != t )
            folder += "/system";

    // append the folder
    if ( folder.length() )
        d.setPath( d.path() + "/" + folder );
}
