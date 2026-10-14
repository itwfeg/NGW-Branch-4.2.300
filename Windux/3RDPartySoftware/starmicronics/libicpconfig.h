#ifndef LIBICPCONFIG_H
#define LIBICPCONFIG_H

#include <string>

using namespace std;

/*! \defgroup libICPConfig libICPConfig
    
    @~english
    \brief A C library to retreive configuration settings stored in a local file.

    The useage is to provide one of the keys below and get back a value for that setting.
    If you want to attempt to retrive one of the values as an int, you can call GetInt instead of GetString.
    More doc help below.

    \section libicperrors Int Error Returns
     -1 = Undefined Error\n
      0 = Success (No Error)\n
      1 = Config File Not Found\n
      2 = Key name is not in the config file\n
      3 = Key exists but no value was set\n
      4 = Max length is too small (buffer overrun)\n
      5 = Value cannot be converted to an int\n

    \section icpconfiguration iCP Settings Configuration
    To get values from this file by key, use:\n
    <b>iCPConfig key</b>\n\n
    This binary console application will call a key value pair from the /etc/icp.conf file.\n\n
    \subsection editicpconf Manually Edit of icp.conf

    Find your target iCP configuration file (Default is /etc/icp.conf) and open it sudo with nano, vi, or any text editor.\n
    You can edit it by hand to change key value pairs. Note that all built in configuration options are all commented out with the # symbol and one uncommented k->v pair that will be the actual value the key is set to. We highly recommend that you copy this config file before editing as to avoid config file corruption. Commenting or uncommenting certain settings may break your iCP system so be aware what is being changed.\n\n

    @~japanese
    \brief ローカルファイルに保存されている構成設定を取り出すC-ライブラリーです

    使用する方法は、下記のキーを入力し、設定値を返してもらいます。
    整数としての設定値を返えしてもらいたい場合、GetStringの代わりにGetIntを実行してください。
    詳細は下記に書いてあります。

    \section libicperrors Int エラーメッセージ
     -1 = 不明なエラー \n
      0 = 成功 (エラーが発生されていない)\n
      1 = 設定ファイルが見つかりません  \n
            2  =  キー名が設定ファイルに入っていません。 \n
            3  =  キーが存在していますが、値が設定されていません。\n
            4  =  キーの最大長が足りません。 (バッファーオーバーフロー) \n
      5 = 値を整数に変更することができません \n

    \section icpconfiguration iCPの設定構成
        このファイルからキーで値を返すには, ￥nを使用してください。\n
    <b>iCPConfig key</b>\n\n
        このバイナリーコンソールアプリケーションがキーのペアを /etc/icp.confファイルから取り出します。  \n\n
    \subsection editicpconf 手動で icp.confの編集

    iCP構成ファイルを選択し、 (デフォルトで /etc/icp.conf) Nano、またはVi, 他テキスト編集プログラムで開いてください。 \n
    手動でキーのペアを変更できます。 すべてのデフォルト構成オプションは、「＃」で示されています。 
    一つのコメントもないキーのペアもあります（この値がキーに設定されています）。  構成ファイルの破損を防ぐために、編集する前に必ずコピーしてください。 設定は、
    慎重に行ってください。\n\n

    \subsection icp.conf Default sdk config file
# iCP Config -> Printer models\n
# PRINTER_MODEL="ICP"\n
PRINTER_MODEL="TSP100GT"\n
# PRINTER_MODEL="TSP100"\n\n

# iCP Config -> Printer Cut Mech Settings\n
PRINTER_CUT="FULL"\n
# PRINTER_CUT="PARTIAL"\n
# PRINTER_CUT="NONE"\n\n

# Printer speed settings\n
PRINTER_SPEED="FULL"\n
# PRINTER_SPEED="MEDIUM"\n
# PRINTER_SPEED="LOW"\n\n

# Dither setting ("NONE" or "ERROR")\n
PRINTER_DITHER="NONE"\n
# PRINTER_DITHER="ERROR"\n\n

# Density setting (Int Value)\n
PRINTER_DENSITY="0"\n\n

# Paper Type (0 to 3)\n
PRINTER_PAPER="0"\n\n

# Printer Paper Width (640, 576, 420, 416, 406, etc.) (dots per line)\n
PAPER_WIDTH="576"\n\n

# Grey Levels (1 to 32)\n
# GREY_LEVELS="1"\n
GREY_LEVELS="8"\n
# GREY_LEVELS="16"\n
# GREY_LEVELS="32"\n\n

# StarIO Default Connection Hook\n
STARIO_PORT="usbprn:"\n

# StarIO Default Settings\n
STARIO_SETTINGS=""\n\n

  \ingroup libICPConfig
  \{

  \file libicpconfig.h
  \author  GFDT - Joshua Copeland
  \version 1.0
  
  @~english
  \brief A C library to retrive configuration settings stored in a local file.

  Header file which exports the libICPConfig C API.

  @~japanese
  \brief ローカルファイルに保存されている構成設定を取り出すC-ライブラリーです。

  libICPConfig C APIをエクスポートするヘッダーファイル。

*/

