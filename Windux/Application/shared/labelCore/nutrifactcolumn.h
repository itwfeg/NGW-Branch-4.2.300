/**
 * @file nutrifactcolumn.h
 * Nutrifact data column object.
 *
 * Nutrifact data is composed of one or more columns.  Each column is made up of multiple tags and
 * and data. This class represents a single column of nutrifact data.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef NUTRIFACTCOLUMN_H
#define NUTRIFACTCOLUMN_H

#include <QString>
#include <QMap>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class nutrifactColumn
 * @brief Nutrifact column object.
 *
 * We will assume that each nutrient per column has an Amount and/or a %DV (Daily Value). Using
 * the first three characters to identify the nutrient, we can append either an "A" for Amount or
 * a "P" for % Daily value. Below contains the list of tags.
 * @code
 *   Nutrient                           Amount tag          % Daily Value tag
 *   ================================== =================== ===================
 *   Biotin                             BioA                BioP
 *   Calcium                            ClcA                ClcP
 *   Calories (Total Calories)          CalA                CalP
 *   Calories From Fat                  CaFA                CaFP
 *   Calories From Saturated Fat        CSFA                CSFP
 *   Carbohydrate (Total Carbohydrate)  CarA                CarP
 *   Chloride                           ChlA                ChlP
 *   Cholesterol                        ChoA                ChoP
 *   Chromium                           ChrA                ChrP
 *   Copper                             CopA                CopP
 *   Dietary Fiber                      DiFA                DiFP
 *   Fat (Total Fat)                    FatA                FatP
 *   Folate                             FolA                FolP
 *   Insoluble Fiber                    InFA                InFP
 *   Iodine                             IodA                IodP
 *   Iron                               IrnA                IrnP
 *   Magnesium                          MagA                MagP
 *   Manganese                          MngA                MngP
 *   Molybdenum                         MolA                MolP
 *   Monounsaturated Fat                MuFA                MuFP
 *   Niacin                             NiaA                NiaP
 *   Other carbohydrate                 OCrA                OCrP
 *   Pantothenic Acid                   PaAA                PaAP
 *   Percent of vitamin A present
 *    as beta-carotene                  VABA                VABP
 *   Phosphorus                         PhoA                PhoP
 *   Polyunsaturated Fat                PuFA                PuFP
 *   Potassium                          PotA                PotP
 *   Protein                            ProA                ProP
 *   Riboflavin                         RibA                RibP
 *   Saturated Fat                      SaFA                SaFP
 *   Selenium                           SelA                SelP
 *   Serving Size                       SSzA                SSzP
 *   Servings Per Container             SPCA                SPCP
 *   Sodium                             SodA                SodP
 *   Soluble Fiber                      SoFA                SoFP
 *   Sugar Alcohol                      SuAA                SuAP
 *   Sugars                             SugA                SugP
 *   Thiamin                            ThiA                ThiP
 *   Trans Fat                          TrFA                TrFP
 *   Vitamin A                          ViAA                ViAP
 *   Vitamin B12                        B12A                B12P
 *   Vitamin B6                         VB6A                VB6P
 *   Vitamin C                          ViCA                ViCP
 *   Vitamin D                          ViDA                ViDP
 *   Vitamin E                          ViEA                ViEP
 *   Vitamin K                          ViKA                ViKP
 *   Zinc                               ZncA                ZncP
 * @endcode
 *
 * The nutrifact column streaming data is just a seperated string of tag/value pairs:
 * @code
 *   SSzA(27g)<SUB>SPCA1<SUB>CalA100<SUB>CaFA0<SUB>FatA0g<SUB>FatP0%<SUB>SaFA0g<SUB>SaFP0%<SUB>
 *   ChoA0mg<SUB>ChoP0%<SUB>SodA120mg<SUB>SodP5%<SUB>PotA30mg<SUB>PotP1%<SUB>CarA24g<SUB>
 *   CarP8%<SUB>DiFA1g<SUB>DiFP4%<SUB>SugA13g<SUB>ProA1g<SUB>ViAP10%<SUB>ViCP90%<SUB>ClcP0%<SUB>
 *   IrnP20%<SUB>ThiP20%<SUB>RibP20%<SUB>NiaP20%<SUB>VB6P20%
 * @endcode
 */
class nutrifactColumn : public QMap<QString, QString>
{
    typedef nutrifactColumn _Myt;
    typedef QMap<QString, QString> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    nutrifactColumn();

    /**
     * Constructor.
     *
     * @param[in] data  object string
     */
    nutrifactColumn( const QString& data );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    nutrifactColumn( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~nutrifactColumn();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve column data as string value.
     *
     * @returns  column data as delimited string
     */
    virtual QString data() const;

    /**
     * Set column data from string value.
     *
     * @param[in] value  column data as delimited string
     */
    virtual void setData( const QString& value );

protected:

    /**
     * Substitute.
     */
    static const qint8 SUB;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NUTRIFACTCOLUMN_H
