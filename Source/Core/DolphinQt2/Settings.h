// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <QSettings>

#include "Common/Logging/Log.h"
#include "Common/Logging/LogManager.h"
#include "Core/ConfigManager.h"
#include "DiscIO/Volume.h"

// UI settings to be stored in the config directory.
class Settings final : public QSettings
{
	Q_OBJECT

public:
	explicit Settings(QObject* parent = nullptr);

	// UI
	QString GetThemeDir() const;

	// GameList
	QString GetLastGame() const;
	void SetLastGame(const QString& path);
	QStringList GetPaths() const;
	void SetPaths(const QStringList& paths);
	void RemovePath(int i);
	QString GetDefaultGame() const;
	void SetDefaultGame(const QString& path);
	QString GetDVDRoot() const;
	void SetDVDRoot(const QString& path);
	QString GetApploader() const;
	void SetApploader(const QString& path);
	QString GetWiiNAND() const;
	void SetWiiNAND(const QString& path);
	DiscIO::IVolume::ELanguage GetWiiSystemLanguage() const;
	DiscIO::IVolume::ELanguage GetGCSystemLanguage() const;

	// Emulation
	bool GetConfirmStop() const;

	// Graphics
	bool GetRenderToMain() const;
	bool GetFullScreen() const;
	QSize GetRenderWindowSize() const;

	//Log
	LogTypes::LOG_LEVELS GetDefaultLogLevel() const;
	LogTypes::LOG_LEVELS GetLogLevel(LogTypes::LOG_TYPE) const;
	bool GetLogListener(LogListener::LISTENER listener) const;
	bool GetLogType(LogTypes::LOG_TYPE) const; //TODO: Rename GetLogTypeEnabled etc. ??
	bool GetLogWindowEnable() const;
	bool GetLogWrap() const;
	bool GetLogMonospace() const;
public slots:
	void SetLogListener(LogListener::LISTENER listener, bool enable) const;
	void SetDefaultLogLevel(LogTypes::LOG_LEVELS verbosity) const;
	void SetLogType(LogTypes::LOG_TYPE type, bool enable);
	void SetLogLevel(LogTypes:: LOG_TYPE, LogTypes::LOG_LEVELS verbosity) const;
	void SetLogWindowEnable(bool enable);
	void SetLogWrap(bool wrap);
	void SetLogMonospace(bool monospace);
	//WHERE SHOULD I USE CONST?!?!?!
};
