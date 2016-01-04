// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <QColor>
#include <QDockWidget>
#include <QObject>
#include <QString>
#include <QTextEdit>
#include <QWidget>
#include <mutex>
#include <queue>
#include <utility>

#include "Common/Logging/LogManager.h"

class LogViewer final : public QTextEdit, LogListener
{
	Q_OBJECT

public:
	explicit LogViewer(QWidget* parent);
	~LogViewer();
	void Log(LogTypes::LOG_LEVELS, const char* msg) override;

private slots:
	void displayLog();

private:
	QColor LevelColor(LogTypes::LOG_LEVELS level);
	std::mutex m_LogEntry;
	std::queue<std::pair<LogTypes::LOG_LEVELS, QString>> msgQueue;
};

class LogDock : public QDockWidget
{
	Q_OBJECT
private:
	LogViewer* log_viewer;
public:
	explicit LogDock(const QString& title, QWidget* parent);
	bool event(QEvent* event) override;

public slots:
	void ToggleLogViewer(bool display);
	void ToggleWrap(bool wrap);
	void ToggleMonospace(bool monospace);
};
