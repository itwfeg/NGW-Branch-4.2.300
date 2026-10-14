#include "superReportingClassesValidation.h"

superReportingClassesValidation::superReportingClassesValidation()
{
}

bool superReportingClassesValidation::validEntries(int column, QString entry)
{
    if (column == 0)
    {
        if (entry.compare("[NO DATA]",Qt::CaseInsensitive) != 0)
        {
            // number column
            bool ok;
            int numericValue = entry.toInt(&ok);
            if (!ok)
            {
                return false;
            }
            if (numericValue < 0 || numericValue > 999999)
            {
                return false;
            }
        }
        else
            return false;
    }
    else if (column == 1)
    {
        if (entry.compare("[NO DATA]",Qt::CaseInsensitive) != 0)
        {
            // description field length is 0 then invalid
            if (entry.trimmed().length() == 0)
            {
                return false;
            }
            // TODO
            // There must be a max length for description
        }
        else
            return false;
    }
    return true;
}
