#include "Qt/Util/CoreUtil.h"

QString CStringToQString(const Niflect::CString& str)
{
	return QString::fromLocal8Bit(str.c_str());
}
Niflect::CString QStringToCString(const QString& qstr) {
	QByteArray byteArray = qstr.toUtf8();
	return Niflect::CString(byteArray.constData(), byteArray.size());
}