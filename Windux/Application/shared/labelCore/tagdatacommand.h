/**
 * @file tagdatacommand.h
 * Tag data commands and record types.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef TAGDATACOMMAND_H
#define TAGDATACOMMAND_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class tagDataCommand
 * @brief Hobart tagged data command types.
 *
 * This enumeration details all the different tag data commands possible. If you choose to
 * implement one or more of the below commands please follow the specific directions.
 *
 * If you support tag data there are interface methods you may need to implement. Below is a list
 * of the methods. See the specific directions for which methods you will need to implement.
 * @arg static QList<myClass::key_type> toKeyList( const tagDataList& r );
 * @arg static QList<myClass> toValueList( const tagDataList& r );
 * @arg static tagDataRecordList toTagDataRecordList( const QList<myClass>& values );
 *
 * If you support the @c tagDataCommand::READ_ONE command you must:
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::READ_ONE, 'your record type' );
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::WRITE, 'your record type' );
 * @arg Implement the function toKeyList() above
 * @arg Implement the function toTagDataRecordList() above
 *
 * If you support the @c tagDataCommand::READ_ALL command you must:
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::READ_ALL, 'your record type' );
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::WRITE, 'your record type' );
 * @arg Implement the function toTagDataRecordList() above
 *
 * If you support the @c tagDataCommand::WRITE command you must:
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::WRITE, 'your record type' );
 * @arg Implement the function toValueList() above
 *
 * If you support the @c tagDataCommand::DELETE_ONE command you must:
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::DELETE_ONE, 'your record type' );
 * @arg Implement the function toKeyList() above
 *
 * If you support the @c tagDataCommand::DELETE_ALL command you must:
 * @arg Declare support using the macro
 *      DECLARE_TAG_DATA_RECORD_TYPE( myClass, tagDataCommand::DELETE_ALL, 'your record type' );
 *
 * You may use this example code as a beginning for your class implementation:
 * @code
 *   // the myLabel class supports the following tag data commands
 *   DECLARE_TAG_DATA_RECORD_TYPE( myLabel, tagDataCommand::READ_ALL, 'RT71' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( myLabel, tagDataCommand::WRITE, 'RT9C' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( myLabel, tagDataCommand::DELETE_ONE, 'RTA7' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( myLabel, tagDataCommand::DELETE_ALL, 'RT46' );
 *
 *   // the myLabel class uses int as a key type
 *   class myLabel : public dataObject<int> {
 *   public:
 *
 *       // ctor/dtor and other methods here...
 *
 *       // convert the tag data into a list of int
 *       static QList<int> toKeyList( const tagDataList& r ) {
 *           // iterate r and process each tag that is a key value into the list
 *       }
 *
 *       // convert the tag data into a list of myLabel objects
 *       static QList<myLabel> toValueList( const tagDataList& r ) {
 *           // iterate r and process each tag into a list of objects
 *           // for most tag data formats this is just one object!
 *       }
 *
 *       // convert a list of myLabel objects into tag data
 *       static tagDataList toTagDataList( const QList<myLabel>& values ) {
 *           // iterate values and convert the objects into tag format
 *           // for most tag data formats this is just one object!
 *       }
 *   };
 * @endcode
 */
class tagDataCommand
{
public:

    /**
     * Hobart tagged data command types.
     *
     * Data objects may support one or more of the below commands.
     */
    enum type
    {
        READ_ONE,                           ///< Read one record
        READ_ALL,                           ///< Read all records
        WRITE,                              ///< Write a record
        DELETE_ONE,                         ///< Delete one record
        DELETE_ALL,                         ///< Delete all records
        OTHER,                              ///< Other command
        HOBART_DISK                         ///< Hobart disk command
    };

    /**
     * @struct toRecordType
     * @brief Retrieve record type for specific command.
     *
     * Example usage:
     * @code
     *   // assuming there exists a myLabel class
     *   tagDataRecordType myLabelReadOneRecordType = tagDataCommand::toRecordType<myLabel, tagDataCommand::READ_ONE>::TYPE;
     * @endcode
     *
     * @note
     * This is just a forward declaration. Use the DECLARE_TAG_DATA_RECORD_TYPE() macro below in
     * your class source header file.
     *
     * @tparam T  data object class
     * @tparam Command  command to map
     */
    template <class T, type Command>
    struct toRecordType {
        enum {TYPE = 0};
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This macro maps a data object and tag data command to a record type.
 *
 * Example usage:
 * @code
 *   // declare what tag record types are supported for the label data object
 *   DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::READ_ALL, 'RT71' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::WRITE, 'RT9C' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::DELETE_ONE, 'RTA7' );
 *   DECLARE_TAG_DATA_RECORD_TYPE( label, tagDataCommand::DELETE_ALL, 'RT46' );
 * @endcode
 */
#define DECLARE_TAG_DATA_RECORD_TYPE( CLASS, COMMAND, RECORD_TYPE ) \
    class CLASS; \
    template <> struct tagDataCommand::toRecordType<CLASS, COMMAND> { \
        enum {TYPE = RECORD_TYPE}; \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TAGDATACOMMAND_H
