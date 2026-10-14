/**
    @defgroup libStarMessage libStarMessage

    @~english
        Used by applications to create IPC channels to which they can Send and Receive
        StarMessage structs that contain StarDataBlocks with print data. StarMessage can be used
        to listen, read, or both on an interface defined within.

    @~japanese
        IPCチャネルを作成するために使用されます。
        このICPチャネルには、StarDataBlocksが含まれているStarMessageの構造体を送受信できます。

    @~
    @ingroup libStarMessage
    @{

    @~english
        Inter Process Communication for Star ICP components.

    @~japanese
        インタープロセスコミュニケーションによるICP通信

    Header file which exports the libStarMessage C API.
 */


#ifndef LIBSTARMESSAGE_H
#define LIBSTARMESSAGE_H


// Define Const Data
/// \cond internal
#define TCPPortHandlerToCommandEmulator          000
#define USBPortHandlerToCommandEmulator          001
#define JavaPOSPortHandlerToCommandEmulator      010
#define CommandEmulatorToJobHandler_FromTCP      100
#define CommandEmulatorToJobHandler_FromUSB      101
#define FolderMonitorToJobHandler                102
#define CommandEmulatorToJobHandler_FromJavaPOS  110
#define JobHandlerToMasterJobHandler_FromTCP     200
#define JobHandlerToMasterJobHandler_FromUSB     201
#define JobHandlerToMasterJobHandler_FromJavaPOS 210
#define QAsuraToMechHandler                      320
#define AsuraSpmToMechHandler                    330
#define JAsuraToMechHandler                      432
#define PrinterTransactionToMechHandler          350
#define MechHandlerToQAsura                      520
#define MechHandlerToAsuraSpm                    530
#define MechHandlerToJAsura                      433
#define MechHandlerToPrinterTransaction          550
#define JobHandlerToFolderMonitor                602
/// \endcond


