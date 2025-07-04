#pragma once
#include "Niflect/Base/String.h"
#include "qstring.h"

QString CStringToQString(const Niflect::CString& str);
Niflect::CString QStringToCString(const QString& qstr);