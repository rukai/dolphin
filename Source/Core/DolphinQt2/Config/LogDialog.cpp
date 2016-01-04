// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>

#include "Common/IniFile.h"
#include "Common/Logging/Log.h"
#include "Common/Logging/LogManager.h"
#include "DolphinQt2/Config/LogDialog.h"
#include "DolphinQt2/LogViewer.h"
#include "DolphinQt2/Settings.h"

#include <QDebug>

LogDialog::LogDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Log Configuration"));
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumHeight(700); //take from resolution?
	setMinimumWidth(330); //take from children size. this is really bad, scrollbars pop up on some Qt themes.

	QVBoxLayout* layout = new QVBoxLayout();
	
	layout->addWidget(TextFormatting());
	layout->addWidget(Output());
	layout->addWidget(DefaultLogLevel());
	layout->addWidget(LogTypes());
	layout->addLayout(LogTypeReset());

	QDialogButtonBox* dialog_buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(dialog_buttons, &QDialogButtonBox::accepted, this, &LogDialog::accept);
	layout->addWidget(dialog_buttons);
	setLayout(layout);
}

QGroupBox* LogDialog::TextFormatting()
{
	QGroupBox* group_box = new QGroupBox(tr("Text Formatting"));
	QVBoxLayout* layout = new QVBoxLayout();
	group_box->setLayout(layout);

	QCheckBox* wrap_check = new QCheckBox(tr("Wrap"));
	wrap_check->setChecked(Settings().GetLogWrap());
	connect(wrap_check, SIGNAL(toggled(bool)), this, SIGNAL(ToggleWrap(bool)));
	connect(this, SIGNAL(ToggleWrap(bool)), new Settings(), SLOT(SetLogWrap(bool)));

	QCheckBox* monospace_check = new QCheckBox(tr("Monospace Font"));
	connect(monospace_check, SIGNAL(toggled(bool)), this, SIGNAL(ToggleMonospace(bool)));
	connect(this, SIGNAL(ToggleMonospace(bool)), new Settings(), SLOT(SetLogMonospace(bool)));
	monospace_check->setChecked(Settings().GetLogMonospace());

	layout->addWidget(wrap_check);
	layout->addWidget(monospace_check);
	return group_box;
}

QGroupBox* LogDialog::DefaultLogLevel()
{
	QGroupBox* group_box = new QGroupBox(tr("Default Verbosity"));
	QVBoxLayout* layout = new QVBoxLayout();
	group_box->setLayout(layout);

	levels_group = new QButtonGroup();
	for (int i = 1; i <= MAX_LOGLEVEL; i++)
	{
		LogTypes::LOG_LEVELS level = static_cast<LogTypes::LOG_LEVELS>(i);
		QRadioButton* button = new QRadioButton(GetLevelName(level));
		layout->addWidget(button);
		levels_group->addButton(button, i);
	}
	connect(levels_group, SIGNAL(buttonClicked(int)), this, SLOT(SetDefaultLogLevel(int)));

	int level = Settings().GetDefaultLogLevel();
	QRadioButton* button = static_cast<QRadioButton*>(levels_group->button(level));
	button->setChecked(true);

	return group_box;
}

void LogDialog::SetDefaultLogLevel(int value)
{
	LogTypes::LOG_LEVELS level = static_cast<LogTypes::LOG_LEVELS>(value);
	Settings().SetDefaultLogLevel(level);
}

QString LogDialog::GetLevelName(LogTypes::LOG_LEVELS levels)
{
	switch (levels)
	{
	case LogTypes::LNOTICE: return tr("Notice");
	case LogTypes::LERROR: return tr("Error");
	case LogTypes::LWARNING: return tr("Warning");
	case LogTypes::LINFO: return tr("Info");
	case LogTypes::LDEBUG: return tr("Debug");
	default: return QStringLiteral("DEADBEEF");
	}
}

