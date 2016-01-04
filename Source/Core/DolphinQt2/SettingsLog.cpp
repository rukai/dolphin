// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "Common/FileUtil.h"
#include "Common/Logging/Log.h"
#include "Common/Logging/LogManager.h"
#include "Core/ConfigManager.h"
#include "DolphinQt2/Settings.h"

#include <QDebug>

bool Settings::GetLogListener(LogListener::LISTENER listener) const
{
	return LogManager::GetInstance()->GetEnableListener(listener);
}

void Settings::SetLogListener(LogListener::LISTENER listener, bool enable) const
{
	LogManager::GetInstance()->SetEnableListener(listener, enable);
}

LogTypes::LOG_LEVELS Settings::GetDefaultLogLevel() const
{
	return LogManager::GetInstance()->GetDefaultLogLevel();
}

void Settings::SetDefaultLogLevel(LogTypes::LOG_LEVELS level) const
{	
	LogManager::GetInstance()->SetDefaultLogLevel(level);
}

LogTypes::LOG_LEVELS Settings::GetLogLevel(LogTypes::LOG_TYPE type) const
{
	//retrieve from LogManager - which retrives from ini in constuctor
	return LogManager::GetInstance()->GetLogLevel(type);
}

void Settings::SetLogLevel(LogTypes::LOG_TYPE type, LogTypes::LOG_LEVELS level) const
{	
	//update LogManager - which saves to ini
	LogManager::GetInstance()->SetLogLevel(type, level);
}

bool Settings::GetLogWrap() const
{
	return value(QStringLiteral("Log/Wrap"), false).toBool();
}

void Settings::SetLogWrap(bool wrap)
{
	setValue(QStringLiteral("Log/Wrap"), wrap);
}

bool Settings::GetLogMonospace() const
{
	return value(QStringLiteral("Log/Monospace"), true).toBool();
}

void Settings::SetLogMonospace(bool monospace)
{
	setValue(QStringLiteral("Log/Monospace"), monospace);
}

bool Settings::GetLogWindowEnable() const
{
	return value(QStringLiteral("Log/Enable"), false).toBool();
}

void Settings::SetLogWindowEnable(bool enable)
{
	setValue(QStringLiteral("Log/Enable"), enable);
}

bool Settings::GetLogType(LogTypes::LOG_TYPE type) const
{
	return LogManager::GetInstance()->GetEnableType(type);
}

void Settings::SetLogType(LogTypes::LOG_TYPE type, bool enable)
{
	LogManager::GetInstance()->SetEnable(type, enable);
}