#ifdef __cplusplus
extern "C"
{
#endif


/**
    StarDataBlock is used to hold the data for StarMessage
*/
typedef struct
{
        int shmid;                /**< An internal id that is used to connect the current applications access to the shared memory segment.*/
        unsigned int type;        /**< Used to describe the data blocks use */
        int dataLength;           /**< An array of data sizes for the shared memory or d-bus array read data length. */
        unsigned char *data;      /** A pointer to the shared memory segment or loaded with d-bus read array. */
        int paramLength;          /**< The length of each parameter read through d-bus read array. */
        unsigned char *parameter; /**< The parameters read from d-bus read array. */
} StarDataBlock;


/**
    StarMessage container
*/
typedef struct
{
        unsigned int type;          /**< A way to identify what type of StarMessage this is*/
        unsigned int num_of_blocks; /**< Number of StarDataBlocks encapsulated witin StarMessage*/
        StarDataBlock *datablock;   /**< The StarDataBlocks */
} StarMessage;


/**
    @~english
        Creates a bus for start IPC communication.

    @return
        0 = Success, 0 > Failed

    @~japanese
        StarMessageを利用するために、最初に実行します。

    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_CreateBus();


/**
    @~english
        Creates a channel to listen for IPC messages on.

    @param ChannelNumber
        A Channel Number that has not been created on previously.
    @return
        0 = Success, 0 > Failed

    @~japanese
        IPCのメッセージを受信するチャネルを作成します。
        StarMessage_CreateBus()が実行されていない場合、最初に呼び出します。

    @param ChannelNumber
        StarMessageの受信に使用するチャネル番号を指定します。
    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_CreateChannel( int ChannelNumber );


/**
    @~english
        If the StarMessage is not NULL, a StarMessage struct will contain the last message passed from another application sending on the same interface.

    @param ChannelNumber
        The channel number you set on CreateChannel.
    @return
        Returns a StarMessage struct containing StarDataBlocks.
    
    @~japanese
        StarMessageがNULLでなければ、同じインタフェースで最後に他アプリケーションから渡されたメッセージがStarMessageの構造体に含まれます。

    @param ChannelNumber
        StarMessageを受信したチャネル番号が代入されます。
    @return
        StarDataBlocksが含まれているStarMessageの構造体が返り値になります。
*/
StarMessage* StarMessage_ReadNext( int *ChannelNumber );


/**
    @~english
        This will destroy a channel that you were previously listening for StarMessages on.

    @param ChannelNumber
        The channel number you want to destroy.
    @return
        0 = Success 0 > Failed

    @~japanese
        StarMessagesを受信していたチャネルを削除します。
        最後のチャネルが削除された時、StarMessage_DeleteMessage()も実行します。

    @param ChannelNumber
        削除するチャネル番号を指定します。
    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_DestroyChannel( int ChannelNumber );


/**
    @~english
        This will create a channel to send StarMessages on.

    @param ChannelNumber
        A Bus Name that has not been created on previously. (defined busnames & interfaces)
    @return
        0 = Success 0 > Failed

    @~japanese
        StarMessagesを送信するチャネルを作成します。
        StarMessage_CreateBus()が実行されていない場合、最初に呼び出します。

    @param ChannelNumber
        StarMessageの送信に使用するチャネル番号を指定します。
    @return
        0 = 成功, 0 > 失敗
*/ 
int StarMessage_OpenChannel( int ChannelNumber );


/**
    @~english
        This will send a StarMessage through D-BUS on the previously created ID handle.

    @param *message
        StarMessage container to send.
    @param ChannelNumber
        The handle you received back from CreateChannel.
    @return
        0 = Success 0 > Failed

    @~japanese
        作成されたIDハンドルで、DーBUSを通じてStarMessageを送信します。

    @param *message
        送信するStarMessage構造体を指定します。
    @param ChannelNumber
        送信に使用するチャネル番号を指定します。
    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_Send( StarMessage *message, int ChannelNumber );


/**
    @~english
        This is used to create a StarMessage with the correct StarDataBlock sizes to occupy with your applications data.

    @param *dataLen
        An array of data lengths you wish to create in the StarDataBlocks.
    @param *paramLen
        An array of parameters sizes that are stored in StarDataBlock.
    @param dataBlockCount
        The handle you received back from CreateChannel.
    @return
        Empty StarMessage container with allocated StarDataBlocks.

    @~japanese
        アプリケーションデータを送信するため、 StarDataBlockのサイズを指定してStarMessageを作成します。

    @param *dataLen
        StarDataBlocksで作成するデータ長の配列
    @param *paramLen
        StarDataBlockに保存されるパラメータサイズの配列
    @param dataBlockCount
        CreateChannelから受信したハンドル
    @return
        StarDataBlocksが割り当てられた空のStarMessage構造体
*/
StarMessage* StarMessage_CreateMessage( int *dataLen, int *paramLen, unsigned int dataBlockCount );


/**
    @~english
        This is a method to add a data block to a StarMessage.

    @param *message
        StarMessage container that is reallocated.
    @param dataLen
        An array of data lengths you wish to create in the StarDataBlocks.
    @param paramLen
        An array of parameters sizes that are stored in StarDataBlock.
    @return
        StarMessage container reallocated with the additional StarDataBlocks.

    @~japanese
        StarMessage構造体にデータ・ブロックを追加するメソッドです。

    @param *message
        データ・ブロックを追加するStarMessage構造体
    @param dataLen
        追加するデータ配列のサイズ
    @param paramLen
        追加するパラメータ配列のサイズ
    @return
        データ・ブロックが追加されたStarMessage構造体
*/
StarMessage* StarMessage_AddDataBlock(StarMessage *message, int dataLen, int paramLen);


/**
    @~english
        This will close a channel you previously opened to send messages on.

    @param ChannelNumber
        The previously created Handle from OpenChannel.
    @return
        0 = Success 0 > Failed

    @~japanese
        メッセージを送信するために開いていたチャネルを閉じます。
        最後のチャネルを閉じた時、StarMessage_DeleteMessage()も実行します。

    @param ChannelNumber
        削除するチャネル番号を指定します。
    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_CloseChannel( int ChannelNumber );


/**
    @~english
        Very important to avoid memory leak with shared memory segments.
        Always delete a message at the end of its life.
        If a process will just read and send the message, do not use this to delete the message.
        Instead just continue to send the message.

    @param *message
        The StarMessage with live shared memory segments that need to be released.
    @return
        0 = Success 0 > Failed

    @~japanese
        共有メモリ部分でメモリリークを防ぐのに重要です。利用しないメッセージを必ず削除してください。
        もしデータをパススルーだけの場合、この関数を呼ばずに、そのままメッセージを送信してください。

    @param *message
        使用されている共有メモリ部分をクリアすべきStarMessage構造体。
    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_DeleteMessage(StarMessage *message);


/**
    @~english
        Deletes a bus to finish IPC communication.

    @return
        0 = Success, 0 > Failed

    @~japanese
        StarMessageの利用を終了するために実行します。
        またチャネルが存在する場合、全てのチャネルを削除します。

    @return
        0 = 成功, 0 > 失敗
*/
int StarMessage_DeleteBus();


#ifdef __cplusplus
}
#endif


#endif // LIBSTARMESSAGE_H


/// @}