QGroupBox* LogDialog::Output()
{
	QGroupBox* group_box = new QGroupBox(tr("Output"));
	QVBoxLayout* layout = new QVBoxLayout();
	group_box->setLayout(layout);
	
	listeners_group = new QButtonGroup();
	listeners_group->setExclusive(false);
	for (int i = 0; i < LogListener::NUMBER_OF_LISTENERS; i++)
	{	
		LogListener::LISTENER listener = static_cast<LogListener::LISTENER>(i);
		QCheckBox* button = new QCheckBox(GetListenerName(listener));
		layout->addWidget(button);
		listeners_group->addButton(button, i);
		button->setChecked(Settings().GetLogListener(listener));
	}
	connect(listeners_group, SIGNAL(buttonToggled(int, bool)), this, SLOT(SetListener(int, bool)));
	return group_box;
}

void LogDialog::SetListener(int listener, bool enable)
{
	Settings().SetLogListener(static_cast<LogListener::LISTENER>(listener), enable);
}

QString LogDialog::GetListenerName(LogListener::LISTENER listener)
{
	switch (listener)
	{
	case LogListener::FILE_LISTENER: return tr("File");
	case LogListener::CONSOLE_LISTENER: return tr("Console");
	case LogListener::LOG_WINDOW_LISTENER: return tr("Window");
	default: return QStringLiteral("DEADBEEF");
	}
}

QHBoxLayout* LogDialog::LogTypeReset()
{
	QHBoxLayout* layout = new QHBoxLayout();

	QPushButton* on = new QPushButton(tr("All On"));
	QPushButton* off = new QPushButton(tr("All Off"));
	layout->addWidget(on);
	layout->addWidget(off);
	connect(on, SIGNAL(clicked(bool)), this, SLOT(EnableAllTypes()));
	connect(off, SIGNAL(clicked(bool)), this, SLOT(DisableAllTypes()));

	return layout;
}

QGroupBox* LogDialog::LogTypes()
{
	QGroupBox* group_box = new QGroupBox(tr("Log Types"));
	QLayout* gb_layout = new QVBoxLayout();
	QScrollArea* scroll_area = new QScrollArea();
	QWidget* sa_contents = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout();

	group_box->setLayout(gb_layout);
	gb_layout->addWidget(scroll_area);
	scroll_area->setWidgetResizable(true);
	scroll_area->setWidget(sa_contents);
	sa_contents->setLayout(layout);
	
	types_group = new QButtonGroup();
	types_group->setExclusive(false);
	for (int i = 0; i < LogTypes::NUMBER_OF_LOGS; i++)
	{
		QHBoxLayout* type_layout = new QHBoxLayout();
		LogTypes::LOG_TYPE type = static_cast<LogTypes::LOG_TYPE>(i);
		QString logType = QString::fromStdString(LogManager::GetInstance()->GetFullName(type));
		QCheckBox* button = new QCheckBox(logType);
		QComboBox* level = TypeLevelComboBox();

		type_layout->addWidget(button);
		type_layout->addWidget(level);
		layout->addLayout(type_layout);
		types_group->addButton(button, i);
		button->setChecked(Settings().GetLogType(type));
	}
	connect(types_group, SIGNAL(buttonToggled(int, bool)), this, SLOT(SetType(int, bool)));
	return group_box;
}

void LogDialog::EnableAllTypes()
{
	for(int i = 0; i < LogTypes::NUMBER_OF_LOGS; i++)
	{
		QAbstractButton* button = types_group->button(i);
		button->setChecked(true);
	}
	//call settings
}

void LogDialog::DisableAllTypes()
{
	for(int i = 0; i < LogTypes::NUMBER_OF_LOGS; i++)
	{
		QAbstractButton* button = types_group->button(i);
		button->setChecked(false);
	}
	//call settings
}

void LogDialog::SetType(int type, bool enable)
{
	Settings().SetLogType(static_cast<LogTypes::LOG_TYPE>(type), enable);
}

QComboBox* LogDialog::TypeLevelComboBox()
{
	QComboBox* combo_box = new QComboBox();
	combo_box->addItem(tr("Default"));
	for (int i = 1; i <= MAX_LOGLEVEL; i++)
	{
		LogTypes::LOG_LEVELS level = static_cast<LogTypes::LOG_LEVELS>(i);
		combo_box->addItem(GetLevelName(level));
	}
	return combo_box;
}