#define ICP_CONFIG_FILE "/etc/icp.conf"             /// \todo Add support for alternate files specified by the client application

/// \cond internal
#define PRINTER_MODEL           "PRINTER_MODEL"
#define PRINTER_CUT             "PRINTER_CUT"
#define PRINTER_SPEED           "PRINTER_SPEED"
#define PRINTER_DITHER          "PRINTER_DITHER"
#define PRINTER_DENSITY         "PRINTER_DENSITY"
#define PRINTER_PAPER           "PRINTER_PAPER"
#define PAPER_WIDTH             "PAPER_WIDTH"
#define GREY_LEVELS             "GREY_LEVELS"
#define STARIO_PORT             "STARIO_PORT"
#define STARIO_SETTINGS         "STARIO_SETTINGS"
#define MECH_HANDLER_CHANNELS   "MECH_HANDLER_CHANNELS"
#define SPOOLER_WATCH           "SPOOLER_WATCH"
#define CUSTOM_EXTENSION        "CUSTOM_EXTENSION"
#define PAGE_BUFFERING          "PAGE_BUFFERING"
#define SPOOLER_CHANNELS        "SPOOLER_CHANNELS"
#define STAR_WEB_CLOSE_PASSWORD "STAR_WEB_CLOSE_PASSWORD"
#define START_WEB_PAGE          "START_WEB_PAGE"
#define WEB_FILE_MONITOR        "WEB_FILE_MONITOR"
#define JOURNAL_FOLDER_LOCATION "JOURNAL_FOLDER_LOCATION"
/// \endcond

#define MAX_LINE_LEN    255

    #ifdef __cplusplus
    extern "C"
    {
    #endif

        /** \fn int GetString(char const key[], char * value, int maxLen, int * error)
            
	    @~english
	    \brief This function reads the configuration file for a specific key->value string.

            \section Notes
            Max length will prevent buffer overrun on the value char array, however if this does happen...
            An error will return notifying the buffer is not large enough to hold the value.
            One fix would be to call this once just to get back the length of the char array for that given key->value
            Most of these value string are not very long so this should not be an issue.

            \param key      const char array with the key name Ex. (STARIO_PORT or PRINTER_SPEED)
            \param value    pointer to a value that we will change to the key->value
            \param maxLen   max length specifies the maximum chars our value char array can hold
            \param error    pointer to a int that will be set with an error number if one arises
            \return The total length of the key->value string.

	    @~japanese
	    \brief この関数はキー値を見つけるために構成ファイルから読み込みます。

            \section Notes
            文字配列の最大値を設定することで、バッファーオーバーフローを防ぎます。

            \param key      キー名：Ex. (STARIO_PORT or PRINTER_SPEED)で定文字配列
            \param value    値をキー値に変更するポインタ
            \param maxLen   文字配列の最大文字数を定義します。
            \param error    整数にエラーを設定するポインタ（エラーが発生した場合）
            \return         キー値の合計の長さが返り値になります。
        */
        int GetString(char const key[], char * value, int maxLen, int * error);

        int GetStringExt(char const fileName[],char const key[], char * value, int maxLen, int * error);

        void SetString(const char * key, const char * setting, int * error);

        void SetStringExt(const char * fileName,const char * key, const char * setting, int * error);


        /** \fn int GetInt(char const key[], int * error)
            
	    @~english
	    \brief This function reads the configuration file for a specific key->value int.

            \section Notes
            Max int size should not be an issue for most settings...

            \param key      const char array with the key name Ex. (STARIO_PORT or PRINTER_SPEED)
            \param error    pointer to a int that will be set with an error number if one arises
            \return The int value of the key specified.

            @~japanese
	    \brief この関数は、キー値から整数を読み込みます。

            \section Notes
            最大整数サイズは、ほとんどの設定で問題になりません。

            \param key      キー名： Ex. (STARIO_PORT or PRINTER_SPEED)で定文字配列
            \param error    整数にエラーを設定するポインタ （エラーが発生した場合）
            \return         キーの整数値が返り値になります。

        */
        int GetInt(char const key[], int * error);
 int GetIntExt(char const fileName[], char const key[], int * error);

        /** \cond internal */

            /**
                \fn void GetAllKeys()
                
		@~english
		\brief This function outputs all the available default key names (std::cout all icp config related #defines).

		@~japanese
		\brief この関数は、全デフォルトキー名を出力します。 (std::cout すべてのICP構成に関する#defines).
            */
            void GetAllKeys();


            /**
                \fn void OutputError(int errorValue)
                
		@~english
		\brief This function uses the error value int to determine what the error code means, once found it is outputted to std::cerr.

		@~japanese
		\brief この関数は、エラーメッセージの意味を確認するためにstd::cerrに出力します.
            */
            void OutputError(int errorValue);

        /** \endcond */

    #ifdef __cplusplus
    }
    #endif

#endif // LIBICPCONFIG_H

/// \}
