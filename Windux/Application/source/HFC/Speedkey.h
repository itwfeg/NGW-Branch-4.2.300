/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPEEDKEY_H
#define SPEEDKEY_H

#include "Data.h"
#include "StringType.h"
#include "NumType.h"

class Report;

/**
 * DPS-5.11
 * This functionality was a part of the Ultima.  It will no longer be supported in the Dominator. 
 */
class Speedkey : Data
{
   public:

   NumType& getNum();

   void setNum(NumType& num);

   int getFlags();

   void setFlags(int flags);

   StringType& getMacro();

   void setMacro(StringType& macro);

   StringType& getDescription();

   void setDescription(StringType& description);

   private:
   
   NumType num;
   int flags;
   StringType macro;
   StringType description;
   
   public:

   class Type
   {
      private:
	   int INVALID;
      int PLU;
      int OPERATOR;
      int MACRO;
   };
   
};

#endif //SPEEDKEY_H
