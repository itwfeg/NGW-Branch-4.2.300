/**
 * @file foldertype.h
 * Folder types.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef FOLDERTYPE_H
#define FOLDERTYPE_H

class QDir;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart folder types.
class folderType
{
public:

    /// Folder types.
    enum type
    {
        // root folder
        RootFolder = 0,                                 ///< Root folder

        // generic folders
        Apps = 100,                                     ///< Application config (i.e. conf)
        Audio,                                          ///< Audio files (i.e. mp3, wav)
        DBs,                                            ///< Databases (i.e. db)
        Documents,                                      ///< Documents (i.e. pdf)
        Fonts,                                          ///< Fonts (i.e. ttf)
        Firmware,                                       ///< Firmware installation tarballs.
        Html,                                           ///< Html files (i.e. htm)
        Images,                                         ///< Images (i.e. gif, png)
        Layouts,                                        ///< Layouts (i.e. xml)
        Licenses,                                       ///< Licenses (i.e. lic)
        Playlists,                                      ///< Playlist files for GCD (i.e. txt)
        Python,                                         ///< Python scripts (i.e. py)
        Videos,                                         ///< Videos (i.e. mp4, wmv)
        Upgrades,                                       ///< Pending Firmware upgrade file storage
        DownloadHolding,                                ///< Pending downloads awaiting install
        Download,                                       ///< Download folder for Update Now
        // folder for specific image types
        CategoriesImages = 200,                         ///< Categories
        FlashkeyImages,                                 ///< Flashkeys/Softkeys
        KeyboardImages,                                 ///< Keyboard Graphics
        LabelImages,                                    ///< Label Graphics
        PlanogramImages,                                ///< Planograms
        ProductsImages,                                 ///< Products
        SelfServeImages,                                ///< Self Service
        SlideshowImages,                                ///< Slideshow
        SpecialsImages                                  ///< Specials
    };

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if folder type is cached or not.
    /**
     * Cached folders will be copied local when connecting to a remote filesystem.
     *
     * @param[in] t  folder type
     * @return  @c true if cached, @c false otherwise
     */
    static bool isCachedFolderType( type t );

    /// Append folder name onto @c QDir object.
    /**
     * @param[in,out] d  folder to append
     * @param[in] t  folder type
     * @param[in] system  @c true to append system folder, @c false otherwise
     */
    static void appendFolderName( QDir& d, type t, bool system = false );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FOLDERTYPE_H
