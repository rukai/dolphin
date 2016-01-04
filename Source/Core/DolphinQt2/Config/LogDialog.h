// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLayout>
#include <QObject>
#include <QRadioButton>
#include <QScrollArea>

#include "Common/IniFile.h"
#include "Common/Logging/Log.h"
#include "Common/Logging/LogManager.h"
#include "DolphinQt2/LogViewer.h"

class LogDialog final : public QDialog
{
	Q_OBJECT
public:
	explicit LogDialog(QWidget* parent = nullptr);

private:
	QGroupBox* TextFormatting();
	QGroupBox* DefaultLogLevel();
	QGroupBox* Output();
	QGroupBox* LogTypes();
	QHBoxLayout* LogTypeReset();
	QComboBox* TypeLevelComboBox();
	QString GetLevelName(LogTypes::LOG_LEVELS levels);
	QString GetListenerName(LogListener::LISTENER listener);

	QButtonGroup* levels_group;
	QButtonGroup* listeners_group;
	QButtonGroup* types_group;

private slots:
	void SetDefaultLogLevel(int value);
	void SetListener(int listener, bool enable);
	void SetType(int type, bool enable);
	void EnableAllTypes();
	void DisableAllTypes();

signals:
	void ToggleWrap(bool);
	void ToggleMonospace(bool);
};
