// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <QStringList>
#include <QFile>

#include "Common/CommonPaths.h"
#include "Common/FileUtil.h"
#include "DolphinQt2/Resources.h"

#include <QDebug>

QList<QPixmap> Resources::m_platforms;
QList<QPixmap> Resources::m_countries;
QList<QPixmap> Resources::m_ratings;
QList<QPixmap> Resources::m_misc;

int Resources::m_magnification;

void Resources::Init(int magnification)
{
	m_magnification = magnification;

	QStringList platforms{
		QStringLiteral("Platform_Gamecube"),
		QStringLiteral("Platform_Wii"),
		QStringLiteral("Platform_Wad"),
		QStringLiteral("Platform_File")
	};
	for (QString platform : platforms)
		m_platforms.append(GetPixmap(platform));

	QStringList countries{
		QStringLiteral("Flag_Europe"),
		QStringLiteral("Flag_Japan"),
		QStringLiteral("Flag_USA"),
		QStringLiteral("Flag_Australia"),
		QStringLiteral("Flag_France"),
		QStringLiteral("Flag_Germany"),
		QStringLiteral("Flag_Italy"),
		QStringLiteral("Flag_Korea"),
		QStringLiteral("Flag_Netherlands"),
		QStringLiteral("Flag_Russia"),
		QStringLiteral("Flag_Spain"),
		QStringLiteral("Flag_Taiwan"),
		QStringLiteral("Flag_International"),
		QStringLiteral("Flag_Unknown")
	};
	for (QString country : countries)
		m_countries.append(GetPixmap(country));

	QStringList ratings{
		QStringLiteral("rating0"),
		QStringLiteral("rating1"),
		QStringLiteral("rating2"),
		QStringLiteral("rating3"),
		QStringLiteral("rating4"),
		QStringLiteral("rating5")
	};
	for (QString rating : ratings)
		m_ratings.append(GetPixmap(rating));

	m_misc.append(GetPixmap(QStringLiteral("nobanner")));
	m_misc.append(GetPixmap(QStringLiteral("dolphin_logo")));
	m_misc.append(GetPixmap(QStringLiteral("Dolphin")));
}

QPixmap Resources::GetPlatform(int platform)
{
	return m_platforms[platform];
}

QPixmap Resources::GetCountry(int country)
{
	return m_countries[country];
}

QPixmap Resources::GetRating(int rating)
{
	return m_ratings[rating];
}

QPixmap Resources::GetMisc(int id)
{
	return m_misc[id];
}

QPixmap Resources::GetPixmap(QString name)
{
	QString sys_dir = QString::fromStdString(File::GetSysDirectory() + RESOURCES_DIR + DIR_SEP);
	QString path;
	//TODO: Include downscaled higher res?
	for (int i = m_magnification; i > 0; i--)
	{
        QString magnification_infix = (i == 1) ? QStringLiteral("") : QStringLiteral("@%1x").arg(i);
		path = sys_dir + name + magnification_infix + QStringLiteral(".png");
		if (QFile::exists(path))
			break;
	}
	qDebug() << path;
	qDebug() << m_magnification;
	QPixmap pixmap = QPixmap(path);
	pixmap.setDevicePixelRatio(m_magnification);
	return pixmap;
}
